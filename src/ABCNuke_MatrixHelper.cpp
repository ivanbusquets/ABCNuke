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

//-*****************************************************************************
#include "ABCNuke_MatrixHelper.h"
#include "DDImage/Vector3.h"
#include "DDImage/Matrix4.h"
#include "DDImage/Quaternion.h"
//-*****************************************************************************

using namespace DD::Image;
using namespace Alembic::AbcGeom;


void decomposeMatrix(const Matrix4& mat, Vector3& scale, Vector3& translation, Quaternion& rotation ) {

	Matrix4 t_mat(mat);
	t_mat.translationOnly();
	translation = t_mat.translation();

	Matrix4 r_mat(mat);
	r_mat.rotationOnly();
	const Matrix4 r_mat2(r_mat);
	Quaternion rotation2(r_mat2);
	rotation = rotation2;

	Matrix4 s_mat(mat);
	s_mat.scaleOnly();
	scale = s_mat.scale();

}

Matrix4 recomposeMatrix(
		const Vector3 &scale,
		const Vector3 &translation,
		const Quaternion &rotation
)
{
	Matrix4 s_mtx, r_mtx, t_mtx;

	s_mtx.scaling(scale);
	r_mtx = rotation.matrix();
	t_mtx.translation(translation);

	return s_mtx * r_mtx * t_mtx;
}

//-*****************************************************************************
// The following functions are taken from OpenEXR - Imath.
// DecomposeXForm(), RecomposeXForm()
// To do: replace them with own functions using Nuke's native
// Matrix4, Vector3 and Quaternion classes
//-*****************************************************************************
void DecomposeXForm(
		const Imath::M44d &mat,
		Imath::V3d &scale,
		Imath::V3d &shear,
		Imath::Quatd &rotation,
		Imath::V3d &translation
)
{
	Imath::M44d mat_remainder(mat);

	// Extract Scale, Shear
	Imath::extractAndRemoveScalingAndShear(mat_remainder, scale, shear);

	// Extract translation
	translation.x = mat_remainder[3][0];
	translation.y = mat_remainder[3][1];
	translation.z = mat_remainder[3][2];

	// Extract rotation
	rotation = extractQuat(mat_remainder);
}

// from OpenEXR Imath
M44d RecomposeXForm(
		const Imath::V3d &scale,
		const Imath::V3d &shear,
		const Imath::Quatd &rotation,
		const Imath::V3d &translation
)
{
	Imath::M44d scale_mtx, shear_mtx, rotation_mtx, translation_mtx;

	scale_mtx.setScale(scale);
	shear_mtx.setShear(shear);
	rotation_mtx = rotation.toMatrix44();
	translation_mtx.setTranslation(translation);

	return scale_mtx * shear_mtx * rotation_mtx * translation_mtx;
}


// when amt is 0, a is returned
inline double lerp(double a, double b, double amt)
{
	return (a + (b-a)*amt);
}


Imath::V3d lerp(const Imath::V3d &a, const Imath::V3d &b, double amt)
{
	return Imath::V3d(lerp(a[0], b[0], amt),
			lerp(a[1], b[1], amt),
			lerp(a[2], b[2], amt));
}


void accumXform( Imath::M44d &xf, IObject obj, chrono_t curTime = 0, bool interpolate = false )
{
	if ( IXform::matches( obj.getHeader() ) )
	{
		Imath::M44d mtx;
		IXform x( obj, kWrapExisting );
		XformSample xs;
		x.getSchema().get( xs );

		if (!x.getSchema().isConstant()) {

			TimeSamplingPtr timeSampler = x.getSchema().getTimeSampling();

			if (interpolate) {

				//std::pair<index_t, chrono_t> lSamp;// = timeSampler->getFloorIndex(curTime, x.getSchema().getNumSamples());
				Alembic::AbcCoreAbstract::index_t floorIdx, ceilIdx;

				double amt = getWeightAndIndex(curTime, timeSampler,
						x.getSchema().getNumSamples(), floorIdx, ceilIdx);

				if (amt != 0 && floorIdx != ceilIdx) {

					const ISampleSelector iss_start(floorIdx);//lSamp.first);
					Imath::M44d mtx_start = x.getSchema().getValue(iss_start).getMatrix();

					//std::pair<index_t, chrono_t> rSamp = timeSampler->getCeilIndex(curTime, x.getSchema().getNumSamples());
					const ISampleSelector iss_end(ceilIdx);//rSamp.first);
					Imath::M44d mtx_end = x.getSchema().getValue(iss_end).getMatrix();


					Imath::V3d s_l,s_r,h_l,h_r,t_l,t_r;
					Imath::Quatd quat_l,quat_r;



					DecomposeXForm(mtx_start, s_l, h_l, quat_l, t_l);
					DecomposeXForm(mtx_end, s_r, h_r, quat_r, t_r);


					if ((quat_l ^ quat_r) < 0)
					{
						quat_r = -quat_r;
					}

					mtx =  RecomposeXForm(Imath::lerp(s_l, s_r, amt),
							Imath::lerp(h_l, h_r, amt),
							Imath::slerp(quat_l, quat_r, amt),
							Imath::lerp(t_l, t_r, amt));


				}

				else {
					const ISampleSelector iss(curTime);
					xs = x.getSchema().getValue(iss);
					mtx = xs.getMatrix();
				}

			}



			else { // no interpolation, get nearest sample
				const ISampleSelector iss(curTime);
				xs = x.getSchema().getValue(iss);
				mtx = xs.getMatrix();
			}

		}
		else {
			mtx = xs.getMatrix();
		}
		xf *= mtx;
	}
}

Matrix4 convert( const Imath::M44d &from )
{
	Matrix4 result;
	for( unsigned int i=0; i<4; i++ )
	{
		for( unsigned int j=0; j<4; j++ )
		{
			result[j][i] = from[j][i];
		}
	}
	return result;
}

Imath::M44d convert( const Matrix4 &from )
{
	Imath::M44d result;
	for( unsigned int i=0; i<4; i++ )
	{
		for( unsigned int j=0; j<4; j++ )
		{
			result[j][i] = from[j][i];
		}
	}
	return result;
}


const Matrix4 getConcatMatrix( IObject iObj, chrono_t curTime = 0, bool interpolate = false )
{
	Imath::M44d xf;
	xf.makeIdentity();
	IObject parent = iObj.getParent();

	// Once the Archive's Top Object is reached, IObject::getParent() will
	// return an invalid IObject, and that will evaluate to False.
	while ( parent )
	{
		accumXform( xf, parent, curTime, interpolate );
		parent = parent.getParent();
	}

	Matrix4 ret_matrix = convert(xf);

	return ret_matrix;
}
