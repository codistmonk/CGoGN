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

#ifndef __MAP2_H__
#define __MAP2_H__

#include "Topology/map/map1.h"

namespace CGoGN
{

/*! \brief The class of dual 2-dimensional combinatorial maps:
 *  set of oriented faces pairwise sewed by an adjacency relation.
 *  A dual 2-map represents close or open oriented 2-manifolds (surfaces).
 *  - A dual 2-map is made of darts linked by the phi1 permutation
 * 	and/or the phi2 one-to-one relation.
 *  - In this class darts are interpreted as oriented edges.
 *  - The phi1 relation defines oriented faces (see Map1)
 *  and faces may have arbitrary size (degenerated faces are accepted).
 *  - The phi2 relation links oriented faces along oriented edges.
 *  A set of phi2-linked faces represents a surface 
 *  - Edges that have no phi2-link are border edges. If there exists
 *  such edges the map is open.
 *  - When every edge is phi2-linked, the map is closed. In this case
 *  some optimizations are enabled that speed up the processing of vertices.
 */
class Map2 : public Map1
{
protected:
	AttribMultiVect<Dart>* m_phi2 ;

public:
	typedef Map1 ParentMap;

	Map2();

	virtual std::string mapTypeName();

	virtual unsigned int dimension();

	/*! @name Basic Topological Operators
	 * Access and Modification
	 *************************************************************************/

	virtual Dart newDart();

	Dart phi2(Dart d);

	template <int N>
	Dart phi(Dart d);

	Dart alpha0(Dart d);

	Dart alpha1(Dart d);

	Dart alpha_1(Dart d);

protected:
	//! Link dart d with dart e by an involution
	/*  @param d,e the darts to link
	 *	- Before:	d->d and e->e
	 *	- After:	d->e and e->d
	 */
	void phi2sew(Dart d, Dart e);

	//! Unlink the current dart by an involution
	/*  @param d the dart to unlink
	 * - Before:	d->e and e->d
	 * - After:		d->d and e->e
	 */
	void phi2unsew(Dart d);

	/*! @name Generator and Deletor
	 *  To generate or delete faces in a 2-map
	 *************************************************************************/

	//@{
	//! Delete an oriented face erasing all its darts.
	/*! The phi2-links around the face are removed
	 *  @param d a dart of the face
	 */
	void deleteOrientedFace(Dart d) ;

public:
	virtual void deleteFace(Dart d) ;
	//@}

	/*! @name Topological Operators
	 *  Topological operations on 2-maps
	 *************************************************************************/

	//@{
	//! Split a vertex v between d and e inserting an edge after d and e
	/*! \pre Darts d & e MUST belong to the same vertex
	 *  @param d first dart in vertex v
	 *  @param e second dart in vertex v
	 */
	virtual void splitVertex(Dart d, Dart e);

	//! Delete the vertex of d (works only for internal vertices)
	/*! All the faces around the vertex are merged into one face
	 *  @param d a dart of the vertex to delete
	 * @return true if the deletion has been executed, false otherwise
	 */
	virtual bool deleteVertex(Dart d) ;

	//! Cut the edge of d and its opposite edge if it exists
	/*! @param d a dart of the edge to cut
	 */
	virtual void cutEdge(Dart d);

	//! Collapse an edge (that is deleted) possibly merging its vertices
	/*! If delDegenerateFaces is true, the method checks that no degenerate
	 *  faces are built (faces with less than 3 edges). If it occurs the faces
	 *  are deleted and the adjacencies are updated (see collapseDegeneratedFace).
	 *  \warning This may produce two distinct vertices if the edge
	 *  was the only link between two border faces
	 *  @param d a dart in the deleted edge
	 *  @param delDegenerateFaces a boolean (default to true)
	 *  @return a dart of the resulting vertex
	 */
	virtual Dart collapseEdge(Dart d, bool delDegenerateFaces = true);

	/**
	 * Flip the edge of d. (rotation in phi1 order)
	 * WARNING : Works only for non-border edges.
	 * @param d a dart of the edge to flip
	 * @return true if the flip has been executed, false otherwise
	 */
	virtual bool flipEdge(Dart d);

	/**
	 * Flip the edge of d. (rotation in phi_1 order)
	 * WARNING : Works only for non-border edges.
	 * @param d a dart of the edge to flip
	 * @return true if the flip has been executed, false otherwise
	 */
	virtual bool flipBackEdge(Dart d);

	//! Sew two oriented faces along oriented edges
	/*! \pre Darts d & e MUST be fixed point of phi2 relation
	 *  @param d a dart of the first face
	 *  @param e a dart of the second face
	 */
	virtual void sewFaces(Dart d, Dart e);

	//! Unsew two oriented faces along oriented edges
	 /*! @param d a dart of one face
	 */
	virtual void unsewFaces(Dart d);
	
	//! Delete an oriented face if and only if it has one or two edges
	/*! If the face is phi2-linked to two distinct adjacent faces,
	 *  then those two faces are phi2-linked
	 *  @param d a dart of the face
	 *  @return true if the collapse has been executed, false otherwise
	 */
	virtual bool collapseDegeneratedFace(Dart d);

	//! Split a face f between d and e inserting an edge between vertices d & e
	/*! \pre Darts d & e MUST belong to the same face
	 *  @param d first dart in face f
	 *  @param e second dart in face f
	 */
	virtual void splitFace(Dart d, Dart e);

