/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009-2012, IGG Team, LSIIT, University of Strasbourg           *
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
* Web site: http://cgogn.unistra.fr/                                           *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#ifndef __SUBDIVISION_H__
#define __SUBDIVISION_H__

#include <math.h>
#include <vector>

namespace CGoGN
{

namespace Algo
{

namespace Modelisation
{

/**
* Triangule a face with central vertex
* @param d dart of face
* @return a dart of the central vertex
*/
template <typename PFP>
Dart trianguleFace(typename PFP::MAP& map, Dart d);

/**
 * Triangule all the faces of the mesh
 */
template <typename PFP, typename EMBV, typename EMB>
void trianguleFaces(typename PFP::MAP& map, EMBV& attributs, const FunctorSelect& selected = allDarts) ;

template <typename PFP>
void trianguleFaces(typename PFP::MAP& map, VertexAttribute<typename PFP::VEC3>& position, const FunctorSelect& selected = allDarts) ;

/**
 * Triangule all the faces of the mesh
 * positions for new vertices are given as face attribute
 */
template <typename PFP>
void trianguleFaces(
	typename PFP::MAP& map,
	VertexAttribute<typename PFP::VEC3>& position, const FaceAttribute<typename PFP::VEC3>& positionF,
	const FunctorSelect& selected = allDarts) ;

/**
* Quadrangule a face with central vertex
* warning: edges are considered here as already cut !!
* A face with 2n edges give n quads
* @param d dart of face with belong to a vertex introduce when cutting the edge
* @return a dart of central point
*/
template <typename PFP>
Dart quadranguleFace(typename PFP::MAP& map, Dart d);

/**
 * Quadrangule all the faces of the mesh
 */
template <typename PFP, typename EMBV, typename EMB>
void quadranguleFaces(typename PFP::MAP& map, EMBV& attributs, const FunctorSelect& selected = allDarts) ;

template <typename PFP>
void quadranguleFaces(typename PFP::MAP& map, VertexAttribute<typename PFP::VEC3>& position, const FunctorSelect& selected = allDarts) ;

/**
 * Catmull-Clark subdivision scheme
 */
template <typename PFP, typename EMBV, typename EMB>
void CatmullClarkSubdivision(typename PFP::MAP& map, EMBV& attributs, const FunctorSelect& selected = allDarts) ;

template <typename PFP>
void CatmullClarkSubdivision(typename PFP::MAP& map, VertexAttribute<typename PFP::VEC3>& position, const FunctorSelect& selected = allDarts) ;

/**
 * Loop subdivision scheme
 */
template <typename PFP, typename EMBV, typename EMB>
void LoopSubdivision(typename PFP::MAP& map, EMBV& attributs, const FunctorSelect& selected = allDarts) ;

template <typename PFP>
void LoopSubdivision(typename PFP::MAP& map, VertexAttribute<typename PFP::VEC3>& position, const FunctorSelect& selected = allDarts) ;


template <typename PFP, typename EMBV, typename EMB>
void TwoNPlusOneSubdivision(typename PFP::MAP& map, EMBV& attributs, const FunctorSelect& selected = allDarts) ;

/**
 * Reverse the orientation of the map
 */
template <typename PFP>
void reverseOrientation(typename PFP::MAP& map) ;

///**
// * Dual mesh computation
// */
//template <typename PFP>
//void computeDual(typename PFP::MAP& map, const FunctorSelect& selected = allDarts) ;

///**
// * Sqrt(3) subdivision scheme
// */
//template <typename PFP>
//void Sqrt3Subdivision(typename PFP::MAP& map, typename PFP::TVEC3& position, const FunctorSelect& selected = allDarts) ;


} // namespace Modelisation

} // namespace Algo

} // namespace CGoGN

#include "Algo/Modelisation/subdivision.hpp"

#endif
