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

#ifndef __SUBDIVISION3_H__
#define __SUBDIVISION3_H__

#include <math.h>
#include <vector>
#include "Geometry/plane_3d.h"

namespace CGoGN
{

namespace Algo
{

namespace Modelisation
{

/**
* Cut a 3D ear from a mesh : the ear is sewn by phi3 to the rest of the volume
* @param d dart of the point of the ear
* @return a dart from the new face connecting the ear and the rest of the volume
*/
template <typename PFP>
Dart cut3Ear(typename PFP::MAP& map, Dart d);

/**
* Cut a volume considering a plane
* @param d dart of the volume
* @return a dart from the face in the midle
*/
template <typename PFP>
Dart sliceConvexVolume(typename PFP::MAP& map, typename PFP::TVEC3& position, Dart d, Geom::Plane3D<typename PFP::REAL > pl);


/**
* catmull clark volumic : do not move the original vertices
* @param map the map
* @param attributs geometric attributes of the vertices
* @param selected a functor to select volumes to subdivide
* TODO : test if it works for the functorselect
*/
template <typename PFP, typename EMBV, typename EMB>
void catmullClarkVol(typename PFP::MAP& map, EMBV& attributs, const FunctorSelect& selected= allDarts);

template <typename PFP>
void catmullClarkVol(typename PFP::MAP& map, typename PFP::TVEC3& position, const FunctorSelect& selected= allDarts)
{
	catmullClarkVol<PFP,typename PFP::TVEC3, typename PFP::VEC3>(map, position, selected);
}

} // namespace Modelisation

} // namespace Algo

} // namespace CGoGN

#include "Algo/Modelisation/subdivision3.hpp"

#endif


