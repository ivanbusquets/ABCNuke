/*
 * ABCReadGeo.cpp - An Alembic geometry reader for Nuke
 *
 *  Created on: Oct 24, 2011
 *  Author: Ivan Busquets
 *
 */


// DDImage headers
#include "DDImage/SourceGeo.h"
#include "DDImage/Knobs.h"
#include "DDImage/TableKnobI.h"

// Alembic headers
#include "Alembic/AbcCoreHDF5/All.h"
#include "Alembic/AbcGeom/All.h"
#include "Alembic/Abc/All.h"

// ABCNuke helpers
#include "ABCNuke_ArchiveHelper.h"
#include "ABCNuke_GeoHelper.h"
#include "ABCNuke_MatrixHelper.h"

// std libs
#include <iostream>


#define _FPS 24.0  // Hard code a base of 24fps. Is there a way to get this from the project settings?

using namespace DD::Image;
using namespace Alembic::AbcGeom;

static const char* const interpolation_types[] = { "off", "linear" , 0};
static const char* const timing_types[] = { "original timing", "retime", 0};


static const char* nodeClass = "ABCReadGeo";


class ABCReadGeo : public SourceGeo
{
	const char* 				m_filename;
	unsigned 					m_version;
	int 						interpolate;
	IArchive 					archive;
	Knob* 						p_tableKnob;
	Table_KnobI* 				p_tableKnobI;
	int							timing;
	int							m_first;
	int							m_last;
	float						m_frame;
	float						m_sampleFrame;
	std::vector<bool>			active_objs;
	std::vector<bool>			bbox_objs;


public:
	ABCReadGeo(Node* node): SourceGeo(node) {
		m_filename = "";
		m_version = 0;
		interpolate = 0;
		p_tableKnob = NULL;
		p_tableKnobI = NULL;
		timing = 0;
		m_first = m_last = 1;
		m_frame = 1;
		m_sampleFrame = 1;

	}

	virtual void knobs(Knob_Callback f);
	int knob_changed(DD::Image::Knob* k);
	void _validate(bool for_real);
	virtual const char* Class() const {return nodeClass;}
	static const Op::Description description;

	void updateTableKnob();
	void updateTimingKnobs();


protected:
	const char * 	filename () const {return m_filename;}
	virtual void create_geometry(Scene& scene, GeometryList& out);
	virtual void append(Hash& hash);
	virtual void get_geometry_hash();


};

// *****************************************************************************
// 	VALIDATE : clamp outputContext and call _validate on base class
// *****************************************************************************

void ABCReadGeo::_validate(bool for_real)
{
	// original timing
	if (knob("timing")->get_value() == 0) {
		m_sampleFrame = clamp(outputContext().frame(), m_first, m_last);
	}
	//retime
	else {
		m_sampleFrame = clamp(knob("frame")->get_value(), m_first, m_last) ;
	}

	SourceGeo::_validate(for_real);

}


// *****************************************************************************
// 	KNOBS : Implement the file, timing, and table knobs
// *****************************************************************************

