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

#include <algorithm>
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
void facesRaySelection(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position, const FunctorSelect& good, const typename PFP::VEC3& rayA, const typename PFP::VEC3& rayAB, std::vector<Dart>& vecFaces)
{
	std::vector<typename PFP::VEC3> iPoints;

	// get back intersected faces
	vecFaces.clear();
	Algo::Selection::FuncFaceInter<PFP> ffi(map, position, vecFaces, iPoints, rayA, rayAB);
	map.template foreach_orbit<FACE>(ffi, good);

	// compute all distances to observer for each intersected face
	// and put them in a vector for sorting
	typedef std::pair<typename PFP::REAL, Dart> DartDist;
	std::vector<DartDist> distndart;

	unsigned int nbi = vecFaces.size();
	distndart.resize(nbi);
	for (unsigned int i = 0; i < nbi; ++i)
	{
		distndart[i].second = vecFaces[i];
		typename PFP::VEC3 V = iPoints[i] - rayA;
		distndart[i].first = V.norm2();
	}

	// sort the vector of pair dist/dart
	std::sort(distndart.begin(), distndart.end(), distndartOrdering<PFP>);

	// store sorted darts in returned vector
	for (unsigned int i = 0; i < nbi; ++i)
		vecFaces[i] = distndart[i].second;
}

/**
 * Function that does the selection of edges, returned darts are sorted from closest to farthest
 * @param map the map we want to test
 * @param rayA first point of  ray (user side)
 * @param rayAB vector of ray (directed ot the scene)
 * @param vecEdges (out) vector to store dart of intersected edges
 * @param dist radius of the cylinder of selection
 */
template<typename PFP>
void edgesRaySelection(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position, const FunctorSelect& good, const typename PFP::VEC3& rayA, const typename PFP::VEC3& rayAB, std::vector<Dart>& vecEdges, float dist)
{
	typename PFP::REAL dist2 = dist * dist;
	typename PFP::REAL AB2 = rayAB * rayAB;

	// recuperation des aretes intersectees
	vecEdges.clear();
	Algo::Selection::FuncEdgeInter<PFP> ffi(map, position, vecEdges, rayA, rayAB, AB2, dist2);
	map.template foreach_orbit<EDGE>(ffi, good);

	typedef std::pair<typename PFP::REAL, Dart> DartDist;
	std::vector<DartDist> distndart;

	unsigned int nbi = vecEdges.size();
	distndart.resize(nbi);

	// compute all distances to observer for each middle of intersected edge
	// and put them in a vector for sorting
	for (unsigned int i = 0; i < nbi; ++i)
	{
		Dart d = vecEdges[i];
		distndart[i].second = d;
		typename PFP::VEC3 V = (position[d] + position[map.phi1(d)]) / typename PFP::REAL(2);
		V -= rayA;
		distndart[i].first = V.norm2();
	}

	// sort the vector of pair dist/dart
	std::sort(distndart.begin(), distndart.end(), distndartOrdering<PFP>);

	// store sorted darts in returned vector
	for (unsigned int i = 0; i < nbi; ++i)
		vecEdges[i] = distndart[i].second;
}

/**
 * Function that does the selection of vertices, returned darts are sorted from closest to farthest
 * @param map the map we want to test
 * @param rayA first point of  ray (user side)
 * @param rayAB vector of ray (directed ot the scene)
 * @param vecVertices (out) vector to store dart of intersected vertices
 * @param dist radius of the cylinder of selection
 */
template<typename PFP>
void verticesRaySelection(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position, const typename PFP::VEC3& rayA, const typename PFP::VEC3& rayAB, std::vector<Dart>& vecVertices, float dist, const FunctorSelect& good= allDarts)
{
	typename PFP::REAL dist2 = dist * dist;
	typename PFP::REAL AB2 = rayAB * rayAB;

	// recuperation des sommets intersectes
	vecVertices.clear();
	Algo::Selection::FuncVertexInter<PFP> ffi(map, position, vecVertices, rayA, rayAB, AB2, dist2);
	map.template foreach_orbit<VERTEX>(ffi, good);

	typedef std::pair<typename PFP::REAL, Dart> DartDist;
	std::vector<DartDist> distndart;

	unsigned int nbi = vecVertices.size();
	distndart.resize(nbi);

	// compute all distances to observer for each intersected vertex
	// and put them in a vector for sorting
	for (unsigned int i = 0; i < nbi; ++i)
	{
		Dart d = vecVertices[i];
		distndart[i].second = d;
		typename PFP::VEC3 V = position[d] - rayA;
		distndart[i].first = V.norm2();
	}

	// sort the vector of pair dist/dart
	std::sort(distndart.begin(), distndart.end(), distndartOrdering<PFP>);

	// store sorted darts in returned vector
	for (unsigned int i = 0; i < nbi; ++i)
		vecVertices[i] = distndart[i].second;
}

