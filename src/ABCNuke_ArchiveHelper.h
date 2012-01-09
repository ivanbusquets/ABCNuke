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

#ifndef _ABCNuke_ArchiveHelper_h_
#define _ABCNuke_ArchiveHelper_h_

//-*****************************************************************************
#include <Alembic/Abc/All.h>
#include <Alembic/AbcGeom/All.h>
//-*****************************************************************************

using namespace Alembic::AbcGeom;

// Get a list of geometry objects - IPolyMeshes and ISubDs
void getABCGeos(Alembic::Abc::IObject & iObj,
				std::vector<Alembic::AbcGeom::IObject> & _objs);

// Get a list of IXforms
void getABCXforms(Alembic::Abc::IObject & iObj,
				std::vector<Alembic::AbcGeom::IXform> & _objs);

// Get a list of ICameras
void getABCCameras(Alembic::Abc::IObject & iObj,
				std::vector<Alembic::AbcGeom::ICamera> & _objs);

// Find object by name
bool getNamedXform( IObject iObjTop, const std::string &iName, IXform &iXf );
bool getNamedCamera( IObject iObjTop, const std::string &iName, ICamera &iCam );

// Get the time span of a single IObject, or the whole ABC archive
void getXformTimeSpan(IXform iXf, chrono_t& first, chrono_t& last, bool inherits = false);
void getCameraTimeSpan(ICamera iCam, chrono_t& first, chrono_t& last);
void getPolyMeshTimeSpan(IPolyMesh iPoly, chrono_t& first, chrono_t& last);
void getSubDTimeSpan(ISubD iSub, chrono_t& first, chrono_t& last);
void getObjectTimeSpan(IObject obj, chrono_t& first, chrono_t& last, bool doChildren = false);
void getABCTimeSpan(Alembic::Abc::IArchive archive, chrono_t& first, chrono_t& last);

#endif
