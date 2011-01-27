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

#ifndef __GEOMETRY_APPROXIMATOR_H__
#define __GEOMETRY_APPROXIMATOR_H__

#include "Algo/Decimation/approximator.h"

namespace CGoGN
{

namespace Algo
{

namespace Decimation
{

template <typename PFP>
class Approximator_QEM : public Approximator<PFP, typename PFP::VEC3>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

protected:
	AttributeHandler<Quadric<REAL> > m_quadric ;

public:
	Approximator_QEM(MAP& m, AttributeHandler<VEC3>& pos, Predictor<PFP, VEC3>* pred = NULL) :
		Approximator<PFP, VEC3>(m, pos, EDGE_ORBIT, pred)
	{}
	~Approximator_QEM()
	{}
	ApproximatorType getType() const { return A_QEM ; }
	bool init() ;
	void approximate(Dart d) ;
} ;

template <typename PFP>
class Approximator_MidEdge : public Approximator<PFP, typename PFP::VEC3>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

	Approximator_MidEdge(MAP& m, AttributeHandler<VEC3>& pos, Predictor<PFP, VEC3>* pred = NULL) :
		Approximator<PFP, VEC3>(m, pos, EDGE_ORBIT, pred)
	{}
	~Approximator_MidEdge()
	{}
	ApproximatorType getType() const { return A_MidEdge ; }
	bool init() ;
	void approximate(Dart d) ;
} ;

template <typename PFP>
class Approximator_HalfCollapse : public Approximator<PFP, typename PFP::VEC3>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

	Approximator_HalfCollapse(MAP& m, AttributeHandler<VEC3>& pos, Predictor<PFP, VEC3>* pred = NULL) :
		Approximator<PFP, VEC3>(m, pos, EDGE_ORBIT, pred)
	{}
	~Approximator_HalfCollapse()
	{}
	ApproximatorType getType() const { return A_HalfCollapse ; }
	bool init() ;
	void approximate(Dart d) ;
} ;

template <typename PFP>
class Approximator_CornerCutting : public Approximator<PFP, typename PFP::VEC3>
{
public:
	typedef typename PFP::MAP MAP ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

	Approximator_CornerCutting(MAP& m, AttributeHandler<VEC3>& pos, Predictor<PFP, VEC3>* pred = NULL) :
		Approximator<PFP, VEC3>(m, pos, EDGE_ORBIT, pred)
	{}
	~Approximator_CornerCutting()
	{}
	ApproximatorType getType() const { return A_CornerCutting ; }
	bool init() ;
	void approximate(Dart d) ;
} ;

} //namespace Decimation

} //namespace Algo

} //namespace CGoGN

#include "Algo/Decimation/geometryApproximator.hpp"

#endif