/**
 * Function that does the selection of one vertex
 * @param map the map we want to test
 * @param rayA first point of  ray (user side)
 * @param rayAB vector of ray (directed ot the scene)
 * @param vertex (out) dart of selected vertex (set to NIL if no vertex selected)
 */
template<typename PFP>
void vertexRaySelection(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position, const typename PFP::VEC3& rayA, const typename PFP::VEC3& rayAB, Dart& vertex, const FunctorSelect& good = allDarts)
{
	std::vector<Dart> vecFaces;
	std::vector<typename PFP::VEC3> iPoints;

	// recuperation des faces intersectes
	Algo::Selection::FuncFaceInter<PFP> ffi(map, position, vecFaces, iPoints, rayA, rayAB);
	map.template foreach_orbit<FACE>(ffi, good);

	if(vecFaces.size() > 0)
	{
		typedef std::pair<typename PFP::REAL, unsigned int> IndexedDist;
		std::vector<IndexedDist> distnint;

		unsigned int nbi = vecFaces.size();
		distnint.resize(nbi);
		for (unsigned int i = 0; i < nbi; ++i)
		{
			distnint[i].first = (iPoints[i] - rayA).norm2();
			distnint[i].second = i;
		}

		// sort the vector of pair dist/dart
		std::sort(distnint.begin(), distnint.end(), distnintOrdering<PFP>);

		// recuperation du point d'intersection sur la face la plus proche
		unsigned int first = distnint[0].second;
		typename PFP::VEC3 ip = iPoints[first];

		// recuperation du sommet le plus proche du point d'intersection
		Dart d = vecFaces[first];
		Dart it = d;
		typename PFP::REAL minDist = (ip - position[it]).norm2();
		vertex = it;
		it = map.phi1(it);
		while(it != d)
		{
			typename PFP::REAL dist = (ip - position[it]).norm2();
			if(dist < minDist)
			{
				minDist = dist;
				vertex = it;
			}
			it = map.phi1(it);
		}
	}
	else
		vertex = NIL;
}

/**
 * Fonction that do the selection of darts, returned darts are sorted from closest to farthest
 * Dart is here considered as a triangle formed by the 2 end vertices of the edge and the face centroid
 * @param map the map we want to test
 * @param rayA first point of  ray (user side)
 * @param rayAB vector of ray (directed ot the scene)
 * @param vecDarts (out) vector to store dart of intersected darts
 */
template<typename PFP>
void dartsRaySelection(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position, const typename PFP::VEC3& rayA, const typename PFP::VEC3& rayAB, std::vector<Dart>& vecDarts, const FunctorSelect& good = allDarts)
{
	// recuperation des brins intersectes
	vecDarts.clear();
	Algo::Selection::FuncDartMapD2Inter<PFP> ffi(map, position, vecDarts, rayA, rayAB);
	map.template foreach_orbit<FACE>(ffi, good);

	typedef std::pair<typename PFP::REAL, Dart> DartDist;
	std::vector<DartDist> distndart;

	unsigned int nbi = vecDarts.size();
	distndart.resize(nbi);

	// compute all distances to observer for each dart of middle of edge
	// and put them in a vector for sorting
	for (unsigned int i = 0; i < nbi; ++i)
	{
		Dart d = vecDarts[i];
		distndart[i].second = d;
		typename PFP::VEC3 V = (position[d] + position[map.phi1(d)]) / typename PFP::REAL(2);
		V -= rayA;
		distndart[i].first = V.norm2();
	}

	// sort the vector of pair dist/dart
	std::sort(distndart.begin(), distndart.end(), distndartOrdering<PFP>);

	// store sorted darts in returned vector
	for (unsigned int i=0; i< nbi; ++i)
		vecDarts[i] = distndart[i].second;
}

} //namespace Selection

} //namespace Algo

} //namespace CGoGN

#endif /* RAYSELECTOR_H_ */
