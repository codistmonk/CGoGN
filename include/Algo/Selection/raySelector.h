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

#ifndef RAYSELECTOR_H_
#define RAYSELECTOR_H_


#include <vector>
#include "Algo/Selection/raySelectFunctor.hpp"

namespace CGoGN
{

namespace Algo
{

namespace Selection
{

/**
 * Function that does the selection of faces, returned darts are sorted from closest to farthest
 * @param map the map we want to test
 * @param good a dart selector
 * @param rayA first point of ray (user side)
 * @param rayAB direction of ray (directed to the scene)
 * @param vecFaces (out) vector to store the darts of intersected faces
 */
template<typename PFP>
void facesRaySelection(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position,
		const typename PFP::VEC3& rayA, const typename PFP::VEC3& rayAB, std::vector<Dart>& vecFaces,
		const FunctorSelect& good=allDarts);

/**
 * Function that does the selection of edges, returned darts are sorted from closest to farthest
 * @param map the map we want to test
 * @param rayA first point of  ray (user side)
 * @param rayAB vector of ray (directed ot the scene)
 * @param vecEdges (out) vector to store dart of intersected edges
 * @param dist radius of the cylinder of selection
 */
template<typename PFP>
void edgesRaySelection(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position,
				const typename PFP::VEC3& rayA, const typename PFP::VEC3& rayAB, std::vector<Dart>& vecEdges, float distMax,
				const FunctorSelect& good=allDarts);

/**
 * Function that does the selection of vertices, returned darts are sorted from closest to farthest
 * @param map the map we want to test
 * @param rayA first point of  ray (user side)
 * @param rayAB vector of ray (directed ot the scene)
 * @param vecVertices (out) vector to store dart of intersected vertices
 * @param dist radius of the cylinder of selection
 */
template<typename PFP>
void verticesRaySelection(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position,
		const typename PFP::VEC3& rayA, const typename PFP::VEC3& rayAB, std::vector<Dart>& vecVertices,
		float dist, const FunctorSelect& good=allDarts);


/**
 * Volume selection, not yet functional
 */
template<typename PFP>
void volumesRaySelection(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position,
		const typename PFP::VEC3& rayA, const typename PFP::VEC3& rayAB, std::vector<Dart>& vecVolumes,
		const FunctorSelect& good = allDarts);




/**
 * Function that does the selection of one vertex
 * @param map the map we want to test
 * @param rayA first point of  ray (user side)
 * @param rayAB vector of ray (directed ot the scene)
 * @param vertex (out) dart of selected vertex (set to NIL if no vertex selected)
 */
template<typename PFP>
void vertexRaySelection(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position,
		const typename PFP::VEC3& rayA, const typename PFP::VEC3& rayAB, Dart& vertex,
		const FunctorSelect& good = allDarts);


template<typename PFP>
void facesPlanSelection(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position,
		const typename Geom::Plane3D<typename PFP::VEC3::DATA_TYPE>& plan, std::vector<Dart>& vecDarts,
		const FunctorSelect& good = allDarts);


/**
 * Function that does the selection of vertices in a cone, returned darts are sorted from closest to farthest
 * @param map the map we want to test
 * @param position the position attribute
 * @param rayA first point of  ray (user side)
 * @param rayAB vector of ray (directed ot the scene)
 * @param angle angle of the code in degree.
 * @param vecVertices (out) vector to store dart of intersected vertices
 * @param good the selector
 */
template<typename PFP>
void verticesConeSelection(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position,
		const typename PFP::VEC3& rayA, const typename PFP::VEC3& rayAB, float angle, std::vector<Dart>& vecVertices,
		const FunctorSelect& good= allDarts);

/**
 * Function that does the selection of edges, returned darts are sorted from closest to farthest
 * @param map the map we want to test
 * @param position the position attribute
 * @param rayA first point of  ray (user side)
 * @param rayAB vector of ray (directed ot the scene)
 * @param angle radius of the cylinder of selection
 * @param vecEdges (out) vector to store dart of intersected edges
 * @param good the selector
 */
template<typename PFP>
void edgesConeSelection(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position,
				const typename PFP::VEC3& rayA, const typename PFP::VEC3& rayAB, float angle, std::vector<Dart>& vecEdges,
				const FunctorSelect& good=allDarts);


/**
 * Function that select the closest vertex in the bubble
 * @param map the map we want to test
 * @param position the position attribute
 * @param cursor the cursor position (center of bubble)
 * @param radiusMax max radius of selection
 * @param good the selector
 */
template<typename PFP>
Dart verticesBubbleSelection(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position,
		const typename PFP::VEC3& cursor, typename PFP::REAL radiusMax,
		const FunctorSelect& good=allDarts);


/**
 * Function that select the closest edge in the bubble
 * @param map the map we want to test
 * @param position the position attribute
 * @param cursor the cursor position (center of bubble)
 * @param radiusMax max radius of selection
 * @param good the selector
 */
template<typename PFP>
Dart edgesBubbleSelection(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position,
		const typename PFP::VEC3& cursor, typename PFP::REAL radiusMax,
		const FunctorSelect& good=allDarts);





/**
 * Fonction that do the selection of darts, returned darts are sorted from closest to farthest
 * Dart is here considered as a triangle formed by the 2 end vertices of the edge and the face centroid
 * @param map the map we want to test
 * @param rayA first point of  ray (user side)
 * @param rayAB vector of ray (directed ot the scene)
 * @param vecDarts (out) vector to store dart of intersected darts
 */
//template<typename PFP>
//void dartsRaySelection(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position, const typename PFP::VEC3& rayA, const typename PFP::VEC3& rayAB, std::vector<Dart>& vecDarts, const FunctorSelect& good = allDarts);

//namespace Parallel
//{
//template<typename PFP>
//void facesRaySelection(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position, const FunctorSelect& good, const typename PFP::VEC3& rayA, const typename PFP::VEC3& rayAB, std::vector<Dart>& vecFaces, unsigned int nbth=0, unsigned int current_thread=0);
//template<typename PFP>
//void edgesRaySelection(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position, const FunctorSelect& good, const typename PFP::VEC3& rayA, const typename PFP::VEC3& rayAB, std::vector<Dart>& vecEdges, float dist, unsigned int nbth=0, unsigned int current_thread=0);
//template<typename PFP>
//void vertexRaySelection(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position, const typename PFP::VEC3& rayA, const typename PFP::VEC3& rayAB, Dart& vertex, const FunctorSelect& good = allDarts, unsigned int nbth=0, unsigned int current_thread=0);
//}

} //namespace Selection

} //namespace Algo

} //namespace CGoGN

#include "Algo/Selection/raySelector.hpp"

#endif /* RAYSELECTOR_H_ */
