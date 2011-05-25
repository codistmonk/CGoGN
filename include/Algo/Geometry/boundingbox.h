/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009-2011, IGG Team, LSIIT, University of Strasbourg           *
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
* Web site: http://cgogn.u-strasbg.fr/                                         *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#ifndef __ALGO_GEOMETRY_BOUNDINGBOX_H__
#define __ALGO_GEOMETRY_BOUNDINGBOX_H__

#include "Geometry/basic.h"
#include "Geometry/bounding_box.h"
#include "Topology/generic/cellmarker.h"

namespace CGoGN
{

namespace Algo
{

namespace Geometry
{

template <typename PFP>
Geom::BoundingBox<typename PFP::VEC3> computeBoundingBox(typename PFP::MAP& map, const typename PFP::TVEC3& position, const FunctorSelect& select = SelectorTrue())
{
	Geom::BoundingBox<typename PFP::VEC3> bb(position[map.begin()]) ;

	CellMarker vmarker(map, VERTEX_CELL);
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(select(d) && !vmarker.isMarked(d))
		{
			vmarker.mark(d);
			bb.addPoint(position[d]) ;
		}
	}

	return bb ;
}

} // namespace Geometry

} // namespace Algo

} // namespace CGoGN

#endif
