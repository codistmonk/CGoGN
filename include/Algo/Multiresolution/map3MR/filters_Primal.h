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

#ifndef __3MR_FILTERS_PRIMAL__
#define __3MR_FILTERS_PRIMAL__

#include <cmath>
#include "Algo/Geometry/centroid.h"
#include "Algo/Modelisation/tetrahedralization.h"

namespace CGoGN
{

namespace Algo
{

namespace MR
{

class Filter
{
public:
	Filter() {}
	virtual ~Filter() {}
	virtual void operator() () = 0 ;
} ;


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
 *          BSXW02 BASIC FUNCTIONS : polyhedral meshes
 *********************************************************************************/



/*********************************************************************************
 *                           ANALYSIS FILTERS
 *********************************************************************************/

/* Loop
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

/* Linear Interpolation
 *********************************************************************************/
template <typename PFP>
class LerpEdgeSynthesisFilter : public Filter
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;

public:
	LerpEdgeSynthesisFilter(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		TraversorE<typename PFP::MAP> trav(m_map) ;
		for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		{
			typename PFP::VEC3 p = (m_position[d] + m_position[m_map.phi2(d)]) * typename PFP::REAL(0.5);

			m_map.incCurrentLevel() ;

			Dart midV = m_map.phi2(d) ;
			m_position[midV] = p ;

			m_map.decCurrentLevel() ;
		}
	}
} ;

template <typename PFP>
class LerpFaceSynthesisFilter : public Filter
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;

public:
	LerpFaceSynthesisFilter(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		TraversorF<typename PFP::MAP> trav(m_map) ;
		for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		{
			typename PFP::VEC3 p = Algo::Geometry::faceCentroid<PFP>(m_map, d, m_position);

			m_map.incCurrentLevel() ;
			if(m_map.faceDegree(d) != 3)
			{
				Dart midF = m_map.phi2(m_map.phi1(d));
				m_position[midF] = p ;
			}
			m_map.decCurrentLevel() ;

		}
	}
} ;

template <typename PFP>
class LerpVolumeSynthesisFilter : public Filter
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;

public:
	LerpVolumeSynthesisFilter(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		TraversorW<typename PFP::MAP> trav(m_map) ;
		for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		{
			typename PFP::VEC3 p = Algo::Geometry::volumeCentroid<PFP>(m_map, d, m_position);

			m_map.incCurrentLevel() ;

			if(!Algo::Modelisation::Tetrahedralization::isTetrahedron<PFP>(m_map,d))
			{

				Dart midV = m_map.phi_1(m_map.phi2(m_map.phi1(d)));
				m_position[midV] = p ;
			}
			m_map.decCurrentLevel() ;

		}
	}
} ;

/* Ber02
 *********************************************************************************/

//w-lift(a)
template <typename PFP>
class Ber02OddSynthesisFilter : public Filter
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;

public:
	Ber02OddSynthesisFilter(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		float a = 0.5;

		TraversorW<typename PFP::MAP> travW(m_map) ;
		for (Dart d = travW.begin(); d != travW.end(); d = travW.next())
		{
			typename PFP::VEC3 vc = Algo::Geometry::volumeCentroid<PFP>(m_map, d, m_position);

			unsigned int count = 0;
			typename PFP::VEC3 ec(0);
			Traversor3WE<typename PFP::MAP> travWE(m_map, d);
			for (Dart dit = travWE.begin(); dit != travWE.end(); dit = travWE.next())
			{
				m_map.incCurrentLevel();
				ec += m_position[m_map.phi2(dit)];
				m_map.decCurrentLevel();
				++count;
			}
			ec /= count;

			count = 0;
			typename PFP::VEC3 fc(0);
			Traversor3WF<typename PFP::MAP> travWF(m_map, d);
			for (Dart dit = travWF.begin(); dit != travWF.end(); dit = travWF.next())
			{
				m_map.incCurrentLevel();
				fc += m_position[m_map.phi2(m_map.phi1(dit))];
				m_map.decCurrentLevel();
				++count;
			}

			fc /= count;

			m_map.incCurrentLevel() ;
			Dart midV = m_map.phi_1(m_map.phi2(m_map.phi1(d)));
			m_position[midV] += 8 * a * a * a * vc + 12 * a * a * ec + 6 * a * fc;
			m_map.decCurrentLevel() ;
		}

		TraversorF<typename PFP::MAP> travF(m_map) ;
		for (Dart d = travF.begin(); d != travF.end(); d = travF.next())
		{
			typename PFP::VEC3 vf = Algo::Geometry::faceCentroid<PFP>(m_map, d, m_position);

			typename PFP::VEC3 ef(0);
			unsigned int count = 0;
			Traversor3FE<typename PFP::MAP> travFE(m_map, d);
			for (Dart dit = travFE.begin(); dit != travFE.end(); dit = travFE.next())
			{
				m_map.incCurrentLevel();
				ef += m_position[m_map.phi2(dit)];
				m_map.decCurrentLevel();
				++count;
			}
			ef /= count;

			m_map.incCurrentLevel() ;
			Dart midF = m_map.phi2(m_map.phi1(d));
			m_position[midF] += vf * 4.0 * a * a + ef * 4.0 * a;
			m_map.decCurrentLevel() ;
		}

		TraversorE<typename PFP::MAP> travE(m_map) ;
		for (Dart d = travE.begin(); d != travE.end(); d = travE.next())
		{
			typename PFP::VEC3 ve = (m_position[d] + m_position[m_map.phi2(d)]) * typename PFP::REAL(0.5);

			m_map.incCurrentLevel() ;
			Dart midV = m_map.phi2(d) ;
			m_position[midV] += ve * a * 2.0;
			m_map.decCurrentLevel() ;
		}

	}
} ;

