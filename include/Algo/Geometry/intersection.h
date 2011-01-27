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

#ifndef __ALGO_GEOMETRY_INTERSECTION_H__
#define __ALGO_GEOMETRY_INTERSECTION_H__

#include "Geometry/basic.h"
#include "Geometry/intersection.h"

namespace CGoGN
{

namespace Algo
{

namespace Geometry
{

/**
 * test the intersection between a line and a n-sided face (n>=3)
 * (works with triangular faces but not optimized)
 * TODO to test
 * @param map the map
 * @param d a dart defining the n-sided face
 * @param PA segment point 1
 * @param Dir a direction for the line
 * @param Inter store the intersection point
 * @return true if segment intersects the face
 */
template <typename PFP>
bool intersectionLineConvexFace(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& positions, const typename PFP::VEC3& PA, const typename PFP::VEC3& Dir, typename PFP::VEC3& Inter) ;

/**
 * test the intersection between a segment and a n-sided face (n>=3)
 * (works with triangular faces but not optimized)
 * TODO to optimize -based on intersectionLineConvexFace- check whether the points are on both sides of the face before computing intersection
 * @param map the map
 * @param d a dart defining the n-sided face
 * @param PA segment point 1
 * @param PB segment point 2
 * @param Inter store the intersection point
 * @return true if segment intersects the face
 */
template <typename PFP>
bool intersectionSegmentConvexFace(typename PFP::MAP& map, Dart d, const typename PFP::TVEC3& positions, const typename PFP::VEC3& PA, const typename PFP::VEC3& PB, typename PFP::VEC3& Inter) ;

/**
 * test if two triangles intersect
 * (based on the "Faster Triangle-Triangle intersection Tests" by Oliver Deviller and Philippe Guigue)
 * @param the map
 * @param a dart of the first triangle
 * @param a dart of the second triangle
 */
template <typename PFP>
bool areTrianglesInIntersection(typename PFP::MAP& map, Dart tri1, Dart tri2, const typename PFP::TVEC3& positions) ;

}

}

}

#include "Algo/Geometry/intersection.hpp"

#endif
