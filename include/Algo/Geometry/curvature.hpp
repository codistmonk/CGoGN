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

#include "Algo/Geometry/localFrame.h"
#include "Geometry/matrix.h"
#include "Topology/generic/cellmarker.h"
#include "Algo/Selection/collector.h"

extern "C"
{
	#include "C_BLAS_LAPACK/INCLUDE/f2c.h"
	#include "C_BLAS_LAPACK/INCLUDE/clapack.h"
}
#undef max
#undef min

namespace CGoGN
{

namespace Algo
{

namespace Geometry
{

template <typename PFP>
void computeCurvatureVertices_QuadraticFitting(
	typename PFP::MAP& map,
	const typename PFP::TVEC3& position,
	const typename PFP::TVEC3& normal,
	typename PFP::TREAL& kmax,
	typename PFP::TREAL& kmin,
	typename PFP::TVEC3& Kmax,
	typename PFP::TVEC3& Kmin,
	const FunctorSelect& select)
{
	CellMarker marker(map, VERTEX);
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(select(d) && !marker.isMarked(d))
		{
			marker.mark(d);
			computeCurvatureVertex_QuadraticFitting<PFP>(map, d, position, normal, kmax, kmin, Kmax, Kmin) ;
		}
	}
}

template <typename PFP>
void computeCurvatureVertex_QuadraticFitting(
	typename PFP::MAP& map,
	Dart dart,
	const typename PFP::TVEC3& position,
	const typename PFP::TVEC3& normal,
	typename PFP::TREAL& kmax,
	typename PFP::TREAL& kmin,
	typename PFP::TVEC3& Kmax,
	typename PFP::TVEC3& Kmin)
{
	typedef typename PFP::REAL REAL ;
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::MATRIX33 MATRIX33 ;

	VEC3 n = normal[dart] ;

	MATRIX33 localFrame = Algo::Geometry::vertexLocalFrame<PFP>(map, dart, position, n) ;
	MATRIX33 invLocalFrame ;
	localFrame.invert(invLocalFrame) ;

	REAL a, b, c, d, e;
	//vertexCubicFitting(map,dart,localFrame,a,b,c,d,e,f,g,h,i) ;
	vertexQuadraticFitting<PFP>(map, dart, localFrame, position, normal, a, b, c, d, e) ;

	REAL kmax_v, kmin_v, Kmax_x, Kmax_y ;
	//int res = slaev2_(&e, &f, &g, &maxC, &minC, &dirX, &dirY) ;
	/*int res = */slaev2_(&a, &b, &c, &kmax_v, &kmin_v, &Kmax_x, &Kmax_y) ;

	VEC3 Kmax_v(Kmax_x, Kmax_y, 0.0f) ;
	Kmax_v = invLocalFrame * Kmax_v ;
	VEC3 Kmin_v = n ^ Kmax_v ;

	if (kmax_v < kmin_v)
	{
		kmax[dart] = -kmax_v ;
		kmin[dart] = -kmin_v ;
		Kmax[dart] = Kmax_v ;
		Kmin[dart] = Kmin_v ;
	}
	else
	{
		kmax[dart] = -kmin_v ;
		kmin[dart] = -kmax_v ;
		Kmax[dart] = Kmin_v ;
		Kmin[dart] = Kmax_v ;
	}
}

template <typename PFP>
void vertexQuadraticFitting(
		typename PFP::MAP& map,
		Dart dart,
		typename PFP::MATRIX33& localFrame,
		const typename PFP::TVEC3& position,
		const typename PFP::TVEC3& normal,
		float& a, float& b, float& c, float& d, float& e)
{
	typename PFP::VEC3 p = position[dart] ;

	LinearSolver<CPUSolverTraits> solver(5) ;
	solver.set_least_squares(true) ;
	solver.begin_system() ;
	Dart it = dart ;
	do
	{
		// 1-ring vertices
		typename PFP::VEC3 v = position[map.phi2(it)] ;
		quadraticFittingAddVertexPos<PFP>(v, p, localFrame, solver) ;
		typename PFP::VEC3 n = normal[map.phi2(it)] ;
		quadraticFittingAddVertexNormal<PFP>(v, n, p, localFrame, solver) ;
		// 2-ring vertices
//		Dart d2 = map.phi1(map.phi1(map.phi2(map.phi1(it)))) ;
//		VEC3 v2 = position[d2] ;
//		quadricFittingAddVertexPos(v2, p, localFrame, solver) ;
//		VEC3 n2 = normal[d2] ;
//		quadricFittingAddVertexNormal(v2, n2, p, localFrame, solver) ;
		it = map.phi1( map.phi2(it)) ;
	} while (it != dart) ;
	solver.end_system() ;
	solver.solve() ;

	a = solver.variable(0).value() ;
	b = solver.variable(1).value() ;
	c = solver.variable(2).value() ;
	d = solver.variable(3).value() ;
	e = solver.variable(4).value() ;
}

template <typename PFP>
void quadraticFittingAddVertexPos(typename PFP::VEC3& v, typename PFP::VEC3& p, typename PFP::MATRIX33& localFrame, LinearSolver<CPUSolverTraits>& solver)
{
	typename PFP::VEC3 vec = v - p ;
	vec = localFrame * vec ;
	solver.begin_row() ;

	solver.add_coefficient(0, vec[0]*vec[0]) ;
	solver.add_coefficient(1, vec[0]*vec[1]) ;
	solver.add_coefficient(2, vec[1]*vec[1]) ;
	solver.add_coefficient(3, vec[0]) ;
	solver.add_coefficient(4, vec[1]) ;

	solver.set_right_hand_side(vec[2]) ;
	solver.end_row() ;
}

template <typename PFP>
void quadraticFittingAddVertexNormal(typename PFP::VEC3& v, typename PFP::VEC3& n, typename PFP::VEC3& p, typename PFP::MATRIX33& localFrame, LinearSolver<CPUSolverTraits>& solver)
{
	typename PFP::VEC3 vec = v - p ;
	vec = localFrame * vec ;
	typename PFP::VEC3 norm = localFrame * n ;
	solver.begin_row() ;

	solver.add_coefficient(0, 2.0f * vec[0]) ;
	solver.add_coefficient(1, vec[1]) ;
	solver.add_coefficient(2, 0) ;
	solver.add_coefficient(3, 1.0f) ;
	solver.add_coefficient(4, 0) ;
	solver.set_right_hand_side(-1.0f * norm[0] / norm[2]) ;
	solver.end_row() ;

	solver.begin_row() ;
	solver.add_coefficient(0, 0) ;
	solver.add_coefficient(1, vec[0]) ;
	solver.add_coefficient(2, 2.0f * vec[1]) ;
	solver.add_coefficient(3, 0) ;
	solver.add_coefficient(4, 1.0f) ;
	solver.set_right_hand_side(-1.0f * norm[1] / norm[2]) ;
	solver.end_row() ;
}
/*
template <typename PFP>
void vertexCubicFitting(Dart dart, gmtl::Vec3f& normal, float& a, float& b, float& c, float& d, float& e, float& f, float& g, float& h, float& i)
{
	gmtl::Matrix33f localFrame, invLocalFrame ;
	Algo::Geometry::vertexLocalFrame<PFP>(m_map,dart,normal,localFrame) ;
	gmtl::invertFull(invLocalFrame, localFrame) ;
	gmtl::Vec3f p = m_map.getVertexEmb(dart)->getPosition() ;
	solverC->reset(false) ;
	solverC->set_least_squares(true) ;
	solverC->begin_system() ;
	Dart it = dart ;
	do
	{
		// 1-ring vertices
		gmtl::Vec3f v = m_map.getVertexEmb(m_map.phi2(it))->getPosition() ;
		cubicFittingAddVertexPos(v,p,localFrame) ;
		gmtl::Vec3f n = m_normalsV[m_map.getVertexEmb(m_map.phi2(it))->getLabel()] ;
		cubicFittingAddVertexNormal(v,n,p,localFrame) ;
		// 2-ring vertices
//		Dart d2 = m_map.phi1(m_map.phi1(m_map.phi2(m_map.phi1(it)))) ;
//		gmtl::Vec3f v2 = m_map.getVertexEmb(d2)->getPosition() ;
//		cubicFittingAddVertexPos(v2,p,localFrame) ;
//		gmtl::Vec3f n2 = m_normalsV[m_map.getVertexEmb(d2)->getLabel()] ;
//		cubicFittingAddVertexNormal(v2,n2,p,localFrame) ;

		it = m_map.phi1( m_map.phi2(it)) ;
	} while (it != dart) ;
	solverC->end_system() ;
	solverC->solve() ;
	a = solverC->variable(0).value() ;
	b = solverC->variable(1).value() ;
	c = solverC->variable(2).value() ;
	d = solverC->variable(3).value() ;
	e = solverC->variable(4).value() ;
	f = solverC->variable(5).value() ;
	g = solverC->variable(6).value() ;
	h = solverC->variable(7).value() ;
	i = solverC->variable(8).value() ;

//	normal = gmtl::Vec3f(-h, -i, 1.0f) ;
//	gmtl::normalize(normal) ;
//	normal = invLocalFrame * normal ;
}

template <typename PFP>
void cubicFittingAddVertexPos(gmtl::Vec3f& v, gmtl::Vec3f& p, gmtl::Matrix33f& localFrame)
{
	gmtl::Vec3f vec = v - p ;
	vec = localFrame * vec ;
	solverC->begin_row() ;

	solverC->add_coefficient(0, vec[0]*vec[0]*vec[0]) ;
	solverC->add_coefficient(1, vec[0]*vec[0]*vec[1]) ;
	solverC->add_coefficient(2, vec[0]*vec[1]*vec[1]) ;
	solverC->add_coefficient(3, vec[1]*vec[1]*vec[1]) ;
	solverC->add_coefficient(4, vec[0]*vec[0]) ;
	solverC->add_coefficient(5, vec[0]*vec[1]) ;
	solverC->add_coefficient(6, vec[1]*vec[1]) ;
	solverC->add_coefficient(7, vec[0]) ;
	solverC->add_coefficient(8, vec[1]) ;

	solverC->set_right_hand_side(vec[2]) ;
	solverC->end_row() ;
}

template <typename PFP>
void cubicFittingAddVertexNormal(gmtl::Vec3f& v, gmtl::Vec3f& n, gmtl::Vec3f& p, gmtl::Matrix33f& localFrame)
{
	gmtl::Vec3f vec = v - p ;
	vec = localFrame * vec ;
	gmtl::Vec3f norm = localFrame * n ;
	solverC->begin_row() ;

	solverC->add_coefficient(0, 3.0f*vec[0]*vec[0]) ;
	solverC->add_coefficient(1, 2.0f*vec[0]*vec[1]) ;
	solverC->add_coefficient(2, vec[1]*vec[1]) ;
	solverC->add_coefficient(3, 0) ;
	solverC->add_coefficient(4, 2.0f*vec[0]) ;
	solverC->add_coefficient(5, vec[1]) ;
	solverC->add_coefficient(6, 0) ;
	solverC->add_coefficient(7, 1.0f) ;
	solverC->add_coefficient(8, 0) ;
	solverC->set_right_hand_side(-1.0f*norm[0]/norm[2]) ;
	solverC->end_row() ;

	solverC->begin_row() ;
	solverC->add_coefficient(0, 0) ;
	solverC->add_coefficient(1, vec[0]*vec[0]) ;
	solverC->add_coefficient(2, 2.0f*vec[0]*vec[1]) ;
	solverC->add_coefficient(3, 3.0f*vec[1]*vec[1]) ;
	solverC->add_coefficient(4, 0) ;
	solverC->add_coefficient(5, vec[0]) ;
	solverC->add_coefficient(6, 2.0f*vec[1]) ;
	solverC->add_coefficient(7, 0) ;
	solverC->add_coefficient(8, 1.0f) ;
	solverC->set_right_hand_side(-1.0f*norm[1]/norm[2]) ;
	solverC->end_row() ;
}
*/

template <typename PFP>
void computeCurvatureVertices_NormalCycles(
	typename PFP::MAP& map,
	typename PFP::REAL radius,
	const typename PFP::TVEC3& position,
	const typename PFP::TVEC3& normal,
	const typename PFP::TREAL& angles,
	typename PFP::TREAL& kmax,
	typename PFP::TREAL& kmin,
	typename PFP::TVEC3& Kmax,
	typename PFP::TVEC3& Kmin,
	typename PFP::TVEC3& Knormal,
	const FunctorSelect& select = SelectorTrue())
{
	CellMarker marker(map, VERTEX);
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(select(d) && !marker.isMarked(d))
		{
			marker.mark(d);
			computeCurvatureVertex_NormalCycles<PFP>(map, d, radius, position, normal, angles, kmax, kmin, Kmax, Kmin, Knormal) ;
		}
	}
}

template <typename PFP>
void computeCurvatureVertex_NormalCycles(
	typename PFP::MAP& map,
	Dart dart,
	typename PFP::REAL radius,
	const typename PFP::TVEC3& position,
	const typename PFP::TVEC3& normal,
	const typename PFP::TREAL& angles,
	typename PFP::TREAL& kmax,
	typename PFP::TREAL& kmin,
	typename PFP::TVEC3& Kmax,
	typename PFP::TVEC3& Kmin,
	typename PFP::TVEC3& Knormal)
{
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

	Algo::Selection::Collector_WithinSphere<PFP> neigh(map, position) ;
	neigh.init(dart, radius) ;
	neigh.collect() ;
	neigh.computeArea() ;

	VEC3 center = position[neigh.getCenter()] ;

	typename PFP::MATRIX33 tensor(0) ;

	// inside
	const std::vector<Dart>& vd1 = neigh.getInsideEdges() ;
	for (std::vector<Dart>::const_iterator it = vd1.begin(); it != vd1.end(); ++it)
	{
		const VEC3 e = position[map.phi2(*it)] - position[*it] ;
		tensor += Geom::transposed_vectors_mult(e,e) * angles[*it] * (1 / e.norm()) ;
	}

	// border
	const std::vector<Dart>& vd2 = neigh.getBorder() ;
	for (std::vector<Dart>::const_iterator it = vd2.begin(); it != vd2.end(); ++it)
	{
		const VEC3 e = position[map.phi2(*it)] - position[*it] ;
		const REAL alpha = neigh.intersect_SphereEdge(*it, map.phi2(*it)) ;
		tensor += Geom::transposed_vectors_mult(e,e) * angles[*it] * (1 / e.norm()) * alpha ;
	}

	tensor /= neigh.getArea() ;

	long int n = 3, lda = 3, info, lwork = 9 ;
	char jobz='V', uplo = 'U' ;
	float work[9] ;
	float w[3] ;
	float a[3*3] = {
		tensor(0,0), 0.0f, 0.0f,
		tensor(1,0), tensor(1,1), 0.0f,
		tensor(2,0), tensor(2,1), tensor(2,2)
	} ;
	// Solve eigenproblem
	ssyev_(&jobz, &uplo, (integer*)&n, a, (integer*)&lda, w, work, (integer*)&lwork, (integer*)&info) ;
	// Check for convergence
	if(info > 0)
		std::cerr << "clapack ssyev_ failed to compute eigenvalues : exit with code " << info << std::endl ;
	// sort eigen components : w[s[0]] has minimal absolute value ; kmin = w[s[1]] <= w[s[2]] = kmax
	int s[3] = {0, 1, 2} ;
	int tmp ;
	if (abs(w[s[2]]) < abs(w[s[1]])) { tmp = s[1] ; s[1] = s[2] ; s[2] = tmp ; }
	if (abs(w[s[1]]) < abs(w[s[0]])) { tmp = s[0] ; s[0] = s[1] ; s[1] = tmp ; }
	if (w[s[2]] < w[s[1]]) { tmp = s[1] ; s[1] = s[2] ; s[2] = tmp ; }

	kmin[dart] = w[s[1]] ;
	kmax[dart] = w[s[2]] ;
	VEC3& dirMin = Kmin[dart] ;
	dirMin[0] = a[3*s[2]];
	dirMin[1] = a[3*s[2]+1];
	dirMin[2] = a[3*s[2]+2]; // warning : Kmin and Kmax are switched
	VEC3& dirMax = Kmax[dart] ;
	dirMax[0] = a[3*s[1]];
	dirMax[1] = a[3*s[1]+1];
	dirMax[2] = a[3*s[1]+2]; // warning : Kmin and Kmax are switched
	VEC3& dirNormal = Knormal[dart] ;
	dirNormal[0] = a[3*s[0]];
	dirNormal[1] = a[3*s[0]+1];
	dirNormal[2] = a[3*s[0]+2];
	if (dirNormal * normal[dart] < 0)
		dirNormal *= -1; // change orientation
}

} // namespace Geometry

} // namespace Algo

} // namespace CGoGN