// s-lift(a)
template <typename PFP>
class Ber02EvenSynthesisFilter : public Filter
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;

public:
	Ber02EvenSynthesisFilter(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		float a = 0.5;

		TraversorV<typename PFP::MAP> travV(m_map);
		for(Dart d = travV.begin() ; d != travV.end() ; d = travV.next())
		{
			if(!m_map.isBoundaryVertex(d))
			{
				typename PFP::VEC3 cv(0);
				unsigned int count = 0;
				Traversor3VW<typename PFP::MAP> travVW(m_map,d);
				for(Dart dit = travVW.begin(); dit != travVW.end() ; dit = travVW.next())
				{
					m_map.incCurrentLevel() ;
					Dart midV = m_map.phi_1(m_map.phi2(m_map.phi1(dit)));
					cv += m_position[midV];
					m_map.decCurrentLevel() ;
					++count;
				}
				cv /= count;

				typename PFP::VEC3 fv(0);
				count = 0;
				Traversor3VF<typename PFP::MAP> travVF(m_map,d);
				for(Dart dit = travVF.begin(); dit != travVF.end() ; dit = travVF.next())
				{
					m_map.incCurrentLevel() ;
					Dart midV = m_map.phi2(m_map.phi1(dit));
					fv += m_position[midV];
					m_map.decCurrentLevel() ;
					++count;
				}
				fv /= count;

				typename PFP::VEC3 ev(0);
				count = 0;
				Traversor3VE<typename PFP::MAP> travVE(m_map,d);
				for(Dart dit = travVE.begin(); dit != travVE.end() ; dit = travVE.next())
				{
					m_map.incCurrentLevel() ;
					Dart midV = m_map.phi2(dit);
					ev += m_position[midV];
					m_map.decCurrentLevel() ;
					++count;
				}
				ev /= count;

				m_position[d] += cv * 8 * a * a * a + fv * 12 * a * a + ev * 6 * a;
			}
			else
			{
				Dart db = m_map.findBoundaryFaceOfVertex(d);

				typename PFP::VEC3 fv(0);
				unsigned int count = 0;
				Traversor2VF<typename PFP::MAP> travVF(m_map,db);
				for(Dart dit = travVF.begin(); dit != travVF.end() ; dit = travVF.next())
				{
					m_map.incCurrentLevel() ;
					Dart midV = m_map.phi2(m_map.phi1(dit));
					fv += m_position[midV];
					m_map.decCurrentLevel() ;
					++count;
				}
				fv /= count;

				typename PFP::VEC3 ev(0);
				count = 0;
				Traversor2VE<typename PFP::MAP> travVE(m_map,db);
				for(Dart dit = travVE.begin(); dit != travVE.end() ; dit = travVE.next())
				{
					m_map.incCurrentLevel() ;
					Dart midV = m_map.phi2(dit);
					ev += m_position[midV];
					m_map.decCurrentLevel() ;
					++count;
				}
				ev /= count;

				m_position[db] += fv * 4 * a * a + ev * 4 * a;
			}
		}

		TraversorE<typename PFP::MAP> travE(m_map);
		for(Dart d = travE.begin() ; d != travE.end() ; d = travE.next())
		{
			if(m_map.isBoundaryEdge(d))
			{
				Dart db = m_map.findBoundaryFaceOfEdge(d);

				typename PFP::VEC3 fe(0);

				m_map.incCurrentLevel() ;
				Dart midV = m_map.phi2(m_map.phi1(db));
				fe += m_position[midV];
				m_map.decCurrentLevel() ;

				m_map.incCurrentLevel() ;
				midV = m_map.phi2(m_map.phi1(m_map.phi2(db)));
				fe += m_position[midV];
				m_map.decCurrentLevel() ;

				fe /= 2;

				m_map.incCurrentLevel() ;
				Dart midF = m_map.phi2(db);
				m_position[midF] += fe * 2 * a;
				m_map.decCurrentLevel() ;
			}
			else
			{
				typename PFP::VEC3 ce(0);
				unsigned int count = 0;
				Traversor3EW<typename PFP::MAP> travEW(m_map, d);
				for(Dart dit = travEW.begin() ; dit != travEW.end() ; dit = travEW.next())
				{
					m_map.incCurrentLevel() ;
					Dart midV = m_map.phi_1(m_map.phi2(m_map.phi1(dit)));
					ce += m_position[midV];
					m_map.decCurrentLevel() ;
					++count;
				}

				ce /= count;

				typename PFP::VEC3 fe(0);
				count = 0;
				Traversor3FW<typename PFP::MAP> travFW(m_map, d);
				for(Dart dit = travFW.begin() ; dit != travFW.end() ; dit = travFW.next())
				{
					m_map.incCurrentLevel() ;
					Dart midV = m_map.phi2(m_map.phi1(dit));
					fe += m_position[midV];
					m_map.decCurrentLevel() ;
					++count;
				}

				fe /= count;

				m_map.incCurrentLevel() ;
				Dart midF = m_map.phi2(d);
				m_position[midF] += ce * 4 * a * a + fe * 4 * a;
				m_map.decCurrentLevel() ;
			}
		}

		TraversorF<typename PFP::MAP> travF(m_map) ;
		for (Dart d = travF.begin(); d != travF.end(); d = travF.next())
		{
			typename PFP::VEC3 cf(0);

			m_map.incCurrentLevel();
			Dart midV = m_map.phi_1(m_map.phi2(m_map.phi1(d)));
			cf += m_position[midV];
			m_map.decCurrentLevel();

			if(!m_map.isBoundaryFace(d))
			{
				Dart d3 = m_map.phi3(d);
				m_map.incCurrentLevel();
				Dart midV = m_map.phi_1(m_map.phi2(m_map.phi1(d3)));
				cf += m_position[midV];
				m_map.decCurrentLevel();

				cf /= 2;
			}

			m_map.incCurrentLevel() ;
			Dart midF = m_map.phi2(m_map.phi1(d));
			m_position[midF] += cf * 2 * a;
			m_map.decCurrentLevel() ;
		}

	}
} ;

