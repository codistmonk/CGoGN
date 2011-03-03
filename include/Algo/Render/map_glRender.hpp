/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009, IGG Team, LSIIT, University of Strasbourg                *
*                                                                              *
* This library is free software; you can redistribute it and/or modify it      *
* under the terms of the GNU Lesser General Public License as published by the *
* Free Software Foundation; either version 2.1 of the License, or (at your     *
* option) any later version.                                                   *
*                                                                              *
* This library is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License  *
* for more details.                                                            *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with this library; if not, write to the Free Software Foundation,      *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.           *
*                                                                              *
* Web site: https://iggservis.u-strasbg.fr/CGoGN/                              *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#include <vector>
#include <limits>

#include <list>

#include "Algo/Render/renderFunctor.h"

namespace CGoGN
{

namespace Algo
{

namespace Render
{

namespace Direct
{

template <typename PFP>
void renderTriQuadPoly(typename PFP::MAP& the_map, RenderType rt, float explode, const typename PFP::TVEC3& position, const typename PFP::TVEC3& normal, const FunctorSelect& good)
{
	// RenderType => lighted & smooth ??
	bool lighted = (rt != NO_LIGHT) && (rt!= LINE);
	bool smooth = (rt==SMOOTH);

	// first pass render the triangles
	FunctorGLFace<PFP> fgl_tri(the_map, lighted, smooth, 3, explode, true, position, normal, good);


	glBegin(GL_TRIANGLES);
	the_map.foreach_orbit(FACE_ORBIT, fgl_tri, good);
	glEnd();

	// get untreated quads & polygons
	std::vector<Dart>& polygons = fgl_tri.getPolyDarts();
	//std::cout<< "Reste a rendre: "<< polygons.size()<<std::endl;

	if (rt==LINE)
	{
		FunctorGLFace<PFP> fgl_quads(the_map, lighted, smooth, POLYGONS, explode, true, position, normal, good);
		for(typename std::vector<Dart>::iterator d = polygons.begin(); d != polygons.end(); ++d)
		{
			glBegin(GL_POLYGON);
			fgl_quads(*d);
			glEnd();
		}
		polygons = fgl_quads.getPolyDarts();
	}
	else
	{
		FunctorGLFace<PFP> fgl_quads(the_map, lighted, smooth, 4, explode, true, position, normal, good);
		glBegin(GL_TRIANGLES);
		for(typename std::vector<Dart>::iterator d = polygons.begin(); d != polygons.end(); ++d)
		{
			fgl_quads(*d);
		}
		glEnd();
		polygons = fgl_quads.getPolyDarts();
	}

	if (rt==LINE)
	{
		FunctorGLFace<PFP> fgl_polygStar(the_map, lighted, smooth, POLYGONS, explode, true, position, normal, good);
		for(typename std::vector<Dart>::iterator d = polygons.begin(); d != polygons.end(); ++d)
		{
			glBegin(GL_POLYGON);
			fgl_polygStar(*d);
			glEnd();
		}
	}
	else
	{
		FunctorGLFace<PFP> fgl_polygStar(the_map, lighted, smooth, TRIFAN, explode, true, position, normal, good);
		for(typename std::vector<Dart>::iterator d = polygons.begin(); d != polygons.end(); ++d)
		{
			glBegin(GL_TRIANGLE_FAN);
			fgl_polygStar(*d);
			glEnd();
		}
	}
}



template <typename PFP>
void renderTriQuadPoly(typename PFP::MAP& the_map, RenderType rt, float explode, const typename PFP::TVEC3& position, const typename PFP::TVEC3& normal, const typename PFP::TVEC3& color, const FunctorSelect& good)
{
	// RenderType => lighted & smooth ??
	bool lighted = (rt != NO_LIGHT) && (rt!= LINE);
	bool smooth = (rt==SMOOTH);

	// first pass render the triangles
	FunctorGLFaceColor<PFP> fgl_tri(the_map, lighted, smooth, 3, explode, true, position, normal, color, good);


	glBegin(GL_TRIANGLES);
	the_map.foreach_orbit(FACE_ORBIT, fgl_tri, good);
	glEnd();

	// get untreated quads & polygons
	std::vector<Dart>& polygons = fgl_tri.getPolyDarts();
	//std::cout<< "Reste a rendre: "<< polygons.size()<<std::endl;

	if (rt==LINE)
	{
		FunctorGLFaceColor<PFP> fgl_quads(the_map, lighted, smooth, POLYGONS, explode, true, position, normal, color, good);
		for(typename std::vector<Dart>::iterator d = polygons.begin(); d != polygons.end(); ++d)
		{
			glBegin(GL_POLYGON);
			fgl_quads(*d);
			glEnd();
		}
		polygons = fgl_quads.getPolyDarts();
	}
	else
	{
		FunctorGLFaceColor<PFP> fgl_quads(the_map, lighted, smooth, 4, explode, true, position, normal, color, good);
		glBegin(GL_TRIANGLES);
		for(typename std::vector<Dart>::iterator d = polygons.begin(); d != polygons.end(); ++d)
		{
			fgl_quads(*d);
		}
		glEnd();
		polygons = fgl_quads.getPolyDarts();
	}

	if (rt==LINE)
	{
		FunctorGLFaceColor<PFP> fgl_polygStar(the_map, lighted, smooth, POLYGONS, explode, true, position, normal, color, good);
		for(typename std::vector<Dart>::iterator d = polygons.begin(); d != polygons.end(); ++d)
		{
			glBegin(GL_POLYGON);
			fgl_polygStar(*d);
			glEnd();
		}
	}
	else
	{
		FunctorGLFaceColor<PFP> fgl_polygStar(the_map, lighted, smooth, TRIFAN, explode, true, position, normal, color, good);
		for(typename std::vector<Dart>::iterator d = polygons.begin(); d != polygons.end(); ++d)
		{
			glBegin(GL_TRIANGLE_FAN);
			fgl_polygStar(*d);
			glEnd();
		}
	}
}


template <typename PFP>
void renderNormalVertices(typename PFP::MAP& the_map, const typename PFP::TVEC3& position, const typename PFP::TVEC3& normal, float scale, const FunctorSelect& good)
{
	FunctorGLNormal<PFP> fgl_norm(the_map, good, position, normal, scale);

	glBegin(GL_LINES);
	the_map.foreach_orbit(VERTEX_ORBIT, fgl_norm, good);
	glEnd();
}

template <typename PFP>
void renderFrameVertices(typename PFP::MAP& the_map, const typename PFP::TVEC3& position, const typename PFP::TVEC3 frame[3], float scale, const FunctorSelect& good)
{
	FunctorGLFrame<PFP> fgl_frame(the_map, good, position, frame, scale) ;

	glBegin(GL_LINES) ;
	the_map.foreach_orbit(VERTEX_ORBIT, fgl_frame, good) ;
	glEnd();
}



} // namespace Direct

} // namespace Render

} // namespace Algo

} // namespace CGoGN
