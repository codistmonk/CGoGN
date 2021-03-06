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

#ifndef __3MR_LOOP_FILTER__
#define __3MR_LOOP_FILTER__

#include <cmath>
#include "Algo/Geometry/centroid.h"
#include "Algo/Modelisation/tetrahedralization.h"
#include "Algo/Multiresolution/filter.h"

namespace CGoGN
{

namespace Algo
{

namespace MR
{

namespace Primal
{

namespace Filters
{

/*********************************************************************************
 *                           LOOP BASIC FUNCTIONS
 *********************************************************************************/
template <typename PFP>
typename PFP::VEC3 loopOddVertex(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position, Dart d1)
{
	Dart d2 = map.phi2(d1) ;
	Dart d3 = map.phi_1(d1) ;
	Dart d4 = map.phi_1(d2) ;

	typename PFP::VEC3 p1 = position[d1] ;
	typename PFP::VEC3 p2 = position[d2] ;
	typename PFP::VEC3 p3 = position[d3] ;
	typename PFP::VEC3 p4 = position[d4] ;

	p1 *= 3.0 / 8.0 ;
	p2 *= 3.0 / 8.0 ;
	p3 *= 1.0 / 8.0 ;
	p4 *= 1.0 / 8.0 ;

	return p1 + p2 + p3 + p4 ;
}

template <typename PFP>
typename PFP::VEC3 loopEvenVertex(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position, Dart d)
{
	map.incCurrentLevel() ;

	typename PFP::VEC3 np(0) ;
	unsigned int degree = 0 ;
	Traversor2VVaE<typename PFP::MAP> trav(map, d) ;
	for(Dart it = trav.begin(); it != trav.end(); it = trav.next())
	{
		++degree ;
		np += position[it] ;
	}

	map.decCurrentLevel() ;

	float mu = 3.0/8.0 + 1.0/4.0 * cos(2.0 * M_PI / degree) ;
	mu = (5.0/8.0 - (mu * mu)) / degree ;
	np *= 8.0/5.0 * mu ;

	return np ;
}

/*********************************************************************************
 *          SHW04 BASIC FUNCTIONS : tetrahedral/octahedral meshes
 *********************************************************************************/
template <typename PFP>
typename PFP::VEC3 SHW04Vertex(typename PFP::MAP& map, const VertexAttribute<typename PFP::VEC3>& position, Dart d)
{
	typename PFP::VEC3 res(0);

	if(Algo::Modelisation::Tetrahedralization::isTetrahedron<PFP>(map, d))
	{
		Dart d1 = map.phi1(d) ;
		Dart d2 = map.phi_1(d);
		Dart d3 = map.phi_1(map.phi2(d));

		typename PFP::VEC3 p = position[d];
		typename PFP::VEC3 p1 = position[d1] ;
		typename PFP::VEC3 p2 = position[d2] ;
		typename PFP::VEC3 p3 = position[d3] ;

		p *= -1;
		p1 *= 17.0 / 3.0;
		p2 *= 17.0 / 3.0;
		p3 *= 17.0 / 3.0;

		res += p + p1 + p2 + p3;
		res *= 1.0 / 16.0;
	}
	else
	{
		Dart d1 = map.phi1(d);
		Dart d2 = map.phi_1(d);
		Dart d3 = map.phi_1(map.phi2(d));
		Dart d4 = map.phi_1(map.phi2(d3));
		Dart d5 = map.phi_1(map.phi2(map.phi_1(d)));

		typename PFP::VEC3 p = position[d];
		typename PFP::VEC3 p1 = position[d1] ;
		typename PFP::VEC3 p2 = position[d2] ;
		typename PFP::VEC3 p3 = position[d3] ;
		typename PFP::VEC3 p4 = position[d4] ;
		typename PFP::VEC3 p5 = position[d5] ;

		p *= 3.0 / 4.0;
		p1 *= 1.0 / 6.0;
		p2 *= 1.0 / 6.0;
		p3 *= 1.0 / 6.0;
		p4 *= 7.0 / 12.0;
		p5 *= 1.0 / 6.0;

		res += p + p1 + p2 + p3 + p4 + p5;
		res *= 1.0 / 2.0;
	}

	return res;
}


/*********************************************************************************
 *                           ANALYSIS FILTERS
 *********************************************************************************/

template <typename PFP>
class LoopEvenAnalysisFilter : public Filter
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;

public:
	LoopEvenAnalysisFilter(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		TraversorV<typename PFP::MAP> trav(m_map) ;
		for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		{
			if(m_map.isBoundaryVertex(d))
			{
				Dart db = m_map.findBoundaryFaceOfVertex(d);
				typename PFP::VEC3 p = loopEvenVertex<PFP>(m_map, m_position, db) ;
				m_position[db] -= p ;
			}
		}
	}
} ;

template <typename PFP>
class LoopNormalisationAnalysisFilter : public Filter
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;

public:
	LoopNormalisationAnalysisFilter(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		TraversorV<typename PFP::MAP> trav(m_map) ;
		for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		{
			if(m_map.isBoundaryVertex(d))
			{
				Dart db = m_map.findBoundaryFaceOfVertex(d);

				unsigned int degree = m_map.vertexDegreeOnBoundary(db) ;
				float n = 3.0/8.0 + 1.0/4.0 * cos(2.0 * M_PI / degree) ;
				n = 8.0/5.0 * (n * n) ;

				m_position[db] /= n ;
			}
		}
	}
} ;

template <typename PFP>
class LoopOddAnalysisFilter : public Filter
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;

public:
	LoopOddAnalysisFilter(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		TraversorE<typename PFP::MAP> trav(m_map) ;
		for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		{
			if(m_map.isBoundaryEdge(d))
			{
				Dart db = m_map.findBoundaryFaceOfEdge(d);
				typename PFP::VEC3 p = loopOddVertex<PFP>(m_map, m_position, db) ;

				m_map.incCurrentLevel() ;

				Dart oddV = m_map.phi2(db) ;
				m_position[oddV] -= p ;

				m_map.decCurrentLevel() ;
			}
			else
			{
				typename PFP::VEC3 p = (m_position[d] + m_position[m_map.phi2(d)]) * typename PFP::REAL(0.5);

				m_map.incCurrentLevel() ;

				Dart midV = m_map.phi2(d) ;
				m_position[midV] -= p ;

				m_map.decCurrentLevel() ;
			}
		}
	}
} ;


/*********************************************************************************
 *                           SYNTHESIS FILTERS
 *********************************************************************************/


/* Loop on Boundary Vertices and SHW04 on Insides Vertices
 *********************************************************************************/
template <typename PFP>
class LoopOddSynthesisFilter : public Filter
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;

public:
	LoopOddSynthesisFilter(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		TraversorE<typename PFP::MAP> trav(m_map) ;
		for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		{
			if(m_map.isBoundaryEdge(d))
			{
				Dart db = m_map.findBoundaryFaceOfEdge(d);
				typename PFP::VEC3 p = loopOddVertex<PFP>(m_map, m_position, db) ;

				m_map.incCurrentLevel() ;

				Dart oddV = m_map.phi2(db) ;
				m_position[oddV] += p ;

				m_map.decCurrentLevel() ;
			}
			else
			{
				typename PFP::VEC3 p = (m_position[d] + m_position[m_map.phi2(d)]) * typename PFP::REAL(0.5);

				m_map.incCurrentLevel() ;

				Dart midV = m_map.phi2(d) ;
				m_position[midV] += p ;

				m_map.decCurrentLevel() ;
			}
		}
	}
} ;

template <typename PFP>
class LoopNormalisationSynthesisFilter : public Filter
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;

public:
	LoopNormalisationSynthesisFilter(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		TraversorV<typename PFP::MAP> trav(m_map) ;
		for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		{
			if(m_map.isBoundaryVertex(d))
			{
				Dart db = m_map.findBoundaryFaceOfVertex(d);

				unsigned int degree = m_map.vertexDegreeOnBoundary(db) ;
				float n = 3.0/8.0 + 1.0/4.0 * cos(2.0 * M_PI / degree) ;
				n = 8.0/5.0 * (n * n) ;

				m_position[db] *= n ;
			}
		}
	}
} ;