// s-scale(a)
template <typename PFP>
class Ber02ScaleSynthesisFilter : public Filter
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;

public:
	Ber02ScaleSynthesisFilter(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		float a = 0.5;

		TraversorV<typename PFP::MAP> travV(m_map) ;
		for (Dart d = travV.begin(); d != travV.end(); d = travV.next())
		{
			if(m_map.isBoundaryVertex(d))
			{
				Dart db = m_map.findBoundaryFaceOfVertex(d);
				m_position[db] *= a * a;
			}
			else
			{
				m_position[d] *= a * a * a;
			}
		}

		TraversorE<typename PFP::MAP> travE(m_map) ;
		for (Dart d = travE.begin(); d != travE.end(); d = travE.next())
		{
			if(m_map.isBoundaryEdge(d))
			{
				Dart db = m_map.findBoundaryFaceOfEdge(d);

				m_map.incCurrentLevel() ;
				Dart midE = m_map.phi2(db);
				m_position[midE] *= a ;
				m_map.decCurrentLevel() ;
			}
			else
			{
				m_map.incCurrentLevel() ;
				Dart midE = m_map.phi2(d);
				m_position[midE] *= a * a;
				m_map.decCurrentLevel() ;
			}
		}

		TraversorF<typename PFP::MAP> travF(m_map) ;
		for (Dart d = travF.begin(); d != travF.end(); d = travF.next())
		{
			if(!m_map.isBoundaryFace(d))
			{
				m_map.incCurrentLevel() ;
				Dart midF = m_map.phi2(m_map.phi1(d));
				m_position[midF] *= a ;
				m_map.decCurrentLevel() ;
			}
		}
	}
} ;

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