	//! Merge the two faces incident to the edge of d.
	/*! Works only for non-border edges.
	 *  \warning Darts d & phi2(d) no longer exist after the call
	 *  @param d a dart in the first face
	 *  @return true if the merge has been executed, false otherwise
	 */
	virtual bool mergeFaces(Dart d);

	/**
	 * Extract a pair of sewed triangles and sew their adjacent faces
	 * d is a dart of the common edge of the pair of triangles
	 */
	void extractTrianglePair(Dart d) ;

	/**
	 * Insert a pair of sewed triangles in a vertex by exploding the edges of v1 and v2
	 * v1 and v2 belong to the same vertex
	 * d is a dart of the common edge of the pair of triangles
	 */
	void insertTrianglePair(Dart d, Dart v1, Dart v2) ;

	//! Merge two volumes along two faces.
	/*! Works only if the two faces have the same number of edges.
	 *  The faces adjacent to the two given faces are pairwise phi2-linked
	 *  then the 2 faces are deleted.
	 *  If the two faces belong to different surfaces, the surfaces are merged,
	 *  else a handle is created that increases the genus of the surface.
	 *  @param d a dart of the first face
	 *  @param e a dart of the second face
	 *  @return true if the merge has been executed, false otherwise
	 */
	virtual bool mergeVolumes(Dart d, Dart e);

	//! Close a topological hole (a sequence of connected fixed point of phi2).
	/*! \pre dart d MUST be fixed point of phi2 relation
	 *  Add a face to the map that closes the hole.
	 *  The darts of this face are marked with holeMarker.
	 *  @param d a dart of the hole (with phi2(d)==d)
	 *  @return the degree of the created face
	 */
	virtual unsigned int closeHole(Dart d);

	//TODO a mettre en algo
	//! Close the map removing topological holes.
	/*! Add faces to the map that close every existing hole.
	 *  These faces are marked.
	 *  \warning The embeddings of vertices are not updated
	 *  @param marker
	 */
	void closeMap(DartMarker& marker);

	// TODO a mettre en algo
	/**
	* Reverse orientation of map (reverse orientation of all faces)
	* no dart created, each dart keeps its embedding, only changing phi1 and phi2
	*/
	void reverseOrientation();

	// TODO a mettre en algo
	/**
	 * compute the topological dual of the current map
	 */
	void computeDual();
	//@}

	/*! @name Topological Queries
	 *  Return or set various topological information
	 *************************************************************************/

	//@{
	//! Test if dart d and e belong to the same oriented vertex
	/*! @param d a dart
	 *  @param e a dart
	 */
	bool sameOrientedVertex(Dart d, Dart e) ;

	//! Test if dart d and e belong to the same vertex
	/*! @param d a dart
	 *  @param e a dart
	 */
	bool sameVertex(Dart d, Dart e) ;

	/**
	 * compute the number of edges of the vertex of d
	 */
	unsigned int vertexDegree(Dart d) ;

	/**
	 * compute the number of faces in the volume of d
	 */
	unsigned int volumeDegree(Dart d);

	/**
	 * tell if the vertex of d is on the boundary of the map
	 */
	bool isBoundaryVertex(Dart d) ;

	//! Follow the boundary of a surface as if it was a oriented face.
	/*! This operation alternate phi1 and phi2 operator until another
	 *  boudary dart is reached.
	 *  @param d a boundary dart
	 */
	Dart nextOnBoundary(Dart d);

	// TODO a mettre en algo
	/**
	 * check if the mesh is triangular or not
	 * @return a boolean indicating if the mesh is triangular
	 */
	bool isTriangular() ;

	// TODO a mettre en algo
	/**
	 * Check if map is complete
	 * Should be executed after import
	 */
	virtual bool check();
	//@}

	/*! @name Cell Functors
	 *  Apply functors to all darts of a cell
	 *************************************************************************/

	//@{
	//! Apply a functor on every dart of a vertex
	/*! @param d a dart of the vertex
	 *  @param f the functor to apply
	 */
	bool foreach_dart_of_vertex(Dart d, FunctorType& f);

	//! Apply a functor on every dart of a edge
	/*! @param d a dart of the edge
	 *  @param f the functor to apply
	 */
	bool foreach_dart_of_edge(Dart d, FunctorType& f);

	//! Apply a functor on every dart of a volume
	/*! @param d a dart of the volume
	 *  @param f the functor to apply
	 */
	bool foreach_dart_of_oriented_volume(Dart d, FunctorType& f);

	bool foreach_dart_of_volume(Dart d, FunctorType& f);

	//! Apply a functor on every dart of a connected component
	/*! @param d a dart of the connected component
	 *  @param f the functor to apply
	 */
	bool foreach_dart_of_cc(Dart d, FunctorType& f);

	//!
	/*! TODO Ajout a valider
	 * restreint aux complexes simpliciaux
	 * Apply a functor on the all darts in the set of the star from orbit
	 */
	bool foreach_dart_of_star(Dart d, unsigned int orbit, FunctorType& f);

	//!
	/*! TODO Ajout a valider
	 * restreint aux complexes simpliciaux
	 * Apply a functor on the all darts in the set of the link from orbit
	 */
	bool foreach_dart_of_link(Dart d, unsigned int orbit, FunctorType& f);

	//@}
};

} // namespace CGoGN

#include "Topology/map/map2.hpp"

#endif
