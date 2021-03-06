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

namespace CGoGN
{

/*******************************************************************************
					VERTEX CENTERED TRAVERSALS
*******************************************************************************/

// Traversor2VE

template <typename MAP>
Traversor2VE<MAP>::Traversor2VE(MAP& map, Dart dart) : m(map), start(dart)
{}

template <typename MAP>
Dart Traversor2VE<MAP>::begin()
{
	current = start ;
	return current ;
}

template <typename MAP>
Dart Traversor2VE<MAP>::end()
{
	return NIL ;
}

template <typename MAP>
Dart Traversor2VE<MAP>::next()
{
	if(current != NIL)
	{
//		current = m.alpha1(current) ;
		current = m.phi2(m.phi_1(current)) ;
		if(current == start)
			current = NIL ;
	}
	return current ;
}

// Traversor2VF

template <typename MAP>
Traversor2VF<MAP>::Traversor2VF(MAP& map, Dart dart) : m(map), start(dart)
{
	if(m.isBoundaryMarked(start)) // jump over a boundary face
		start = m.phi2(m.phi_1(start)) ;
}

template <typename MAP>
Dart Traversor2VF<MAP>::begin()
{
	current = start ;
	return current ;
}

template <typename MAP>
Dart Traversor2VF<MAP>::end()
{
	return NIL ;
}

template <typename MAP>
Dart Traversor2VF<MAP>::next()
{
	if(current != NIL)
	{
		current = m.phi2(m.phi_1(current)) ;
		if(m.isBoundaryMarked(current)) // jump over a boundary face
			current = m.phi2(m.phi_1(current)) ;
		if(current == start)
			current = NIL ;
	}
	return current ;
}

// Traversor2VVaE

template <typename MAP>
Traversor2VVaE<MAP>::Traversor2VVaE(MAP& map, Dart dart) : m(map)
{
	start = m.phi2(dart) ;
}

template <typename MAP>
Dart Traversor2VVaE<MAP>::begin()
{
	current = start ;
	return current ;
}

template <typename MAP>
Dart Traversor2VVaE<MAP>::end()
{
	return NIL ;
}

template <typename MAP>
Dart Traversor2VVaE<MAP>::next()
{
	if(current != NIL)
	{
		current = m.phi_1(m.phi2(current)) ;
		if(current == start)
			current = NIL ;
	}
	return current ;
}

// Traversor2VVaF

template <typename MAP>
Traversor2VVaF<MAP>::Traversor2VVaF(MAP& map, Dart dart) : m(map)
{
	if(m.isBoundaryMarked(dart))
		dart = m.phi2(m.phi_1(dart)) ;
	start = m.phi1(m.phi1(dart)) ;
	if(start == dart)
		start = m.phi1(dart) ;
	stop = dart ;
}

template <typename MAP>
Dart Traversor2VVaF<MAP>::begin()
{
	current = start ;
	return current ;
}

template <typename MAP>
Dart Traversor2VVaF<MAP>::end()
{
	return NIL ;
}

template <typename MAP>
Dart Traversor2VVaF<MAP>::next()
{
	if(current != NIL)
	{
		current = m.phi1(current) ;
		if(current == stop)
		{
			Dart d = m.phi2(m.phi_1(current)) ;
			if(m.isBoundaryMarked(d)) // jump over a boundary face
			{
				d = m.phi2(m.phi_1(d)) ;
				current = m.phi1(d);
			}
			else
				current = m.phi1(m.phi1(d)) ;
			if(current == d)
				current = m.phi1(d) ;
			stop = d ;
		}
		if(current == start)
			current = NIL ;
	}
	return current ;
}

/*******************************************************************************
					EDGE CENTERED TRAVERSALS
*******************************************************************************/

// Traversor2EV

template <typename MAP>
Traversor2EV<MAP>::Traversor2EV(MAP& map, Dart dart) : m(map), start(dart)
{}

template <typename MAP>
Dart Traversor2EV<MAP>::begin()
{
	current = start ;
	return current ;
}

template <typename MAP>
Dart Traversor2EV<MAP>::end()
{
	return NIL ;
}

template <typename MAP>
Dart Traversor2EV<MAP>::next()
{
	if(current != NIL)
	{
		current = m.phi2(current) ;
		if(current == start)
			current = NIL ;
	}
	return current ;
}

// Traversor2EF

template <typename MAP>
Traversor2EF<MAP>::Traversor2EF(MAP& map, Dart dart) : m(map), start(dart)
{
	if(m.isBoundaryMarked(start))
		start = m.phi2(current) ;
}

template <typename MAP>
Dart Traversor2EF<MAP>::begin()
{
	current = start ;
	return current ;
}

template <typename MAP>
Dart Traversor2EF<MAP>::end()
{
	return NIL ;
}

template <typename MAP>
Dart Traversor2EF<MAP>::next()
{
	if(current != NIL)
	{
		current = m.phi2(current) ;
		if(current == start || m.isBoundaryMarked(current)) // do not consider a boundary face
			current = NIL ;
	}
	return current ;
}

// Traversor2EEaV

template <typename MAP>
Traversor2EEaV<MAP>::Traversor2EEaV(MAP& map, Dart dart) : m(map)
{
	start = m.phi2(m.phi_1(dart)) ;
	stop1 = dart ;
	stop2 = m.phi2(dart) ;
}

template <typename MAP>
Dart Traversor2EEaV<MAP>::begin()
{
	current = start ;
	return current ;
}

template <typename MAP>
Dart Traversor2EEaV<MAP>::end()
{
	return NIL ;
}

template <typename MAP>
Dart Traversor2EEaV<MAP>::next()
{
	if(current != NIL)
	{
		current = m.phi2(m.phi_1(current)) ;
		if(current == stop1)
			current = m.phi2(m.phi_1(stop2)) ;
		else if(current == stop2)
			current = NIL ;
	}
	return current ;
}

// Traversor2EEaF

template <typename MAP>
Traversor2EEaF<MAP>::Traversor2EEaF(MAP& map, Dart dart) : m(map)
{
	start = m.phi1(dart) ;
	stop1 = dart ;
	stop2 = m.phi2(dart) ;
}

template <typename MAP>
Dart Traversor2EEaF<MAP>::begin()
{
	current = start ;
	return current ;
}

template <typename MAP>
Dart Traversor2EEaF<MAP>::end()
{
	return NIL ;
}

template <typename MAP>
Dart Traversor2EEaF<MAP>::next()
{
	if(current != NIL)
	{
		current = m.phi1(current) ;
		if(current == stop1)
			current = m.phi1(stop2) ;
		else if(current == stop2)
			current = NIL ;
	}
	return current ;
}

/*******************************************************************************
					FACE CENTERED TRAVERSALS
*******************************************************************************/

// Traversor2FV

template <typename MAP>
Traversor2FV<MAP>::Traversor2FV(MAP& map, Dart dart) : m(map), start(dart)
{}

template <typename MAP>
Dart Traversor2FV<MAP>::begin()
{
	current = start ;
	return current ;
}

template <typename MAP>
Dart Traversor2FV<MAP>::end()
{
	return NIL ;
}

template <typename MAP>
Dart Traversor2FV<MAP>::next()
{
	if(current != NIL)
	{
		current = m.phi1(current) ;
		if(current == start)
			current = NIL ;
	}
	return current ;
}

// Traversor2FFaV

template <typename MAP>
Traversor2FFaV<MAP>::Traversor2FFaV(MAP& map, Dart dart) : m(map)
{
	start = m.phi2(m.phi_1(m.phi2(m.phi_1(dart)))) ;
	current = start ;
	if(start == dart)
	{
		stop = m.phi2(m.phi_1(dart)) ;
		start = next() ;
	}
	stop = dart ;
	if(m.isBoundaryMarked(start))
		start = next() ;
}

template <typename MAP>
Dart Traversor2FFaV<MAP>::begin()
{
	current = start ;
	return current ;
}

template <typename MAP>
Dart Traversor2FFaV<MAP>::end()
{
	return NIL ;
}

template <typename MAP>
Dart Traversor2FFaV<MAP>::next()
{
	if(current != NIL)
	{
		current = m.phi2(m.phi_1(current)) ;
		if(current == stop)
		{
			Dart d = m.phi1(current) ;
			current = m.phi2(m.phi_1(m.phi2(m.phi_1(d)))) ;
			if(current == d)
			{
				stop = m.phi2(m.phi_1(d)) ;
				return next() ;
			}
			stop = d ;
			if(m.isBoundaryMarked(current))
				return next() ;
		}
		if(current == start)
			current = NIL ;
	}
	return current ;
}

// Traversor2FFaE

template <typename MAP>
Traversor2FFaE<MAP>::Traversor2FFaE(MAP& map, Dart dart) : m(map)
{
	start = m.phi2(dart) ;
	while(start != NIL && m.isBoundaryMarked(start))
	{
		start = m.phi2(m.phi1(m.phi2(start))) ;
		if(start == m.phi2(dart))
			start = NIL ;
	}
}

template <typename MAP>
Dart Traversor2FFaE<MAP>::begin()
{
	current = start ;
	return current ;
}

template <typename MAP>
Dart Traversor2FFaE<MAP>::end()
{
	return NIL ;
}

template <typename MAP>
Dart Traversor2FFaE<MAP>::next()
{
	if(current != NIL)
	{
		do
		{
			current = m.phi2(m.phi1(m.phi2(current))) ;
		} while(m.isBoundaryMarked(current)) ;
		if(current == start)
			current = NIL ;
	}
	return current ;
}


//
//template<typename MAP>
//Traversor2<MAP>* Traversor2<MAP>::createIncident(MAP& map, Dart dart, unsigned int orbX, unsigned int orbY)
//{
//	int code = 0x100*(orbX-VERTEX) + orbY-VERTEX;
//
//	switch(code)
//	{
//	case 0x0001:
//		return new Traversor2VE<MAP>(map,dart);
//		break;
//	case 0x0002:
//		return new Traversor2VF<MAP>(map,dart);
//		break;
//	case 0x0100:
//		return new Traversor2EV<MAP>(map,dart);
//		break;
//	case 0x0102:
//		return new Traversor2EF<MAP>(map,dart);
//		break;
//	case 0x0200:
//		return new Traversor2FV<MAP>(map,dart);
//		break;
//	case 0x0201:
//		return new Traversor2FE<MAP>(map,dart);
//		break;
//	default:
//		return NULL;
//		break;
//	}
//	return NULL;
//}
//
//template<typename MAP>
//Traversor2<MAP>* Traversor2<MAP>::createAdjacent(MAP& map, Dart dart, unsigned int orbX, unsigned int orbY)
//{
//	int code = 0x100*(orbX-VERTEX) + orbY-VERTEX;
//	switch(code)
//	{
//	case 0x0001:
//		return new Traversor2VVaE<MAP>(map,dart);
//		break;
//	case 0x0002:
//		return new Traversor2VVaF<MAP>(map,dart);
//		break;
//	case 0x0100:
//		return new Traversor2EEaV<MAP>(map,dart);
//		break;
//	case 0x0102:
//		return new Traversor2EEaF<MAP>(map,dart);
//		break;
//	case 0x0200:
//		return new Traversor2FFaV<MAP>(map,dart);
//		break;
//	case 0x0201:
//		return new Traversor2FFaE<MAP>(map,dart);
//		break;
//	default:
//		return NULL;
//		break;
//	}
//	return NULL;
//}

} // namespace CGoGN