/* Catmull-clark on Boundary Vertices and MJ96 on Insides Vertices
 *********************************************************************************/
template <typename PFP>
class MJ96VertexSubdivision : public Filter
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;

public:
	MJ96VertexSubdivision(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		TraversorV<typename PFP::MAP> trav(m_map) ;
		for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		{
			if(m_map.isBoundaryVertex(d))
			{
				Dart db = m_map.findBoundaryFaceOfVertex(d);

				typename PFP::VEC3 np1(0) ;
				typename PFP::VEC3 np2(0) ;
				unsigned int degree1 = 0 ;
				unsigned int degree2 = 0 ;
				Dart it = db ;
				do
				{
					++degree1 ;
					Dart dd = m_map.phi1(it) ;
					np1 += m_position[dd] ;
					Dart end = m_map.phi_1(it) ;
					dd = m_map.phi1(dd) ;
					do
					{
						++degree2 ;
						np2 += m_position[dd] ;
						dd = m_map.phi1(dd) ;
					} while(dd != end) ;
					it = m_map.phi2(m_map.phi_1(it)) ;
				} while(it != db) ;

				float beta = 3.0 / (2.0 * degree1) ;
				float gamma = 1.0 / (4.0 * degree2) ;
				np1 *= beta / degree1 ;
				np2 *= gamma / degree2 ;

				typename PFP::VEC3 vp = m_position[db] ;
				vp *= 1.0 - beta - gamma ;

				m_map.incCurrentLevel() ;

				m_position[d] = np1 + np2 + vp ;

				m_map.decCurrentLevel() ;

			}
			else
			{
				typename PFP::VEC3 P = m_position[d];

				//vertex points
				typename PFP::VEC3 Cavg = typename PFP::VEC3(0);
				unsigned int degree = 0;
				Traversor3VW<typename PFP::MAP> travVW(m_map, d);
				for(Dart dit = travVW.begin() ; dit != travVW.end() ; dit = travVW.next())
				{
					Cavg += Algo::Geometry::volumeCentroid<PFP>(m_map, dit, m_position);
					++degree;
				}
				Cavg /= degree;

				typename PFP::VEC3 Aavg = typename PFP::VEC3(0);
				degree = 0;
				Traversor3VF<typename PFP::MAP> travVF(m_map, d);
				for(Dart dit = travVF.begin() ; dit != travVF.end() ; dit = travVF.next())
				{
					Aavg += Algo::Geometry::faceCentroid<PFP>(m_map, dit, m_position);
					++degree;
				}
				Aavg /= degree;

				typename PFP::VEC3 Mavg = typename PFP::VEC3(0);
				degree = 0;
				Traversor3VE<typename PFP::MAP> travVE(m_map, d);
				for(Dart dit = travVE.begin() ; dit != travVE.end() ; dit = travVE.next())
				{
					Dart d2 = m_map.phi2(dit);
					Aavg += (m_position[dit] + m_position[d2]) * typename PFP::REAL(0.5);
					++degree;
				}
				Aavg /= degree;

				typename PFP::VEC3 vp = Cavg + Aavg * 3 + Mavg * 3 + P;
				vp /= 8;

				m_map.incCurrentLevel() ;

				m_position[d] = P;//vp;

				m_map.decCurrentLevel() ;
			}
		}
	}
};

template <typename PFP>
class MJ96EdgeSubdivision : public Filter
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;

