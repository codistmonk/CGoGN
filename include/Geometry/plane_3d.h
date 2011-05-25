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

#ifndef __PLANE_3D__
#define __PLANE_3D__

namespace CGoGN
{

namespace Geom
{

typedef enum 
{
	ON = 0,
	OVER,
	UNDER
} Orientation3D ;

/*
 * Class for the representation of 3d planes
 */
template <typename T>
class Plane3D
{
	public:
		/**********************************************/
		/*                CONSTRUCTORS                */
		/**********************************************/

		Plane3D();

		Plane3D(const Plane3D<T>& p);

		// construct the plane from a normal vector and a scalar
		Plane3D(const Vector<3,T>& n, T d);

		// construct the plane with normal vector n and going through p
		Plane3D(const Vector<3,T>& n, const Vector<3,T>& p);

		// construct the plane going through p1, p2 and p3
		Plane3D(const Vector<3,T>& p1, const Vector<3,T>& p2, const Vector<3,T>& p3);

		/**********************************************/
		/*                 ACCESSORS                  */
		/**********************************************/

		Vector<3,T>& normal();

		const Vector<3,T>& normal() const;

		T& d();

		const T& d() const;

		/**********************************************/
		/*             UTILITY FUNCTIONS              */
		/**********************************************/

		// compute the distance between the plane and point p
		T distance(const Vector<3,T>& p) const;

		// project the point p onto the plane
		void project(Vector<3,T>& p) const;

		// return true or false according to the side of the plane where point p is
		Orientation3D orient(const Vector<3,T>& p) const;

		/**********************************************/
		/*             STREAM OPERATORS               */
		/**********************************************/

		template <typename TT>
		friend std::ostream& operator<<(std::ostream& out, const Plane3D<TT>& p);

		template <typename TT>
		friend std::istream& operator>>(std::istream& in, Plane3D<TT>& p);

	private:
		Vector<3,T> m_normal ;
		T m_d ;
} ;

} // namespace Geom

} // namespace CGoGN

#include "plane_3d.hpp"

#endif
