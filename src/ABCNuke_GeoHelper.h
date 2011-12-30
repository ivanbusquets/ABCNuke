/*
Copyright (c) 2011, Ivan Busquets
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
 * Neither the name of Ivan Busquets nor the names of its contributors may
   be used to endorse or promote products derived from this software without
   specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _ABCNuke_GeoHelper_h_
#define _ABCNuke_GeoHelper_h_

#include "DDImage/Attribute.h"
#include "DDImage/GeoInfo.h"
#include "DDImage/Primitive.h"
#include "DDImage/Polygon.h"
#include "DDImage/Point.h"
#include "DDImage/Vector3.h"
#include "DDImage/Matrix4.h"
#include "DDImage/DDMath.h"

#include <Alembic/AbcGeom/All.h>
#include "ABCNuke_Interpolation.h"
#include "ABCNuke_MatrixHelper.h"

#include <Alembic/AbcCoreHDF5/All.h>

#include <ImathBoxAlgo.h>


// standard libs
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>

using namespace DD::Image;
using namespace Alembic::AbcGeom;

void writePoints(Alembic::AbcGeom::IPolyMesh iPoly, PointList& points, chrono_t curTime, bool interpolate);

void writePoints(Alembic::AbcGeom::ISubD iSub, PointList& points, chrono_t curTime, bool interpolate);

void writePoints(const Alembic::AbcGeom::IObject iObj, PointList& points, chrono_t curTime, bool interpolate);

void fillPrimitiveIndices(const Alembic::AbcGeom::IObject iObj, Int32ArraySamplePtr& _fc, Int32ArraySamplePtr& _fi, chrono_t curTime);

Alembic::AbcGeom::IV2fGeomParam getUVsParam(const Alembic::AbcGeom::IObject iObj);

Alembic::AbcGeom::IN3fGeomParam getNsParam(const Alembic::AbcGeom::IObject iObj);

void setUVs(GeoInfo& obj, Alembic::AbcGeom::IV2fGeomParam & iUVs, Attribute* UV, chrono_t curTime);

void setNormals(GeoInfo& obj, Alembic::AbcGeom::IN3fGeomParam & Ns, Attribute* N, chrono_t curTime);

Box3d getBounds( IObject iObj, chrono_t curTime );

void buildBboxPrimitives(GeometryList& out, unsigned obj);

void buildABCPrimitives(GeometryList& out, unsigned obj, const Alembic::AbcGeom::IObject iObj, chrono_t curTime);




#endif