void ABCReadGeo::knobs(Knob_Callback f)
{
	// File knobs
	File_knob(f, &m_filename, "file", "file", Geo_File);
	Button(f, "Reload");

	// Set up the common SourceGeo knobs.
	SourceGeo::knobs(f);

	// Timing knobs
	BeginGroup(f, "TimingGroup");
	Enumeration_knob(f, &interpolate, interpolation_types, "interpolation");

	Enumeration_knob(f, &timing, timing_types, "timing");

	Int_knob(f, &m_first, "first");
	Tooltip(f, "Frame of the first sample in the Alembic archive.\n"
			"You can change this to clamp the animation to a smaller framerange\n");

	Int_knob(f, &m_last, "last");
	Tooltip(f, "Frame of the last sample in the Alembic archive.\n"
			"You can change this to clamp the animation to a smaller framerange\n");
	ClearFlags(f, Knob::STARTLINE);

	Float_knob(f, &m_frame, "frame");
	Tooltip(f, "Frame the animation will be sampled from.\n"
			"You can set this to a static frame, an animated curve,\n"
			"or an expression like 'frame/2' to effectively retime the animation\n"
			"of the Alembic archive.\n\n"
			"This value will be clamped to the 'first' and 'last' values above.");
	ClearFlags(f, Knob::SLIDER);

	EndGroup(f);

	Divider(f);

	// Object management knobs
	Button(f, "activate_selection", "Activate sel.");
	Tooltip(f, "Activate selected objects\n");

	Button(f, "deactivate_selection", "Dectivate sel.");
	Tooltip(f, "Deactivate selected objects\n");

	Button(f, "bbox_selection", "Bbox sel.");
	Tooltip(f, "Set selected objects to bbox mode\n");

	Button(f, "unbbox_selection", "UnBbox sel.");
	Tooltip(f, "Unset bbox mode for selected objects\n");

	p_tableKnob = Table_knob(f, "Obj_list", "Object list");
	SetFlags(f, Knob::STARTLINE);
	Tooltip(f, "List of objects in the Alembic archive.\n"
			"For each object, the following toggles are available:\n"
			"<b>Active:</b> Enable/disable that particular object. Disabled objects will not be read from the Alembic archive.\n"
			"<b>Bbox:</b> Choose whether to read the full geometry or just a bbox of each object.\n"
			"<b>Attributes:</b>");


	// Disable/enable "frame" knob based on choice in "timing" knob
	Knob* _pTimingKnob = knob("timing");
	Knob* _pFrameKnob = knob("frame");

	if (_pTimingKnob != NULL) {
		_pFrameKnob->visible(_pTimingKnob->get_value()!=0);
	}


	if (f.makeKnobs()) {
		p_tableKnobI = p_tableKnob->tableKnob();
		p_tableKnobI->addStringColumn("name", "Obj Name", false, 216 /*column width*/);
		p_tableKnobI->addColumn("active", "Active", Table_KnobI::BoolColumn, true, 45);
		p_tableKnobI->addColumn("bbox", "BBox", Table_KnobI::BoolColumn, true, 45);

	}

	// Maintain a list of active and bbox booleans, so we don't have to use the tableKnob pointer in create_geometry
	if (p_tableKnob) {
		p_tableKnobI = p_tableKnob->tableKnob();
		int numObjs = p_tableKnobI->getRowCount();
		active_objs.resize(numObjs);
		bbox_objs.resize(numObjs);
		for (int i = 0; i < numObjs; i++) {
			active_objs[i] = p_tableKnobI->getCellBool(i,1);
			bbox_objs[i] = p_tableKnobI->getCellBool(i,2);
		}
	}

}

// *****************************************************************************
// 	KNOB_CHANGED : Callbacks for file and table knobs
// *****************************************************************************

int ABCReadGeo::knob_changed(DD::Image::Knob* k)
{


	if(k->name() == "Reload") {

		knob("file")->changed();
		return 1;
	}

	if(k->name() == "activate_selection") {
		p_tableKnobI->suspendKnobChangedEvents();
		std::vector<int> selectedRows = p_tableKnobI->getSelectedRows();
		for (unsigned i = 0; i < selectedRows.size(); i++) {
			p_tableKnobI->setCellBool(selectedRows[i], 1, true);

		}
		p_tableKnobI->resumeKnobChangedEvents(true);

		return 1;
	}

	if(k->name() == "deactivate_selection") {
		p_tableKnobI->suspendKnobChangedEvents();
		std::vector<int> selectedRows = p_tableKnobI->getSelectedRows();
		for (unsigned i = 0; i < selectedRows.size(); i++) {
			p_tableKnobI->setCellBool(selectedRows[i], 1, false);

		}
		p_tableKnobI->resumeKnobChangedEvents(true);

		return 1;
	}

	if(k->name() == "bbox_selection") {
		p_tableKnobI->suspendKnobChangedEvents();
		std::vector<int> selectedRows = p_tableKnobI->getSelectedRows();
		for (unsigned i = 0; i < selectedRows.size(); i++) {
			p_tableKnobI->setCellBool(selectedRows[i], 2, true);

		}
		p_tableKnobI->resumeKnobChangedEvents(true);

		return 1;
	}

	if(k->name() == "unbbox_selection") {
		p_tableKnobI->suspendKnobChangedEvents();
		std::vector<int> selectedRows = p_tableKnobI->getSelectedRows();
		for (unsigned i = 0; i < selectedRows.size(); i++) {
			p_tableKnobI->setCellBool(selectedRows[i], 2, false);

		}
		p_tableKnobI->resumeKnobChangedEvents(true);

		return 1;
	}

	if(k->name() == "file") {
		updateTableKnob();
		updateTimingKnobs();
		return 1;
	}

	return SourceGeo::knob_changed(k);
}