template <typename PFP>
class LoopEvenSynthesisFilter : public Filter
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;

public:
	LoopEvenSynthesisFilter(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		TraversorV<typename PFP::MAP> trav(m_map) ;
		for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		{
			if(m_map.isBoundaryVertex(d))
			{
				Dart db = m_map.findBoundaryFaceOfVertex(d);
				typename PFP::VEC3 p = loopEvenVertex<PFP>(m_map, m_position, db) ;
				m_position[db] += p ;
			}
		}
	}
} ;

template <typename PFP>
class LoopVolumeSynthesisFilter : public Filter
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;

public:
	LoopVolumeSynthesisFilter(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		TraversorW<typename PFP::MAP> trav(m_map) ;
		for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		{
			if(!Algo::Modelisation::Tetrahedralization::isTetrahedron<PFP>(m_map,d))
			{
				typename PFP::VEC3 p = Algo::Geometry::volumeCentroid<PFP>(m_map, d, m_position);

				m_map.incCurrentLevel() ;

				Dart midV = m_map.phi_1(m_map.phi2(m_map.phi1(d)));
				m_position[midV] = p ;

				m_map.decCurrentLevel() ;
			}
		}
	}
} ;

template <typename PFP>
class SHW04VolumeNormalisationSynthesisFilter : public Filter
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;

public:
	SHW04VolumeNormalisationSynthesisFilter(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		m_map.incCurrentLevel() ;
		TraversorV<typename PFP::MAP> trav(m_map) ;
		for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		{
			if(!m_map.isBoundaryVertex(d))
			{
				typename PFP::VEC3 p = typename PFP::VEC3(0);
				unsigned int degree = 0;

				Traversor3VW<typename PFP::MAP> travVW(m_map, d);
				for(Dart dit = travVW.begin() ; dit != travVW.end() ; dit = travVW.next())
				{
					p += SHW04Vertex<PFP>(m_map, m_position, dit);
					++degree;
				}

				p /= degree;

				m_position[d] = p ;
			}
		}
		m_map.decCurrentLevel() ;
	}
} ;

} // namespace Filters

} // namespace Primal

} // namespace MR

} // namespace Algo

} // namespace CGoGN


#endif /* __3MR_FILTERS_PRIMAL__ */
