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

#ifndef __DART_MARKER__
#define __DART_MARKER__

#include "Topology/generic/marker.h"
#include "Topology/generic/genericmap.h"
#include "Topology/generic/functor.h"

namespace CGoGN
{


/**
 * generic class that allows the marking of darts
 * \warning no default constructor
 */
class DartMarkerGen
{
protected:
	GenericMap& m_map ;
	unsigned int m_thread ;
	Mark m_mark ;

public:
	/**
	 * constructor
	 * @param map the map on which we work
	 */
	DartMarkerGen(GenericMap& map): m_map(map), m_thread(0)
	{
//		m_mark = m_map.m_marksets[DART][m_thread].getNewMark() ;
		m_mark = m_map.getMarkerSet(DART,m_thread).getNewMark() ;
	}

	DartMarkerGen(GenericMap& map, unsigned int thread): m_map(map), m_thread(thread)
	{
//		m_mark = m_map.m_marksets[DART][m_thread].getNewMark() ;
		m_mark = m_map.getMarkerSet(DART,m_thread).getNewMark() ;
	}

	virtual ~DartMarkerGen()
	{
//		m_map.m_marksets[DART][m_thread].releaseMark(m_mark) ;
		m_map.getMarkerSet(DART,m_thread).releaseMark(m_mark) ;
	}

protected:
	// protected copy constructor to forbid its usage
	DartMarkerGen(const DartMarkerGen& dm) : m_map(dm.m_map), m_thread(0), m_mark(NIL)
	{}

public:
	/**
	 * mark the dart
	 */
	virtual void mark(Dart d)
	{
		assert(m_map.getMarkerSet(DART,m_thread).testMark(m_mark));
		m_map.getMarkVector(DART, m_thread)->operator[](d.index).setMark(m_mark);
	}

	/**
	 * unmark the dart
	 */
	virtual void unmark(Dart d)
	{
		assert(m_map.getMarkerSet(DART,m_thread).testMark(m_mark));
		m_map.getMarkVector(DART, m_thread)->operator[](d.index).unsetMark(m_mark);
	}

	/**
	 * test if dart is marked
	 */
	virtual bool isMarked(Dart d)
	{
		assert(m_map.getMarkerSet(DART,m_thread).testMark(m_mark));
		return m_map.getMarkVector(DART, m_thread)->operator[](d.index).testMark(m_mark);
	}

	/**
	 * mark the darts of the given orbit of d
	 */
	virtual void markOrbit(unsigned int orbit, Dart d)
	{
		assert(m_map.getMarkerSet(DART,m_thread).testMark(m_mark));
		FunctorMark<GenericMap> fm(m_map, m_mark, m_map.getMarkVector(DART, m_thread)) ;
		m_map.foreach_dart_of_orbit(orbit, d, fm, m_thread) ;
	}

	/**
	 * unmark the darts of the given orbit of d
	 */
	virtual void unmarkOrbit(unsigned int orbit, Dart d)
	{
		assert(m_map.getMarkerSet(DART,m_thread).testMark(m_mark));
		FunctorUnmark<GenericMap> fm(m_map, m_mark, m_map.getMarkVector(DART, m_thread)) ;
		m_map.foreach_dart_of_orbit(orbit, d, fm, m_thread) ;
	}

	/**
	 * mark the darts of the given orbit of d in the parent map
	 */
	template <typename MAP>
	void markOrbitInParent(unsigned int orbit, Dart d)
	{
		assert(m_map.getMarkerSet(DART,m_thread).testMark(m_mark));
		FunctorMark<GenericMap> fm(m_map, m_mark, m_map.getMarkVector(DART, m_thread)) ;
		foreach_dart_of_orbit_in_parent<MAP>(dynamic_cast<MAP*>(&m_map), orbit, d, fm, m_thread);
	}

	/**
	 * unmark the darts of the given orbit of d in the parent map
	 */
	template <typename MAP>
	void unmarkOrbitInParent(unsigned int orbit, Dart d)
	{
		assert(m_map.getMarkerSet(DART,m_thread).testMark(m_mark));
		FunctorUnmark<GenericMap> fm(m_map, m_mark, m_map.getMarkVector(DART, m_thread)) ;
		foreach_dart_of_orbit_in_parent<MAP>(dynamic_cast<MAP*>(&m_map), orbit, d, fm, m_thread);
	}

	/**
	 * mark all darts
	 */
	virtual void markAll()
	{
		assert(m_map.getMarkerSet(DART,m_thread).testMark(m_mark));
		AttributeContainer& cont = m_map.getAttributeContainer(DART) ;
		for (unsigned int i = cont.begin(); i != cont.end(); cont.next(i))
			m_map.getMarkVector(DART, m_thread)->operator[](i).setMark(m_mark);
	}

	/**
	 * unmark all darts
	 */
	virtual void unmarkAll() = 0 ;
};

/**
 * class that allows the marking of darts
 * \warning no default constructor
 */
class DartMarker : public DartMarkerGen
{
public:
	DartMarker(GenericMap& map) : DartMarkerGen(map)
	{}

	DartMarker(GenericMap& map, unsigned int th): DartMarkerGen(map, th)
	{}