// ***************************************************************************************
// 	UPDATETABLEKNOB : Fill in the table knob with all geo objects from the ABC archive
// ***************************************************************************************

void ABCReadGeo::updateTableKnob()
{

	p_tableKnobI->deleteAllItems();
	p_tableKnobI->reset();

	if (filename()[0] == '\0') {
		return;
	}

	IArchive archive( Alembic::AbcCoreHDF5::ReadArchive(),
			filename(),
			Abc::ErrorHandler::kQuietNoopPolicy );

	if (!archive.valid()) {
		return;
	}

	IObject archiveTop = archive.getTop();
	std::vector<Alembic::AbcGeom::IObject>  _objs;
	getABCGeos(archiveTop, _objs);

	int obj = 0;
	for( std::vector<Alembic::AbcGeom::IObject>::const_iterator iObj( _objs.begin() ); iObj != _objs.end(); ++iObj ) {
		p_tableKnobI->addRow(obj);
		p_tableKnobI->setCellString(obj,0,iObj->getName());
		p_tableKnobI->setCellBool(obj,1,true);
		obj++;
	}
}

// *****************************************************************************
// 	UPDATETIMINGKNOBS : Fill in the frame range knobs
// *****************************************************************************

void ABCReadGeo::updateTimingKnobs()
{

	if (filename()[0] == '\0') {
		return;
	}

	IArchive archive( Alembic::AbcCoreHDF5::ReadArchive(),
			filename(),
			Abc::ErrorHandler::kQuietNoopPolicy );

	if (!archive.valid()) {
		return;
	}

	chrono_t firstSample = std::numeric_limits<double>::max();
	chrono_t lastSample = std::numeric_limits<double>::min();

	getABCTimeSpan(archive, firstSample, lastSample);

	knob("first")->set_value(int(firstSample *  _FPS + 0.5f));
	knob("last")->set_value(int(lastSample *  _FPS + 0.5f));

}


/*----------------------------------------------------------------------------------------------------*/

// *****************************************************************************
// 	APPEND : Build up the Op's hash
// *****************************************************************************
/* virtual */
void ABCReadGeo::append(Hash& hash)
{

	p_tableKnobI = knob("Obj_list")->tableKnob();
	Op::append(hash);

	hash.append(outputContext().frame());
	hash.append(m_filename);
	hash.append(interpolate);

	if (p_tableKnobI) {
		for (unsigned i = 0; i < active_objs.size(); i++) {
			hash.append(active_objs[i]);
			hash.append(bbox_objs[i]);
		}
	}
}

// *****************************************************************************
// 	GET_GEOMETRY_HASH : Build up the geo hashes
// *****************************************************************************
/*virtual*/
void ABCReadGeo::get_geometry_hash()
{
	SourceGeo::get_geometry_hash();

	// Group Points
	geo_hash[Group_Points].append(m_filename);
	geo_hash[Group_Points].append(m_sampleFrame);
	geo_hash[Group_Points].append(interpolate);

	// Group Primitives
	geo_hash[Group_Primitives].append(m_filename);

	// Group Attributes
	geo_hash[Group_Attributes].append(m_filename);
	geo_hash[Group_Attributes].append(m_sampleFrame);

	// Hash up Table knob selections
	for (unsigned i = 0; i < active_objs.size(); i++) {
		geo_hash[Group_Primitives].append(active_objs[i]);
		geo_hash[Group_Points].append(active_objs[i]);
		geo_hash[Group_Attributes].append(active_objs[i]);
		geo_hash[Group_Primitives].append(bbox_objs[i]);
		geo_hash[Group_Points].append(bbox_objs[i]);
		geo_hash[Group_Attributes].append(bbox_objs[i]);
	}

}

