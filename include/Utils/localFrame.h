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

#ifndef LOCALFRAME_H_
#define LOCALFRAME_H_

namespace CGoGN {

namespace Utils {

/**
 * Class for representing a direct local frame composed of 3 orthonormal vectors T (tangent), B (bitangent) and N (normal).
 * This class can compress/decompress a local frame, switching from its explicit representation (3 vectors) to its compressed representation (1 vector).
 * Usage :
 *  VEC3 T,B,N ;							// current set of orthonormal vectors composing the direct frame.
 *  LocalFrame<PFP::VEC3> lf(T,B,N) ; 				// Constructor from explicit expression.
 *  if (lf.isOrthoNormalDirect())			// test if the frame is Orthogonal, Normalized and Direct
 *   VEC3 compressed = lf.getCompressed() ;	// Extract compressed frame
 *  LocalFrame<PFP::VEC3> decompressed(compressed) ;	// Constructor from implicit (compressed) expression.
 *
 * All formulae were provided by "Représentation compacte d'un repère local", june 14th, 2011 by K. Vanhoey
 */
template <typename PFP>
class LocalFrame
{
	typedef typename PFP::REAL REAL ;
	typedef typename PFP::VEC2 VEC2 ;
	typedef typename PFP::VEC3 VEC3 ;

private: // fields
	/**
	 * The three explicit vectors
	 */
	VEC3 m_T,m_B,m_N ;

public: // methods
	/**
	 * Constructor from explicit data
	 * @param T the tangent vector
	 * @param B the bitangent vector
	 * @param N the normal vector
	 */
	LocalFrame(const VEC3& T, const VEC3& B, const VEC3& N) ;
	/**
	 * Constructor from implicit (compressed representation)
	 * @param compressedFrame an implicit (compressed) version of the local frame
	 */
	LocalFrame(const VEC3& compressedFrame) ;
	~LocalFrame() {} ;

	/**
	 * Returns a compressed version of the current local frame
	 */
	VEC3 getCompressed() ;

	/**
	 * Tests if the frame is direct
	 * @return true if the frame is direct
	 */
	bool isDirect() ;

	/**
	 * Tests if the frame is orthogonal
	 * @return true if the frame is orthogonal
	 */
	bool isOrthogonal() ;

	/**
	 * Tests if the frame is normalized
	 * @return true if the frame is normalized
	 */
	bool isNormalized() ;

	/**
	 * Tests if the frame is direct, normalized and orthogonal
	 * @return true if the frame is direct, normalized and orthogonal
	 */
	bool isOrthoNormalDirect() ;

	/**
	 * @return current tangent vector
	 */
	VEC3& getT() { return m_T ; }
	const VEC3& getT() const { return m_T ; }

	/**
	 * @return current bitangent vector
	 */
	VEC3& getB() { return m_B ; }
	const VEC3& getB() const { return m_B ; }

	/**
	 * @return current normal vector
	 */
	VEC3& getN() { return m_N ; }
	const VEC3& getN() const { return m_N ; }

	friend std::ostream& operator<< (std::ostream &out, const LocalFrame& lf) {
		out << "T : " << lf.m_T << std::endl ;
		out << "B : " << lf.m_B << std::endl ;
		out << "N : " << lf.m_N << std::endl ;
		return out ;
	} ;

private : // methods
	VEC2 carthToSpherical(const VEC3& carth) const ;
	VEC3 sphericalToCarth(const VEC2& sph) const ;
} ;

} // Utils

} // CGoGN

#include "localFrame.hpp"

#endif /* LOCALFRAME_H_ */
