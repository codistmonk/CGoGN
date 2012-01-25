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

#ifndef __TRAVERSOR_CELL_H__
#define __TRAVERSOR_CELL_H__

#include "Topology/generic/dart.h"
#include "Topology/generic/dartmarker.h"
#include "Topology/generic/cellmarker.h"

namespace CGoGN
{
//
//template <typename MAP>
//class TraversorCell
//{
//private:
//	MAP& m ;
//	unsigned int m_orbit ;
//	DartMarker* dmark ;
//	CellMarker* cmark ;
//	Dart current ;
//	bool firstTraversal ;
//	const FunctorSelect& m_good ;
//
//public:
//	TraversorCell(MAP& map, unsigned int orbit, const FunctorSelect& good = allDarts, bool forceDartMarker = false, unsigned int thread = 0) :
//		m(map), m_orbit(orbit), dmark(NULL), cmark(NULL), current(NIL), firstTraversal(true), m_good(good)
//	{
//		if(!forceDartMarker && map.isOrbitEmbedded(m_orbit))
//			cmark = new CellMarker(map, m_orbit, thread) ;
//		else
//			dmark = new DartMarker(map, thread) ;
//	}
//
//	~TraversorCell()
//	{
//		if(dmark)
//			delete dmark ;
//		else
//			delete cmark ;
//	}
//
//	Dart begin()
//	{
//		if(!firstTraversal)
//		{
//			if(dmark)
//				dmark->unmarkAll() ;
//			else
//				cmark->unmarkAll() ;
//		}
//
//		current = m.begin() ;
//		while(current != m.end() && (m.isBoundaryMarked(current) || !m_good(current)))
//			m.next(current) ;
//
//		if(current == m.end())
//			current = NIL ;
//		else
//		{
//			if(dmark)
//				dmark->markOrbit(m_orbit, current) ;
//			else
//				cmark->mark(current) ;
//		}
//
//		firstTraversal = false ;
//		return current ;
//	}
//
//	Dart end() { return NIL ; }
//
//	Dart next()
//	{
//		if(current != NIL)
//		{
//			bool ismarked ;
//			if(dmark)
//				ismarked = dmark->isMarked(current) ;
//			else
//				ismarked = cmark->isMarked(current) ;
//
//			while(current != NIL && (ismarked || m.isBoundaryMarked(current) || !m_good(current)))
//			{
//				m.next(current) ;
//				if(current == m.end())
//					current = NIL ;
//				else
//				{
//					if(dmark)
//						ismarked = dmark->isMarked(current) ;
//					else
//						ismarked = cmark->isMarked(current) ;
//				}
//			}
//
//			if(current != NIL)
//			{
//				if(dmark)
//					dmark->markOrbit(m_orbit, current) ;
//				else
//					cmark->mark(current) ;
//			}
//		}
//		return current ;
//	}
//
//	void mark(Dart d)
//	{
//		if(dmark)
//			dmark->markOrbit(m_orbit, d) ;
//		else
//			cmark->mark(d) ;
//	}
//} ;
//
//template <typename MAP>
//class TraversorV : public TraversorCell<MAP>
//{
//public:
//	TraversorV(MAP& m, const FunctorSelect& good = allDarts, unsigned int thread = 0) : TraversorCell<MAP>(m, VERTEX, good, thread)
//	{}
//};
//
//template <typename MAP>
//class TraversorE : public TraversorCell<MAP>
//{
//public:
//	TraversorE(MAP& m, const FunctorSelect& good = allDarts, unsigned int thread = 0) : TraversorCell<MAP>(m, EDGE, good, thread)
//	{}
//};
//
////template <typename MAP>
////class TraversorOF : public TraversorCell<MAP>
////{
////public:
////	TraversorOF(MAP& m, const FunctorSelect& good = allDarts, unsigned int thread = 0) : TraversorCell<MAP>(m, ORIENTED_FACE, good, thread)
////	{}
////};
//
//
//template <typename MAP>
//class TraversorF : public TraversorCell<MAP>
//{
//public:
//	TraversorF(MAP& m, const FunctorSelect& good = allDarts, unsigned int thread = 0) : TraversorCell<MAP>(m, FACE, good, thread)
//	{}
//};
//
//template <typename MAP>
//class TraversorW : public TraversorCell<MAP>
//{
//public:
//	TraversorW(MAP& m, const FunctorSelect& good = allDarts, unsigned int thread = 0) : TraversorCell<MAP>(m, VOLUME, good, thread)
//	{}
//};
//
//
//
//template <typename MAP>
//class TraversorDartsOfOrbit
//{
//private:
//	std::vector<Dart>::iterator m_current;
//	std::vector<Dart> m_vd;
//
//public:
//	TraversorDartsOfOrbit(MAP& map, unsigned int orbit, Dart d, unsigned int thread = 0)
//	{
//		m_vd.reserve(16);
//		FunctorStoreNotBoundary<MAP> fs(map,m_vd);
//		map.foreach_dart_of_orbit(orbit, d, fs, thread);
//		m_vd.push_back(NIL);
//		m_current=m_vd.begin();
//	}
//
//	Dart begin()
//	{
//		m_current=m_vd.begin();
//		return *m_current;
//	}
//
//	Dart end(){ return NIL;}
//
//	Dart next()
//	{
//		if (*m_current != NIL)
//			m_current++;
//		return *m_current;
//	}
//} ;




template <typename MAP>
class TraversorCell
{
private:
	MAP& m ;
	unsigned int m_orbit ;
	DartMarker* dmark ;
	CellMarker* cmark ;
	Dart current ;
	bool firstTraversal ;
	const FunctorSelect& m_good ;

public:
	TraversorCell(MAP& map, unsigned int orbit, const FunctorSelect& good = allDarts, bool forceDartMarker = false, unsigned int thread = 0) ;

	~TraversorCell() ;

	Dart begin() ;

	Dart end() ;

	Dart next() ;

	void mark(Dart d);
} ;

template <typename MAP>
class TraversorV : public TraversorCell<MAP>
{
public:
	TraversorV(MAP& m, const FunctorSelect& good = allDarts, unsigned int thread = 0) : TraversorCell<MAP>(m, VERTEX, good, thread)
	{}
};

template <typename MAP>
class TraversorE : public TraversorCell<MAP>
{
public:
	TraversorE(MAP& m, const FunctorSelect& good = allDarts, unsigned int thread = 0) : TraversorCell<MAP>(m, EDGE, good, thread)
	{}
};


template <typename MAP>
class TraversorF : public TraversorCell<MAP>
{
public:
	TraversorF(MAP& m, const FunctorSelect& good = allDarts, unsigned int thread = 0) : TraversorCell<MAP>(m, FACE, good, thread)
	{}
};

template <typename MAP>
class TraversorW : public TraversorCell<MAP>
{
public:
	TraversorW(MAP& m, const FunctorSelect& good = allDarts, unsigned int thread = 0) : TraversorCell<MAP>(m, VOLUME, good, thread)
	{}
};



template <typename MAP>
class TraversorDartsOfOrbit
{
private:
	std::vector<Dart>::iterator m_current;
	std::vector<Dart> m_vd;

public:
	TraversorDartsOfOrbit(MAP& map, unsigned int orbit, Dart d, unsigned int thread = 0) ;

	Dart begin() ;

	Dart end() ;

	Dart next() ;
} ;

} // namespace CGoGN

#include "Topology/generic/traversorCell.hpp"

#endif