/*---------------------------------------------------------------------------------------------------*/

// *****************************************************************************
// 	CREATE_GEOMETRY : The meat. Query the ABC archive for the needed bits
// *****************************************************************************
/*virtual*/
void ABCReadGeo::create_geometry(Scene& scene, GeometryList& out)
{

	if (filename()[0] == '\0') {
		out.delete_objects();
		return;
	}

	IArchive archive( Alembic::AbcCoreHDF5::ReadArchive(),
			filename(),//archiveName,
			Abc::ErrorHandler::kQuietNoopPolicy );

	if (!archive.valid()) {
		std::cout << "error reading archive" << std::endl;
		error("Unable to read file");
		return;
	}

	IObject archiveTop = archive.getTop();

	std::vector<Alembic::AbcGeom::IObject>  _objs;
	getABCGeos(archiveTop, _objs);

	// current Time to sample from
	chrono_t curTime = m_sampleFrame  / _FPS;

	if ( rebuild(Mask_Primitives)) {
		out.delete_objects();
	}

	int obj = 0;
	for( std::vector<Alembic::AbcGeom::IObject>::const_iterator iObj( _objs.begin() ); iObj != _objs.end(); ++iObj ) {


		// Leave an empty obj if knob is unchecked
		if (!active_objs[obj] ) {
			out.add_object(obj);
			PointList& points = *out.writable_points(obj);
			points.resize(0);
			out[obj].delete_group_attribute(Group_Vertices,kUVAttrName, VECTOR4_ATTRIB);
			obj++;
			continue;
		}


		if ( rebuild(Mask_Primitives)) {

			out.add_object(obj);

			if (bbox_objs[obj]) { //(bbox_mode) {
				buildBboxPrimitives(out, obj);
			}
			else {
				buildABCPrimitives(out, obj, *iObj, curTime);
			}
		}


		if ( rebuild(Mask_Points)) {

			PointList& points = *out.writable_points(obj);

			if (bbox_objs[obj]) { //(bbox_mode) {
				Imath::Box3d bbox = getBounds(*iObj, curTime);

				points.resize(8);

				IObject iObj_copy(*iObj);
				Matrix4 xf = getConcatMatrix(iObj_copy,curTime, interpolate !=0); // for some reason getParent() won't take a const IObject, hence the copy...

				// Add bbox corners
				for (unsigned i = 0; i < 8; i++) {
					Vector3 pt((i&4)>>2 ? bbox.max.x : bbox.min.x, (i&2)>>1 ? bbox.max.y : bbox.min.y, (i%2) ? bbox.max.z : bbox.min.z );
					points[i] = xf.transform(pt);
				}
			}

			else{
				writePoints(*iObj, points, curTime, interpolate !=0);
			}

		}



		if ( rebuild(Mask_Attributes)) {

			if (bbox_objs[obj]) { //(bbox_mode)
				out[obj].delete_group_attribute(Group_Vertices,kUVAttrName, VECTOR4_ATTRIB);
			}
			else {
				// set UVs
				Attribute* UV = out.writable_attribute(obj, Group_Vertices, kUVAttrName, VECTOR4_ATTRIB);
				IV2fGeomParam uvParam = getUVsParam(*iObj);
				setUVs(out[obj], uvParam, UV, curTime);

				// set Normals
				IN3fGeomParam nParam = getNsParam(*iObj);
				if (nParam.valid()) {
					Attribute* N = out.writable_attribute(obj, Group_Vertices, kNormalAttrName, NORMAL_ATTRIB);
					setNormals(out[obj], nParam, N, curTime);
				}
			}
		}

		obj++;
	}

	out.synchronize_objects();

}


static Op* Build(Node* node) { return new ABCReadGeo(node); }
const Op::Description ABCReadGeo::description(nodeClass, Build);


