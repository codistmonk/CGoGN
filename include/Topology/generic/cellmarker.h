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

#ifndef __CELL_MARKER__
#define __CELL_MARKER__

#include "Topology/generic/marker.h"
#include "Topology/generic/genericmap.h"
#include "Topology/generic/functor.h"
#include "Utils/static_assert.h"

namespace CGoGN
{

class CellMarkerGen
{
	friend class GenericMap ;

protected:
	GenericMap& m_map ;
	Mark m_mark ;
	unsigned int m_thread ;
	bool releaseOnDestruct ;

public:
	CellMarkerGen(GenericMap& map, unsigned int thread = 0) : m_map(map), m_thread(thread), releaseOnDestruct(true)
	{}

	virtual ~CellMarkerGen()
	{}

	/**
	 * set if the mark has to be release on destruction or not
	 */
	void setReleaseOnDestruct(bool b) { releaseOnDestruct = b ; }

	virtual void mark(Dart d) = 0 ;
	virtual void unmark(Dart d) = 0 ;
	virtual bool isMarked(Dart d) const = 0 ;
	virtual void mark(unsigned int em) = 0 ;
	virtual void unmark(unsigned int em) = 0 ;
	virtual bool isMarked(unsigned int em) const = 0 ;
	virtual void markAll() = 0 ;
	virtual void unmarkAll() = 0 ;
	virtual bool isAllUnmarked() = 0 ;
};

/**
 * generic class that allows the marking of cells
 * \warning no default constructor
 */
template <unsigned int CELL>
class CellMarkerBase : public CellMarkerGen
{
protected:
	AttributeMultiVector<Mark>* m_markVector ;

public:
	/**
	 * constructor
	 * @param map the map on which we work
	 */
	CellMarkerBase(GenericMap& map, unsigned int thread = 0) : CellMarkerGen(map, thread)
	{
		if(!map.isOrbitEmbedded<CELL>())
			map.addEmbedding<CELL>() ;
		m_mark = m_map.getMarkerSet<CELL>(m_thread).getNewMark() ;
		m_markVector = m_map.getMarkVector<CELL>(m_thread) ;
		m_map.cellMarkers.push_back(this) ;
	}

	virtual ~CellMarkerBase()
	{
		if(releaseOnDestruct)
		{
			m_map.getMarkerSet<CELL>(m_thread).releaseMark(m_mark) ;
			for(std::vector<CellMarkerGen*>::iterator it = m_map.cellMarkers.begin(); it != m_map.cellMarkers.end(); ++it)
			{
				if(*it == this)
				{
					*it = m_map.cellMarkers.back();
					m_map.cellMarkers.pop_back();
					return;
				}
			}
		}
	}

protected:
	// protected copy constructor to forbid its usage
	CellMarkerBase(const CellMarkerGen& cm) : CellMarkerGen(cm.m_map)
	{}

public:
	/**
	 * mark the cell of dart
	 */
	virtual void mark(Dart d)
	{
		assert(m_map.getMarkerSet<CELL>(m_thread).testMark(m_mark));
		assert(m_map.getMarkVector<CELL>(m_thread) != NULL);

		unsigned int a = m_map.getEmbedding<CELL>(d) ;
		if (a == EMBNULL)
			a = m_map.embedNewCell<CELL>(d) ;

		m_markVector->operator[](a).setMark(m_mark) ;
	}

	/**
	 * unmark the cell of dart
	 */
	virtual void unmark(Dart d)
	{
		assert(m_map.getMarkerSet<CELL>(m_thread).testMark(m_mark));
		assert(m_markVector != NULL);

		unsigned int a = m_map.getEmbedding<CELL>(d) ;
		if (a == EMBNULL)
			a = m_map.embedNewCell<CELL>(d) ;

		m_markVector->operator[](a).unsetMark(m_mark) ;
	}

	/**
	 * test if cell of dart is marked
	 */
	virtual bool isMarked(Dart d) const
	{
		assert(m_map.getMarkerSet<CELL>(m_thread).testMark(m_mark));
		assert(m_markVector != NULL);

		unsigned int a = m_map.getEmbedding<CELL>(d) ;
		if (a == EMBNULL)
			return false ;

		return m_markVector->operator[](a).testMark(m_mark) ;
	}

	/**
	 * mark the cell
	 */
	virtual void mark(unsigned int em)
	{
		assert(m_map.getMarkerSet<CELL>(m_thread).testMark(m_mark));
		assert(m_markVector != NULL);

		m_markVector->operator[](em).setMark(m_mark) ;
	}

	/**
	 * unmark the cell
	 */
	virtual void unmark(unsigned int em)
	{
		assert(m_map.getMarkerSet<CELL>(m_thread).testMark(m_mark));
		assert(m_markVector != NULL);

		m_markVector->operator[](em).unsetMark(m_mark) ;
	}

	/**
	 * test if cell is marked
	 */
	virtual bool isMarked(unsigned int em) const
	{
		assert(m_map.getMarkerSet<CELL>(m_thread).testMark(m_mark));
		assert(m_markVector != NULL);

		return m_markVector->operator[](em).testMark(m_mark) ;
	}

	/**
	 * mark all the cells
	 */
	virtual void markAll()
	{
		assert(m_map.getMarkerSet<CELL>(m_thread).testMark(m_mark));
		assert(m_markVector != NULL);

		AttributeContainer& cont = m_map.getAttributeContainer<CELL>() ;
		for (unsigned int i = cont.begin(); i != cont.end(); cont.next(i))
			m_markVector->operator[](i).setMark(m_mark) ;
	}