	virtual ~DartMarker()
	{
		unmarkAll() ;
	}

protected:
	DartMarker(const DartMarker& dm) : DartMarkerGen(dm)
	{}

public:
	void unmarkAll()
	{
		assert(m_map.getMarkerSet(DART,m_thread).testMark(m_mark));

		AttributeContainer& cont = m_map.getAttributeContainer(DART) ;
		for (unsigned int i = cont.begin(); i != cont.end(); cont.next(i))
			m_map.getMarkVector(DART, m_thread)->operator[](i).unsetMark(m_mark);
	}
};

/**
 * class that allows the marking of darts
 * the marked darts are stored to optimize the unmarking task at destruction
 * \warning no default constructor
 */
class DartMarkerStore: public DartMarkerGen
{
protected:
	std::vector<unsigned int> m_markedDarts ;

public:
	DartMarkerStore(GenericMap& map) : DartMarkerGen(map)
	{}

	DartMarkerStore(GenericMap& map, unsigned int thread): DartMarkerGen(map, thread)
	{}

	~DartMarkerStore()
	{
		unmarkAll() ;
	}

protected:
	DartMarkerStore(const DartMarkerStore& dm) : DartMarkerGen(dm),m_markedDarts(dm.m_markedDarts)
	{}

public:
	void mark(Dart d)
	{
		DartMarkerGen::mark(d) ;
		m_markedDarts.push_back(d.index) ;
	}

	void markOrbit(unsigned int orbit, Dart d)
	{
		assert(m_map.getMarkerSet(DART,m_thread).testMark(m_mark));
		FunctorMarkStore<GenericMap> fm(m_map, m_mark, m_map.getMarkVector(DART, m_thread), m_markedDarts) ;
		m_map.foreach_dart_of_orbit(orbit, d, fm, m_thread) ;
	}

	template <typename MAP>
	void markOrbitInParent(unsigned int orbit, Dart d)
	{
		assert(m_map.getMarkerSet(DART,m_thread).testMark(m_mark));
		FunctorMarkStore<GenericMap> fm(m_map, m_mark, m_map.getMarkVector(DART, m_thread), m_markedDarts) ;
		foreach_dart_of_orbit_in_parent<MAP>(dynamic_cast<MAP*>(&m_map), orbit, d, fm, m_thread);
	}

//	void mark(unsigned int d)
//	{
//		DartMarkerGen::mark(d) ;
//		m_markedDarts.push_back(d) ;
//	}

	void unmarkAll()
	{
		assert(m_map.getMarkerSet(DART,m_thread).testMark(m_mark));
		for (std::vector<unsigned int>::iterator it = m_markedDarts.begin(); it != m_markedDarts.end(); ++it)
			m_map.getMarkVector(DART, m_thread)->operator[](*it).unsetMark(m_mark) ;
	}
};

/**
 * class that allows the marking of darts
 * the markers are not unmarked at destruction
 * \warning no default constructor
 */
class DartMarkerNoUnmark: public DartMarkerGen
{
public:
	DartMarkerNoUnmark(GenericMap& map): DartMarkerGen(map)
	{}

	DartMarkerNoUnmark(GenericMap& map, unsigned int th): DartMarkerGen(map,th)
	{}

	~DartMarkerNoUnmark()
	{}

protected:
	DartMarkerNoUnmark(const DartMarkerNoUnmark& dm) : DartMarkerGen(dm)
	{}

public:
	void unmarkAll()
	{
		assert(m_map.getMarkerSet(DART,m_thread).testMark(m_mark));
		AttributeContainer& cont = m_map.getAttributeContainer(DART) ;
		for (unsigned int i = cont.begin(); i != cont.end(); cont.next(i))
			m_map.getMarkVector(DART, m_thread)->operator[](i).unsetMark(m_mark) ;
	}
};

// Selector and count functors testing for marker existence
/********************************************************/

class SelectorMarked : public FunctorSelect
{
protected:
	DartMarkerGen& m_marker;
public:
	SelectorMarked(DartMarkerGen& m) : m_marker(m) {}
	bool operator()(Dart d) const
	{
		return m_marker.isMarked(d);
	}
};

class SelectorUnmarked : public FunctorSelect
{
protected:
	DartMarkerGen& m_marker;
public:
	SelectorUnmarked(DartMarkerGen& m) : m_marker(m) {}
	bool operator()(Dart d) const
	{
		return !m_marker.isMarked(d);
	}
};

// Functor version (needed for use with foreach_xxx)

class FunctorIsMarked : public FunctorType
{
protected:
	DartMarkerGen& m_marker;
public:
	FunctorIsMarked(DartMarkerGen& dm) : m_marker(dm) {}
	bool operator()(Dart d)
	{
		return m_marker.isMarked(d);
	}
};

class FunctorIsUnmarked : public FunctorType
{
protected:
	DartMarkerGen& m_marker;
public:
	FunctorIsUnmarked(DartMarkerGen& dm) : m_marker(dm) {}
	bool operator()(Dart d)
	{
		return !m_marker.isMarked(d);
	}
};

} // namespace CGoGN

#endif
