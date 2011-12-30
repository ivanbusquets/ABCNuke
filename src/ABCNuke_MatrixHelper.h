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

#ifndef _ABCNuke_MatrixHelper_h_
#define _ABCNuke_MatrixHelper_h_

#include "DDImage/Vector3.h"
#include "DDImage/Matrix4.h"
#include "DDImage/Quaternion.h"

#include <Alembic/Abc/All.h>
#include <Alembic/AbcGeom/All.h>

#include "ABCNuke_Interpolation.h"

using namespace DD::Image;
using namespace Alembic::AbcGeom;

typedef std::set<Abc::chrono_t> SampleTimeSet;
typedef std::map<Abc::chrono_t, M44d> MatrixSampleMap;
void decomposeMatrix(const Matrix4& mat, Vector3& scale, Vector3& translation, Quaternion& rotation );
Matrix4 recomposeMatrix(const Vector3 &scale, const Vector3 &translation, const Quaternion &rotation);

//-*****************************************************************************
// The following functions are taken from OpenEXR - Imath.
void DecomposeXForm(const Imath::M44d &mat, Imath::V3d &scale, Imath::V3d &shear, Imath::Quatd &rotation, Imath::V3d &translation);
M44d RecomposeXForm(const Imath::V3d &scale, const Imath::V3d &shear, const Imath::Quatd &rotation, const Imath::V3d &translation);
//-*****************************************************************************

Imath::V3d lerp(const Imath::V3d &a, const Imath::V3d &b, double amt);
void accumXform( Imath::M44d &xf, IObject obj, chrono_t curTime, bool interpolate );
Matrix4 convert( const Imath::M44d &from );
Imath::M44d convert( const Matrix4 &from );
const Matrix4 getConcatMatrix( IObject iObj, chrono_t curTime, bool interpolate );

#endif