	virtual bool isAllUnmarked()
	{
		assert(m_map.getMarkerSet<CELL>(m_thread).testMark(m_mark));
		assert(m_markVector != NULL);

		AttributeContainer& cont = m_map.getAttributeContainer<CELL>() ;
		for (unsigned int i = cont.begin(); i != cont.end(); cont.next(i))
			if(m_markVector->operator[](i).testMark(m_mark))
				return false ;
		return true ;
	}
};

/**
 * class that allows the marking of cells
 * \warning no default constructor
 */
template <unsigned int CELL>
class CellMarker : public CellMarkerBase<CELL>
{
public:
	CellMarker(GenericMap& map, unsigned int thread = 0) : CellMarkerBase<CELL>(map, thread)
	{}

	virtual ~CellMarker()
	{
		unmarkAll() ;
	}

protected:
	CellMarker(const CellMarker& cm) : CellMarkerBase<CELL>(cm)
	{}

public:
	virtual void unmarkAll()
	{
		assert(this->m_map.template getMarkerSet<CELL>(this->m_thread).testMark(this->m_mark));
		assert(this->m_markVector != NULL);

		AttributeContainer& cont = this->m_map.template getAttributeContainer<CELL>() ;
		for (unsigned int i = cont.begin(); i != cont.end(); cont.next(i))
			this->m_markVector->operator[](i).unsetMark(this->m_mark) ;
	}
};

/**
 * class that allows the marking of cells
 * the marked cells are stored to optimize the unmarking task at destruction
 * \warning no default constructor
 */
template <unsigned int CELL>
class CellMarkerStore: public CellMarkerBase<CELL>
{
protected:
	std::vector<unsigned int> m_markedCells ;

public:
	CellMarkerStore(GenericMap& map, unsigned int thread = 0) : CellMarkerBase<CELL>(map, thread)
	{}


	virtual ~CellMarkerStore()
	{
		unmarkAll() ;
//		assert(isAllUnmarked);
		CGoGN_ASSERT(this->isAllUnmarked())
	}

protected:
	CellMarkerStore(const CellMarkerStore& cm) : CellMarkerBase<CELL>(cm)
	{}

public:
	void mark(Dart d)
	{
		CellMarkerBase<CELL>::mark(d) ;
		m_markedCells.push_back(this->m_map.template getEmbedding<CELL>(d)) ;
	}

	void mark(unsigned int em)
	{
		CellMarkerBase<CELL>::mark(em) ;
		m_markedCells.push_back(em) ;
	}

	void unmarkAll()
	{
		assert(this->m_map.template getMarkerSet<CELL>(this->m_thread).testMark(this->m_mark));
		assert(this->m_markVector != NULL);

		for (std::vector<unsigned int>::iterator it = m_markedCells.begin(); it != m_markedCells.end(); ++it)
			this->m_markVector->operator[](*it).unsetMark(this->m_mark) ;
	}
};

/**
 * class that allows the marking of cells
 * the markers are not unmarked at destruction
 * \warning no default constructor
 */
template <unsigned int CELL>
class CellMarkerNoUnmark: public CellMarkerBase<CELL>
{
public:
	CellMarkerNoUnmark(GenericMap& map, unsigned int thread = 0) : CellMarkerBase<CELL>(map, thread)
	{}

	virtual ~CellMarkerNoUnmark()
	{
//		assert(isAllUnmarked()) ;
		CGoGN_ASSERT(this->isAllUnmarked())
	}

protected:
	CellMarkerNoUnmark(const CellMarkerNoUnmark& cm) : CellMarkerBase<CELL>(cm)
	{}

public:
	void unmarkAll()
	{
		assert(this->m_map.template getMarkerSet<CELL>(this->m_thread).testMark(this->m_mark));
		assert(this->m_markVector != NULL);

		AttributeContainer& cont = this->m_map.template getAttributeContainer<CELL>() ;
		for (unsigned int i = cont.begin(); i != cont.end(); cont.next(i))
			this->m_markVector->operator[](i).unsetMark(this->m_mark) ;
	}
};


/**
 * selector that say if a dart has its cell marked
 */
template <unsigned int CELL>
class SelectorCellMarked : public FunctorSelect
{
protected:
	const CellMarkerBase<CELL>& m_cmarker ;
public:
	SelectorCellMarked(const CellMarkerBase<CELL>& cm) : m_cmarker(cm) {}
	bool operator()(Dart d) const
	{
		if (m_cmarker.isMarked(d))
			return true ;
		return false ;
	}
	FunctorSelect* copy() const { return new SelectorCellMarked(m_cmarker);}
};

template <unsigned int CELL>
class SelectorCellUnmarked : public FunctorSelect
{
protected:
	const CellMarkerBase<CELL>& m_cmarker ;
public:
	SelectorCellUnmarked(const CellMarkerBase<CELL>& cm) : m_cmarker(cm) {}
	bool operator()(Dart d) const
	{
		if (!m_cmarker.isMarked(d))
			return true ;
		return false ;
	}
	FunctorSelect* copy() const { return new SelectorCellUnmarked(m_cmarker);}
};

// Functor version (needed for use with foreach_xxx)

template <unsigned int CELL>
class FunctorCellIsMarked : public FunctorType
{
protected:
	CellMarkerBase<CELL>& m_marker;
public:
	FunctorCellIsMarked(CellMarkerBase<CELL>& cm) : m_marker(cm) {}
	bool operator()(Dart d)
	{
		return m_marker.isMarked(d);
	}
};

template <unsigned int CELL>
class FunctorCellIsUnmarked : public FunctorType
{
protected:
	CellMarkerBase<CELL>& m_marker;
public:
	FunctorCellIsUnmarked(CellMarkerBase<CELL>& cm) : m_marker(cm) {}
	bool operator()(Dart d)
	{
		return !m_marker.isMarked(d);
	}
};

} // namespace CGoGN

#endif