public:
	MJ96EdgeSubdivision(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		TraversorE<typename PFP::MAP> trav(m_map) ;
		for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		{
			if(m_map.isBoundaryEdge(d))
			{
				Dart db = m_map.findBoundaryFaceOfEdge(d);

				Dart d1 = m_map.phi2(db) ;
				Dart d2 = m_map.phi2(d1) ;
				Dart d3 = m_map.phi_1(d1) ;
				Dart d4 = m_map.phi_1(d2) ;
				Dart d5 = m_map.phi1(m_map.phi1(d1)) ;
				Dart d6 = m_map.phi1(m_map.phi1(d2)) ;

				typename PFP::VEC3 p1 = m_position[d1] ;
				typename PFP::VEC3 p2 = m_position[d2] ;
				typename PFP::VEC3 p3 = m_position[d3] ;
				typename PFP::VEC3 p4 = m_position[d4] ;
				typename PFP::VEC3 p5 = m_position[d5] ;
				typename PFP::VEC3 p6 = m_position[d6] ;

				p1 *= 3.0 / 8.0 ;
				p2 *= 3.0 / 8.0 ;
				p3 *= 1.0 / 16.0 ;
				p4 *= 1.0 / 16.0 ;
				p5 *= 1.0 / 16.0 ;
				p6 *= 1.0 / 16.0 ;

				m_map.incCurrentLevel() ;

				Dart midV = m_map.phi2(d);

				m_position[midV] = p1 + p2 + p3 + p4 + p5 + p6 ;

				m_map.decCurrentLevel() ;
			}
			else
			{
				//edge points
				typename PFP::VEC3 Cavg = typename PFP::VEC3(0);
				unsigned int degree = 0;
				Traversor3EW<typename PFP::MAP> travEW(m_map, d);
				for(Dart dit = travEW.begin() ; dit != travEW.end() ; dit = travEW.next())
				{
					Cavg += Algo::Geometry::volumeCentroid<PFP>(m_map, dit, m_position);
					++degree;
				}
				Cavg /= degree;

				typename PFP::VEC3 Aavg = typename PFP::VEC3(0);
				degree = 0;
				Traversor3EF<typename PFP::MAP> travEF(m_map, d);
				for(Dart dit = travEF.begin() ; dit != travEF.end() ; dit = travEF.next())
				{
					Aavg += Algo::Geometry::faceCentroid<PFP>(m_map, dit, m_position);
					++degree;
				}
				Aavg /= degree;

				Dart d2 = m_map.phi2(d);
				typename PFP::VEC3 M = (m_position[d] + m_position[d2]) * typename PFP::REAL(0.5);

				typename PFP::VEC3 ep = Cavg + Aavg * 2 + M * (degree - 3);
				ep /= degree;

				m_map.incCurrentLevel() ;

				Dart midV = m_map.phi2(d);

				m_position[midV] = ep;

				m_map.decCurrentLevel() ;
			}
		}
	}
};

template <typename PFP>
class MJ96FaceSubdivision : public Filter
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;

public:
	MJ96FaceSubdivision(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		TraversorF<typename PFP::MAP> trav(m_map) ;
		for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		{
			if(m_map.isBoundaryFace(d))
			{
				Dart db = m_map.phi3(d);

				typename PFP::VEC3 p(0) ;
				unsigned int degree = 0 ;
				Traversor2FV<typename PFP::MAP> trav(m_map, db) ;
				for(Dart it = trav.begin(); it != trav.end(); it = trav.next())
				{
					++degree ;
					p += m_position[it] ;
				}
				p /= degree ;

				m_map.incCurrentLevel() ;

				Dart df = m_map.phi1(m_map.phi1(d)) ;

				m_position[df] = p ;

				m_map.decCurrentLevel() ;
			}
			else
			{
				//face points
				typename PFP::VEC3 C0 = Algo::Geometry::volumeCentroid<PFP>(m_map, d, m_position);
				typename PFP::VEC3 C1 = Algo::Geometry::volumeCentroid<PFP>(m_map, m_map.phi3(d), m_position);

				typename PFP::VEC3 A = Algo::Geometry::faceCentroid<PFP>(m_map, m_map.phi3(d), m_position);

				typename PFP::VEC3 fp = C0 + A * 2 + C1;
				fp /= 4;

				m_map.incCurrentLevel() ;

				Dart df = m_map.phi1(m_map.phi1(d)) ;
				m_position[df] = fp;

				m_map.decCurrentLevel() ;
			}
		}
	}
};

template <typename PFP>
class MJ96VolumeSubdivision : public Filter
{
protected:
	typename PFP::MAP& m_map ;
	VertexAttribute<typename PFP::VEC3>& m_position ;

public:
	MJ96VolumeSubdivision(typename PFP::MAP& m, VertexAttribute<typename PFP::VEC3>& p) : m_map(m), m_position(p)
	{}

	void operator() ()
	{
		TraversorW<typename PFP::MAP> trav(m_map) ;
		for (Dart d = trav.begin(); d != trav.end(); d = trav.next())
		{
			//cell points : these points are the average of the
			//vertices of the lattice that bound the cell
			typename PFP::VEC3 p = Algo::Geometry::volumeCentroid<PFP>(m_map, d, m_position);

			m_map.incCurrentLevel() ;

			if(!Algo::Modelisation::Tetrahedralization::isTetrahedron<PFP>(m_map,d))
			{
				Dart midV = m_map.phi_1(m_map.phi2(m_map.phi1(d)));
				m_position[midV] = p ;
			}

			m_map.decCurrentLevel() ;

		}
	}
};



} // namespace MR

} // namespace Algo

} // namespace CGoGN


#endif /* __3MR_FILTERS_PRIMAL__ */
