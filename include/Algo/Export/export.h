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

#ifndef __EXPORT_H__
#define __EXPORT_H__

#include "Topology/generic/attributeHandler.h"

namespace CGoGN
{

namespace Algo
{

namespace Export
{

/**
* export the map into a PLY file
* @param the_map map to be exported
* @param filename filename of ply file
* @return true
*/
template <typename PFP>
bool exportPLY(typename PFP::MAP& map, const typename PFP::TVEC3& position, const char* filename, const FunctorSelect& good = allDarts) ;

/**
* export the map into a OFF file
* @param the_map map to be exported
* @param filename filename of off file
* @return true
*/
template <typename PFP>
bool exportOFF(typename PFP::MAP& map, const typename PFP::TVEC3& position, const char* filename, const FunctorSelect& good = allDarts) ;

/**
* export the map into a Trian file
* @param the_map map to be exported
* @param filename filename of trian file
* @return true
*/
template <typename PFP>
bool exportTrian(typename PFP::MAP& map, const typename PFP::TVEC3& position, char* filename, const FunctorSelect& good = allDarts) ;

/**
* export the map into a PLYPTMgeneric file (K. Vanhoey generic format).
*
* exports position + any attribute named : "frame_T" (frame tangent : VEC3), "frame_B" (frame binormal : VEC3), "frame_N" (frame normal : VEC3),
* "colorPTM_a<i> : VEC3" (coefficient number i of the 3 polynomials - one per channel - ; the max i depends on the degree of the PTM polynomial),
* "errL2 : REAL" (L2 fitting error), "errLmax : REAL" (maximal fitting error), "stdDev : REAL" (standard deviation of the L2 fitting errors).
*
* @param map map to be exported
* @param filename filename of ply file
* @param position the position container
* @return true
*/
template <typename PFP>
bool exportPlyPTMgeneric(typename PFP::MAP& map, const char* filename, const typename PFP::TVEC3& position, const FunctorSelect& good = allDarts) ;

/**
* export the map into a PLYPTMgeneric file (K. Vanhoey generic format)
* @param map map to be exported
* @param filename filename of ply file
* @param position the position container
* @param the local frame (3xVEC3 : tangent, bitangent, normal)
* @param colorPTM the 6 coefficients (x3 channels) of the PTM functions
* @return true
*/
template <typename PFP>
bool exportPLYPTM(typename PFP::MAP& map, const char* filename, const typename PFP::TVEC3& position, const typename PFP::TVEC3 frame[3], const typename PFP::TVEC3 colorPTM[6], const FunctorSelect& good) ;

} // namespace Export

} // namespace Algo

} // namespace CGoGN

#include "Algo/Export/export.hpp"

#endif
