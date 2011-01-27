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

#ifndef GENERIC_FUNC
#define GENERIC_FUNC

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <list>
#include <vector>

#include "Topology/generic/functor.h"
#include "Utils/marker.h"

namespace CGoGN
{

/**
* mark darts of a cell with marker m
* Warning: works only if any darts of vertex is already marked
* @param dim the dimension of the cell: 0=vertex, 1=edge, 2=face, 3=volume, -1=connected component
* @param MF type of Map used for traversals
* @param d a dart of the cc
* @param m index of the marker
*/
template <typename MF, typename MM>
void markOrbitGen(int dim, typename MM::Dart d, Marker m, MM *ptr);

/**
* unmark darts of a cell with marker m
* Warning: works only if any darts of vertex is already marked
* @param dim the dimension of the cell: 0=vertex, 1=edge, 2=face, 3=volume, -1=connected component
* @param MF type of Map used for traversals
* @param d a dart of the cc
* @param m index of the marker
*/
template <typename MF, typename MM>
void unmarkOrbitGen(int dim, typename MM::Dart d, Marker m, MM* ptr);

/**
* execute functor for each cell
* @param dim the dimension of the cell: 0=vertex, 1=edge, 2=face, 3=volume, -1=connected component
* @param MF type of Map used for traversals
* @param f the functor
*/
template <typename MF, typename MM>
void foreach_orbitGen(int dim, FunctorType<typename MM>& fonct, MM* ptr);

template <typename MF, typename MM>
void foreach_orbitGen_sel(int dim, FunctorType<typename MM>& fonct, MM* ptr, FunctorType<typename MM>& good);

/**
* Associate an embedding to all darts of a vertex
* @param dim the dimension of the cell: 0=vertex, 1=edge, 2=face, 3=volume, -1=connected component
* @param MF type of Map used for traversals
* @param d a dart of the topological vertex
* @param index the index of Embedding to use
* @param em the embedding to associate
* ptr an ptr on the Map
*/
template <typename MF, typename MM>
void embedOrbitGen(int dim, typename MM::Dart d, int index, Embedding* em, MM* ptr);

} //namespace CGoGN

#include "Topology/generic/genericFunc.hpp"

#endif
