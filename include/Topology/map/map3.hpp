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

namespace CGoGN
{

inline void Map3::init()
{
	m_phi3 = addRelation("phi3") ;
}

inline Map3::Map3() : Map2()
{
	init() ;
}

inline std::string Map3::mapTypeName()
{
	return "Map3";
}

inline unsigned int Map3::dimension()
{
	return 3;
}

inline void Map3::clear(bool removeAttrib)
{
	Map2::clear(removeAttrib) ;
	if (removeAttrib)
		init() ;
}

/*! @name Basic Topological Operators
 * Access and Modification
 *************************************************************************/

inline Dart Map3::newDart()
{
	Dart d = Map2::newDart() ;
	(*m_phi3)[d.index] = d ;
	return d ;
}

inline Dart Map3::phi3(Dart d)
{
	return (*m_phi3)[d.index] ;
}

template <int N>
inline Dart Map3::phi(Dart d)
{
	assert( (N >0) || !"negative parameters not allowed in template multi-phi");
	if (N<10)
	{
		switch(N)
		{
		case 1 : return phi1(d) ;
		case 2 : return phi2(d) ;
		case 3 : return phi3(d) ;
		default : assert(!"Wrong multi-phi relation value") ; return d ;
		}
	}
	switch(N%10)
	{
	case 1 : return phi1(phi<N/10>(d)) ;
	case 2 : return phi2(phi<N/10>(d)) ;
	case 3 : return phi3(phi<N/10>(d)) ;
	default : assert(!"Wrong multi-phi relation value") ; return d ;
	}
}

inline Dart Map3::alpha0(Dart d)
{
	return phi3(d) ;
}

inline Dart Map3::alpha1(Dart d)
{
	return phi3(phi_1(d)) ;
}

inline Dart Map3::alpha2(Dart d)
{
	return phi3(phi2(d));
}

inline Dart Map3::alpha_2(Dart d)
{
	return phi2(phi3(d));
}

inline void Map3::phi3sew(Dart d, Dart e)
{
	assert((*m_phi3)[d.index] == d) ;
	assert((*m_phi3)[e.index] == e) ;
	(*m_phi3)[d.index] = e ;
	(*m_phi3)[e.index] = d ;
}

inline void Map3::phi3unsew(Dart d)
{
	Dart e = (*m_phi3)[d.index] ;
	(*m_phi3)[d.index] = d ;
	(*m_phi3)[e.index] = e ;
}

/*! @name Topological Queries
 *  Return or set various topological information
 *************************************************************************/

inline bool Map3::sameEdge(Dart d, Dart e)
{
	return sameOrientedEdge(d, e) || sameOrientedEdge(phi2(d), e) ;
}

inline bool Map3::sameFace(Dart d, Dart e)
{
	return Map2::sameOrientedFace(d, e) || Map2::sameOrientedFace(phi3(d), e) ;
}

inline bool Map3::isBoundaryFace(Dart d)
{
	return isBoundaryMarked(d) || isBoundaryMarked(phi3(d));
}

/*! @name Cell Functors
 *  Apply functors to all darts of a cell
 *************************************************************************/

inline bool Map3::foreach_dart_of_face(Dart d, FunctorType& f, unsigned int thread)
{
	return Map2::foreach_dart_of_oriented_face(d, f, thread) || Map2::foreach_dart_of_oriented_face(phi3(d), f, thread);
}

} // namespace CGoGN
