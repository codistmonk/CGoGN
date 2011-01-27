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

#include "Topology/map/map2.h"

namespace CGoGN
{

/*! @name Generator and Deletor
 *  To generate or delete faces in a 2-map
 *************************************************************************/

void Map2::deleteOrientedFace(Dart d)
{
	Dart e = d ;
	do
	{
		phi2unsew(e) ;		// unsew the face of d
		e = phi1(e) ;		// from all its adjacent faces
	} while (e != d) ;
	Map1::deleteOrientedFace(d);	// delete the face
}

/*! @name Topological Operators
 *  Topological operations on 2-maps
 *************************************************************************/

void Map2::splitVertex(Dart d, Dart e)
{
	assert(sameOrientedVertex(d, e));
	Dart dd = phi2(d) ;
	Dart ee = phi2(e) ;
	Map1::cutEdge(dd);			// Cut the edge of dd (make a new half edge)
	Map1::cutEdge(ee);			// Cut the edge of ee (make a new half edge)
	phi2sew(phi1(dd), phi1(ee));// Sew the two faces along the new edge
}

void Map2::cutEdge(Dart d)
{
	Map1::cutEdge(d);		// Cut the edge of d
	Dart nd = phi1(d);
	Dart e = phi2(d);
	if (e != d)				// Test if an opposite edge exists
	{
		Map1::cutEdge(e);	// Cut the opposite edge
		Dart ne = phi1(e);
		phi2unsew(d);		// Correct the phi2 links
		phi2sew(d, ne);
		phi2sew(e, nd);
	}
}

void Map2::collapseEdge(Dart d, bool delDegenerateFaces)
{
	Dart f;							// A dart in the face to check
	Dart e = phi2(d);				// Test if an opposite edge exists
	if (e != d)
	{
		f = phi1(e);				// A dart in the face of e
		phi2unsew(d);				// Unlink the opposite edges
		if (f != e && delDegenerateFaces)
		{
			Map1::collapseEdge(e);		// Collapse edge e
			collapseDegeneratedFace(f);	// and collapse its face if degenerated
		}
		else
			Map1::collapseEdge(e);	// Just collapse edge e
	}
	f = phi1(d);					// A dart in the face of d
	if (f != d && delDegenerateFaces)
	{
		Map1::collapseEdge(d);		// Collapse edge d
		collapseDegeneratedFace(f);	// and collapse its face if degenerated
	}
	else
		Map1::collapseEdge(d);	// Just collapse edge d
}

bool Map2::flipEdge(Dart d)
{
	Dart e = phi2(d);		// Test if an opposite
	if (e != d)				// edge exists
	{
		Dart dNext = phi1(d);
		Dart eNext = phi1(e);
		Dart dPrev = phi_1(d);
		Dart ePrev = phi_1(e);
		phi1sew(d, ePrev);		// Detach the two
		phi1sew(e, dPrev);		// vertices of the edge
		phi1sew(d, dNext);		// Insert the edge in its
		phi1sew(e, eNext);		// new vertices after flip
		return true ;
	}
	return false ; // cannot flip a border edge
}

bool Map2::flipBackEdge(Dart d)
{
	Dart e = phi2(d);		// Test if an opposite
	if (e != d)				// edge exists
	{
		Dart dNext = phi1(d);
		Dart eNext = phi1(e);
		Dart dPrev = phi_1(d);
		Dart ePrev = phi_1(e);
		phi1sew(d, ePrev);			// Detach the two
		phi1sew(e, dPrev);			// vertices of the edge
		phi1sew(e, phi_1(dPrev));	// Insert the edge in its
		phi1sew(d, phi_1(ePrev));	// new vertices after flip
		return true ;
	}
	return false ; // cannot flip a border edge
}

void Map2::sewFaces(Dart d, Dart e)
{
	phi2sew(d, e);
}

void Map2::unsewFaces(Dart d)
{
	phi2unsew(d);
}

bool Map2::collapseDegeneratedFace(Dart d)
{
	Dart e = phi1(d);				// Check if the face is a loop
	if (phi1(e) == d)				// Yes: it contains one or two edge(s)
	{
		Dart d2 = phi2(d);			// Check opposite edges
		Dart e2 = phi2(e);
		if (d2 != d) phi2unsew(d);	// Update phi2-links if needed
		if (e2 != e) phi2unsew(e);
		if (d2 != d && e2 != e)
			phi2sew(d2, e2);
		Map1::deleteOrientedFace(d);// Delete the single edge or two edges of the loop
		return true ;
	}
	return false ;
}

void Map2::splitFace(Dart d, Dart e)
{
	Map1::splitFace(d, e);			// Split the face
	phi2sew(phi_1(d), phi_1(e));	// Sew the two resulting faces along the new edge
}

bool Map2::mergeFaces(Dart d)
{
	Dart e = phi2(d) ;
	if(e != d)
	{
		phi2unsew(d);			// unsew the face of d
		Map1::mergeFaces(d, e); // merge the two faces along edges of d and e
		return true ;
	}
	return false ;
}

void Map2::extractTrianglePair(Dart d)
{
	assert(isFaceTriangle(d)) ;
	Dart e = phi2(d) ;
	if(e != d)
	{
		assert(isFaceTriangle(e)) ;
		Dart e1 = phi2(phi1(e)) ;
		Dart e2 = phi2(phi_1(e)) ;
		phi2unsew(e1) ;
		phi2unsew(e2) ;
		phi2sew(e1, e2) ;
	}
	Dart d1 = phi2(phi1(d)) ;
	Dart d2 = phi2(phi_1(d)) ;
	phi2unsew(d1) ;
	phi2unsew(d2) ;
	phi2sew(d1, d2) ;
}

void Map2::insertTrianglePair(Dart d, Dart v1, Dart v2)
{
	assert(sameOrientedVertex(v1, v2)) ;
	assert((v1 != v2 && phi2(d) != d) || (v1 == v2 && phi2(d) == d)) ;
	assert(isFaceTriangle(d) && phi2(phi1(d)) == phi1(d) && phi2(phi_1(d)) == phi_1(d)) ;
	Dart e = phi2(d) ;
	if(e != d && v1 != v2)
	{
		assert(isFaceTriangle(e) && phi2(phi1(e)) == phi1(e) && phi2(phi_1(e)) == phi_1(e)) ;
		Dart vv2 = phi2(v2) ;
		phi2unsew(v2) ;
		phi2sew(phi_1(e), v2) ;
		phi2sew(phi1(e), vv2) ;
	}
	Dart vv1 = phi2(v1) ;
	phi2unsew(v1) ;
	phi2sew(phi_1(d), v1) ;
	phi2sew(phi1(d), vv1) ;
}

bool Map2::mergeVolumes(Dart d, Dart e)
{
	// First traversal of both faces to check the face sizes
	// and stored their edges to efficiently access them further
	std::vector<Dart> dDarts;
	std::vector<Dart> eDarts;
	dDarts.reserve(16);		// usual faces have less than 16 edges
	eDarts.reserve(16);

	Dart dFit = d ;
	Dart eFit = phi1(e) ;	// must take phi1 because of the use
	do						// of reverse iterator for sewing loop
	{
		dDarts.push_back(dFit) ;
		dFit = phi1(dFit) ;
	} while(dFit != d) ;
	do
	{
		eDarts.push_back(eFit) ;
		eFit = phi1(eFit) ;
	} while(eFit != phi1(e)) ;

	if(dDarts.size() != eDarts.size())
		return false ;

	// Make the sewing: take darts in initial order (clockwise) in first face
	// and take darts in reverse order (counter-clockwise) in the second face
	std::vector<Dart>::iterator dIt;
	std::vector<Dart>::reverse_iterator eIt;
	for (dIt = dDarts.begin(), eIt = eDarts.rbegin(); dIt != dDarts.end(); ++dIt, ++eIt)
	{
		Dart d2 = phi2(*dIt);			// Search the faces adjacent to dNext and eNext
		Dart e2 = phi2(*eIt);
		if (d2 != *dIt) phi2unsew(d2);	// Unlink the two adjacent faces from dNext and eNext
		if (e2 != *eIt) phi2unsew(e2);
		if (d2 != *dIt && e2 != *eIt) phi2sew(d2,e2); // Link the two adjacent faces together if they exists
	}
	Map1::deleteOrientedFace(d);	// Delete the two alone faces
	Map1::deleteOrientedFace(e);

	return true ;
}

unsigned int Map2::closeHole(Dart d)
{
	assert(phi2(d) == d);		// Nothing to close

	Dart first = newDart();		// First edge of the face that will fill the hole
	unsigned int countEdges = 1;

	phi2sew(d, first);	// phi2-link the new edge to the hole

	Dart dNext = d;	// Turn around the hole
	Dart dPhi1;		// to complete the face
	do
	{
		do
		{
			dPhi1 = phi1(dNext);	// Search and put in dNext
			dNext = phi2(dPhi1);	// the next dart of the hole
		} while (dNext != dPhi1 && dPhi1 != d);

		if (dPhi1 != d)
		{
			Dart next = newDart();	// Add a new edge there and link it to the face
			++countEdges;
			phi1sew(first, next);	// the edge is linked to the face
			phi2sew(dNext, next);	// the face is linked to the hole
		}
	} while (dPhi1 != d);

	if (countEdges == 2)
	{
		countEdges = 0 ;
		collapseDegeneratedFace(first); // if the closing face is 2-sided, collapse it
	}

	return countEdges ;
}

void Map2::closeMap(DartMarker& marker)
{
	// Search the map for topological holes (fixed point for phi2)
	for (Dart d = begin(); d != end(); next(d))
	{
		if (phi2(d) == d)
		{
			closeHole(d);
			marker.markOrbit(FACE_ORBIT, phi2(d)) ;
		}
	}
}

void Map2::reverseOrientation()
{
	DartMarkerNoUnmark mf(*this);

	// reverse all faces (only phi1 is modified)
	for (Dart d = begin(); d != end(); next(d))
	{
		if (!mf.isMarked(d))
		{
			reverseFace(d);
			mf.markOrbit(FACE_ORBIT, d);
		}
	}

	// store all new phi2
	std::vector<Dart> vd;
	vd.reserve(getNbDarts());
	for (Dart d = begin(); d != end(); next(d))
	{
		Dart e = phi_1(phi2(phi1(d)));
		vd.push_back(e);
	}

	// apply the phi2sew with stored phi2 on all darts
	std::vector<Dart>::iterator id = vd.begin();
	for (Dart d = begin(); d != end(); next(d),++id)
	{
		if (mf.isMarked(d))
		{
			mf.unmark(d);		// unmark the two darts
			mf.unmark(*id);
			if (phi2(d) != d)
				phi2unsew(d);	// unsew the two darts if necessary
			if (phi2(*id) != *id)
				phi2unsew(*id);
			phi2sew(d, *id); 	// sew the darts
		}
	}
}

void Map2::computeDual()
{
	AttribContainer& cont = m_attribs[DART_ORBIT] ;

	unsigned int phi1_idx = cont.getAttribute("phi1") ;
	unsigned int new_phi1_idx = cont.addAttribute<Dart>("new_phi1") ;

	AttribMultiVect<Dart>& new_phi1 = cont.getDataVector<Dart>(new_phi1_idx) ;

	for (Dart d = begin() ; d != end() ; next(d))
	{
		Dart dd = alpha1(d) ;
		new_phi1[d.index] = dd ;
		(*m_phi_1)[dd.index] = d ;
	}

	cont.swapAttributes(phi1_idx, new_phi1_idx) ;

	cont.removeAttribute(new_phi1_idx) ;
}

/*! @name Topological Queries
 *  Return or set various topological information
 *************************************************************************/

bool Map2::sameOrientedVertex(Dart d, Dart e)
{
	Dart dNext = d;				// Foreach dart dNext in the vertex of d
	do
	{
		if (dNext == e)			// Test equality with e
			return true;
		dNext = alpha1(dNext);
	} while (dNext != d);
	return false;				// None is equal to e => vertices are distinct
}

unsigned int Map2::vertexDegree(Dart d)
{
	unsigned int count = 0 ;
	Dart dNext = d ;
	do
	{
		++count ;
		if(phi2(dNext) == dNext)
			++count ;
		dNext = alpha1(dNext) ;
	} while (dNext != d) ;
	return count ;
}

unsigned int Map2::volumeDegree(Dart d)
{
	unsigned int count = 0;
	DartMarkerStore mark(*this);		// Lock a marker

	std::vector<Dart> visitedFaces;		// Faces that are traversed
	visitedFaces.reserve(16);
	visitedFaces.push_back(d);			// Start with the face of d
	std::vector<Dart>::iterator face;

	// For every face added to the list
	for (face = visitedFaces.begin(); face != visitedFaces.end(); ++face)
	{
		if (!mark.isMarked(*face))		// Face has not been visited yet
		{
			++count;

			Dart dNext = *face ;
			do
			{
				mark.mark(dNext);					// Mark
				Dart adj = phi2(dNext);				// Get adjacent face
				if (adj != dNext && !mark.isMarked(adj))
					visitedFaces.push_back(adj);	// Add it
				dNext = phi1(dNext);
			} while(dNext != *face);
		}
	}

	return count;
}

bool Map2::isBoundaryVertex(Dart d)
{
	Dart dNext = d ;
	do
	{
		if(phi2(dNext) == dNext)
			return true ;
		dNext = alpha1(dNext) ;
	} while (dNext != d) ;
	return false ;
}

Dart Map2::nextOnBoundary(Dart d)
{
	assert(phi2(d) == d);	// Only work on boundary dart
	Dart dPhi1;
	Dart dNext = d;
	do
	{						// Loop inside the vertex
		dPhi1 = phi1(dNext);
		dNext = phi2(dPhi1);
	} while (dNext != dPhi1);
	return dNext;
}

bool Map2::isTriangular()
{
	DartMarker m(*this) ;
	for(Dart d = begin(); d != end(); next(d))
	{
		if(!m.isMarked(d))
		{
			m.markOrbit(FACE_ORBIT, d) ;
			Dart dd = d ;
			bool t = isFaceTriangle(d) ;
			if(!t)
			{
				return false ;
			}
		}
	}
	return true ;
}

bool Map2::check()
{
	std::cout << "Check: topology begin" << std::endl;
	DartMarker m(*this);
	for(Dart d = begin(); d != end(); next(d))
	{
		Dart d2 = phi2(d);
		if (phi2(d2) != d)	// phi2 involution ?
		{
			std::cout << "Check: phi2 is not an involution" << std::endl;
			return false;
		}

		Dart d1 = phi1(d);
		if (phi_1(d1) != d)	// phi1 a une image correcte ?
		{
			std::cout << "Check: unconsistent phi_1 link" << std::endl;
			return false;
		}

		if (m.isMarked(d1))	// phi1 a un seul antécédent ?
		{
			std::cout << "Check: dart with two phi1 predecessors" << std::endl;
			return false;
		}
		m.mark(d1);

		if (d1 == d)
			std::cout << "Check: (warning) face loop (one edge)" << std::endl;
		if (phi1(d1) == d)
			std::cout << "Check: (warning) face with only two edges" << std::endl;
		if (phi2(d1) == d)
			std::cout << "Check: (warning) dandling edge" << std::endl;
	}
	for(Dart d = begin(); d != end(); next(d))
	{
		if (!m.isMarked(d))	// phi1 a au moins un antécédent ?
		{
			std::cout << "Check: dart with no phi1 predecessor" << std::endl;
			return false;
		}
	}
	std::cout << "Check: topology ok" << std::endl;
	return true;
}

/*! @name Cell Functors
 *  Apply functors to all darts of a cell
 *************************************************************************/

bool Map2::foreach_dart_of_vertex(Dart d, FunctorType& f)
{
	Dart dNext = d;
	do
	{
		if (f(dNext))
			return true;
		dNext = alpha1(dNext);
 	} while (dNext != d);
 	return false;
}

bool Map2::foreach_dart_of_edge(Dart d, FunctorType& fonct)
{
	if (fonct(d))
		return true;

	Dart d2 = phi2(d);
	if (d2 != d)
		return fonct(d2);
	else
		return false;
}

bool Map2::foreach_dart_of_oriented_volume(Dart d, FunctorType& f)
{
	DartMarkerStore mark(*this);	// Lock a marker
	bool found = false;				// Last functor return value

	std::list<Dart> visitedFaces;	// Faces that are traversed
	visitedFaces.push_back(d);		// Start with the face of d
	std::list<Dart>::iterator face;

	// For every face added to the list
	for (face = visitedFaces.begin(); !found && face != visitedFaces.end(); ++face)
	{
		if (!mark.isMarked(*face))		// Face has not been visited yet
		{
			// Apply functor to the darts of the face
			found = foreach_dart_of_oriented_face(*face, f);

			// If functor returns false then mark visited darts (current face)
			// and add non visited adjacent faces to the list of face
			if (!found)
			{
				Dart dNext = *face ;
				do
				{
					mark.mark(dNext);					// Mark
					Dart adj = phi2(dNext);				// Get adjacent face
					if (adj != dNext && !mark.isMarked(adj))
						visitedFaces.push_back(adj);	// Add it
					dNext = phi1(dNext);
				} while(dNext != *face);
			}
		}
	}
	return found;
}

bool Map2::foreach_dart_of_star(Dart d, unsigned int orbit, FunctorType& f)
{
	if(orbit == VERTEX_ORBIT)
	{

		Dart dNext = d;
		do
		{
			if(Map1::foreach_dart_of_face(dNext,f))
				return true;

			dNext = alpha1(dNext);
		} while (dNext != d);

		return false;
	}
	else if(orbit == FACE_ORBIT)
	{
		if(Map1::foreach_dart_of_face(d,f))
			return true;

		if(phi2(d) != d)
			return Map1::foreach_dart_of_face(phi2(d),f);
		else
			return false;
	}

	return false;
}

bool Map2::foreach_dart_of_link(Dart d, unsigned int orbit, FunctorType& f)
{
	if(orbit == VERTEX_ORBIT)
	{
		Dart dNext = d;
		do
		{
			if(Map2::foreach_dart_of_edge(phi1(dNext),f))
				return true;

			dNext = alpha1(dNext);
		} while (dNext != d);

		return false;
	}
	else if(orbit == FACE_ORBIT)
	{
		if(Map2::foreach_dart_of_vertex(phi_1(d),f))
			return true;

		if(Map2::foreach_dart_of_vertex(phi_1(phi2(d)),f))
			return true;

		return false;
	}

	return false;
}


} // namespace CGoGN
