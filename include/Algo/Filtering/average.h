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

#include "Algo/Filtering/functors.h"
#include "Algo/Selection/collector.h"

namespace CGoGN
{

namespace Algo
{

namespace Filtering
{

enum neighborhood { INSIDE = 1, BORDER = 2 };

template <typename PFP, typename T>
void filterAverageAttribute_OneRing(
	typename PFP::MAP& map,
	const AttributeHandler<T>& attIn,
	AttributeHandler<T>& attOut,
	int neigh,
	const FunctorSelect& select = SelectorTrue())
{
	FunctorAverage<T> fa(attIn) ;
	Algo::Selection::Collector_OneRing<PFP> col(map) ;

	CellMarker markV(map, VERTEX) ;
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(select(d) && !markV.isMarked(d))
		{
			markV.mark(d) ;

			if (neigh & INSIDE)
				col.collectAll(d) ;
			else
				col.collectBorder(d) ;

			fa.reset() ;
			if (neigh & INSIDE)
			{
				switch (attIn.getOrbit())
				{
				case VERTEX :
					col.applyOnInsideVertices(fa) ;
					break;
				case EDGE :
					col.applyOnInsideEdges(fa) ;
					break;
				case FACE :
					col.applyOnInsideFaces(fa) ;
					break;
				}
			}
			if (neigh & BORDER) col.applyOnBorder(fa) ;
			attOut[d] = fa.getAverage() ;
		}
	}
}

template <typename PFP, typename T>
void filterAverageEdgesAttribute_WithinSphere(
	typename PFP::MAP& map,
	const AttributeHandler<T>& attIn,
	AttributeHandler<T>& attOut,
	int neigh,
	typename PFP::TVEC3 & position,
	typename PFP::REAL radius,
	const FunctorSelect& select = SelectorTrue())
{
	FunctorAverage<T> fa(attIn) ;
	Algo::Selection::Collector_WithinSphere<PFP> col(map, position, radius) ;

	CellMarker markV(map, VERTEX) ;
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(select(d) && !markV.isMarked(d))
		{
			markV.mark(d) ;

			if (neigh & INSIDE)
				col.collectAll(d) ;
			else
				col.collectBorder(d) ;

			fa.reset() ;
			if (neigh & INSIDE) col.applyOnInsideEdges(fa) ;
			if (neigh & BORDER) col.applyOnBorder(fa) ;
			attOut[d] = fa.getAverage() ;
		}
	}
}

template <typename PFP, typename T>
void filterAverageFacesAttribute_WithinSphere(
	typename PFP::MAP& map,
	const AttributeHandler<T>& attIn,
	AttributeHandler<T>& attOut,
	int neigh,
	typename PFP::TVEC3 & position,
	typename PFP::REAL radius,
	const FunctorSelect& select = SelectorTrue())
{

	FunctorAverage<T> fa(attIn) ;
	Algo::Selection::Collector_WithinSphere<PFP> col(map, position, radius) ;

	CellMarker markV(map, VERTEX) ;
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(select(d) && !markV.isMarked(d))
		{
			markV.mark(d) ;

			if (neigh & INSIDE)
				col.collectAll(d) ;
			else
				col.collectBorder(d) ;

			fa.reset() ;
			if (neigh & INSIDE) col.applyOnInsideFaces(fa) ;
			if (neigh & BORDER) col.applyOnBorder(fa) ;
			attOut[d] = fa.getAverage() ;
		}
	}
}

template <typename PFP, typename T>
void filterAverageVertexAttribute_WithinSphere(
	typename PFP::MAP& map,
	const AttributeHandler<T>& attIn,
	AttributeHandler<T>& attOut,
	int neigh,
	typename PFP::TVEC3 & position,
	typename PFP::REAL radius,
	const FunctorSelect& select = SelectorTrue())
{
	FunctorAverage<T> faInside(attIn) ;
	FunctorAverageOnSphereBorder<PFP, T> faBorder(map, attIn, position) ;
	Algo::Selection::Collector_WithinSphere<PFP> col(map, position, radius) ;

	CellMarker markV(map, VERTEX) ;
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(select(d) && !markV.isMarked(d))
		{
			markV.mark(d) ;

			if (neigh & INSIDE)
				col.collectAll(d) ;
			else
				col.collectBorder(d) ;

			attOut[d] = T(0);
			if (neigh & INSIDE){
				faInside.reset() ;
				col.applyOnInsideVertices(faInside) ;
				attOut[d] += faInside.getSum();
			}
			if (neigh & BORDER){
				faBorder.reset(position[d], radius);
				col.applyOnBorder(faBorder) ;
				attOut[d] += faBorder.getSum();
			}
			attOut[d] /= faInside.getCount() + faBorder.getCount() ;
		}
	}
}

} // namespace Filtering

} // namespace Algo

} // namespace CGoGN
