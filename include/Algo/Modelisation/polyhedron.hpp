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

namespace Algo
{

namespace Modelisation
{

/**
 * create a n-sided pyramid
 */
template <typename PFP>
Dart createPyramid(typename PFP::MAP& map, unsigned int n)
{
	Dart dres = Dart::nil();
	std::vector<Dart> m_tableVertDarts;
	m_tableVertDarts.reserve(n);

	// creation of triangles around circunference and storing vertices
	for (unsigned int i = 0; i < n; ++i)
	{
		Dart d = map.newFace(3, false);
		m_tableVertDarts.push_back(d);
	}

	// sewing the triangles
	for (unsigned int i = 0; i < n-1; ++i)
	{
		Dart d = m_tableVertDarts[i];
		d = map.phi_1(d);
		Dart e = m_tableVertDarts[i+1];
		e = map.phi1(e);
		map.sewFaces(d, e, false);
	}
	//sewing the last with the first
	map.sewFaces(map.phi1(m_tableVertDarts[0]), map.phi_1(m_tableVertDarts[n-1]), false);

	//sewing the bottom face
	Dart d1 = map.newFace(n, false);
	dres = d1;
	for(unsigned int i = 0; i < n ; ++i)
	{
		map.sewFaces(m_tableVertDarts[i], d1, false);
		d1 = map.phi1(d1);
	}

	//return a dart from the base
	return dres;
}

/**
 * create a n-sided prism
 */
template <typename PFP>
Dart createPrism(typename PFP::MAP& map, unsigned int n)
{
	Dart dres = Dart::nil();
	unsigned int nb = n*2;
	std::vector<Dart> m_tableVertDarts;
	m_tableVertDarts.reserve(nb);

	// creation of quads around circunference and storing vertices
	for (unsigned int i = 0; i < n; ++i)
	{
		Dart d = map.newFace(4, false);
		m_tableVertDarts.push_back(d);
	}

	// storing a dart from the vertex pointed by phi1(phi1(d))
	for (unsigned int i = 0; i < n; ++i)
	{
		m_tableVertDarts.push_back(map.phi1(map.phi1(m_tableVertDarts[i])));
	}

	// sewing the quads
	for (unsigned int i = 0; i < n-1; ++i)
	{
		Dart d = m_tableVertDarts[i];
		d = map.phi_1(d);
		Dart e = m_tableVertDarts[i+1];
		e = map.phi1(e);
		map.sewFaces(d, e, false);
	}
	//sewing the last with the first
	map.sewFaces(map.phi1(m_tableVertDarts[0]), map.phi_1(m_tableVertDarts[n-1]), false);

	//sewing the top & bottom faces
	Dart d1 = map.newFace(n, false);
	Dart d2 = map.newFace(n, false);
	dres = d1;
	for(unsigned int i = 0; i < n ; ++i)
	{
		map.sewFaces(m_tableVertDarts[i], d1, false);
		map.sewFaces(m_tableVertDarts[n+i], d2, false);
		d1 = map.phi1(d1);
		d2 = map.phi_1(d2);
	}

	//return a dart from the base
	return dres;
}

/**
 * create a n-sided diamond
 */
template <typename PFP>
Dart createDiamond(typename PFP::MAP& map, unsigned int nbSides)
{
	Dart res = Dart::nil();

	Dart firstP = createPyramid<PFP>(map,nbSides);
	Dart secondP = createPyramid<PFP>(map,nbSides);

	res = map.phi2(firstP);

	map.sewVolumes(firstP, secondP);
	//map.mergeVolumes(firstP);

	return res;
}



/**
 * create a 3-sided prism
 */
template <typename PFP>
Dart createTriangularPrism(typename PFP::MAP& map)
{
	return createPrism<PFP>(map, 3);
}

/**
 * create a hexahedron
 */
template <typename PFP>
Dart createHexahedron(typename PFP::MAP& map)
{
	return createPrism<PFP>(map, 4);
}

/**
 * create a tetrahedron
 */
template <typename PFP>
Dart createTetrahedron(typename PFP::MAP& map)
{
	return createPyramid<PFP>(map, 3);
}

/**
 * create a 4-sided pyramid
 */
template <typename PFP>
Dart createQuadrangularPyramid(typename PFP::MAP& map)
{
	return createPyramid<PFP>(map, 4);
}

/**
 * create an octahedron (i.e. 4-sided diamond)
 */
template <typename PFP>
Dart createOctahedron(typename PFP::MAP& map)
{
	return createDiamond<PFP>(map,4);
}





template <typename PFP>
void explodPolyhedron(typename PFP::MAP& map, Dart d, typename PFP::TVEC3 position)
{
	map.unsewVertexUmbrella(d);
	unsigned int newFaceDegree = map.closeHole(map.phi1(d));

	if(newFaceDegree != 3)
	{
		// chercher le brin ou demarrer

		std::multimap<Dart, int> edges ;
		typename std::multimap<Dart, int>::iterator it ;

		Dart d12 = map.phi2(map.phi1(d));
		Dart fit = d12;
		unsigned int i;

		do
		{
			i = map.faceDegree(map.phi2(fit));
			std::cout << "edge(" << fit << "," << i << ")" << std::endl;
			edges.insert(std::make_pair(fit, i));
			fit = map.phi1(fit);
		}
		while(fit != d12);

		do
		{
			//44 44
			if(edges.find(fit)->second == 4 && edges.find(map.phi1(fit))->second == 4
				&& !map.sameFace(map.phi2(fit), map.phi2(map.phi1(fit))))
			{
				map.splitFace(fit, map.phi1(map.phi1(fit)));
				fit = map.phi2(map.phi_1(fit));
				int i = map.faceDegree(fit);
				edges.insert(std::make_pair(fit, i));

//				Dart fit2 = map.phi2(fit) ;
//				typename PFP::VEC3 p1 = position[fit] ;
//				typename PFP::VEC3 p2 = position[fit2] ;
//
//				map.cutEdge(fit) ;
//				position[map.phi1(fit)] = typename PFP::REAL(0.5) * (p1 + p2);


				std::cout << "flip cas quad quad " << std::endl;
			}

			//3 3
			if(edges.find(fit)->second == 3 && edges.find(map.phi1(fit))->second == 3
				&& !map.sameFace(map.phi2(fit), map.phi2(map.phi1(fit))))
			{
				map.splitFace(fit, map.phi1(fit));
				fit = map.phi2(map.phi_1(fit));
				int i = map.faceDegree(fit);
				edges.insert(std::make_pair(fit, i));

				std::cout << "flip cas tri tri" << std::endl;
			}

			//3 44 ou 44 3
			if( ((edges.find(fit)->second == 4 && edges.find(map.phi1(fit))->second == 3)
				|| (edges.find(fit)->second == 3 && edges.find(map.phi1(fit))->second == 4))
					&& !map.sameFace(map.phi2(fit), map.phi2(map.phi1(fit))))
			{
				map.splitFace(fit, map.phi1(map.phi1(fit)));
				fit = map.phi2(map.phi_1(fit));
				int i = map.faceDegree(fit);
				edges.insert(std::make_pair(fit, i));

				std::cout << "flip cas quad tri" << std::endl;
			}

			fit = map.phi1(fit);
		}
		while(map.faceDegree(fit) > 4 && fit != d12);

	}
}

template <typename PFP>
Polyhedron<PFP>::Polyhedron(const Polyhedron<PFP>& p1, const Polyhedron<PFP>& p2) :
	m_map(p1.m_map),
	m_dart(p1.m_dart),
	m_kind(COMPOSED),
	m_nx(-1), m_ny(-1), m_nz(-1),
	m_top_closed(false), m_bottom_closed(false),
	m_positions(p1.m_positions)
{
	if (&(p1.map) != &(p2.map))
	{
		CGoGNerr << "Warning, can not merge to Polyhedrons of different maps"<< CGoGNendl;
	}

	m_tableVertDarts.reserve(p1.m_tableVertDarts.size() + p2.m_tableVertDarts.size()); // can be too much but ...

	typename PFP::VEC3 center(0);

	for(typename std::vector<Dart>::const_iterator di = p1.m_tableVertDarts.begin(); di != p1.m_tableVertDarts.end(); ++di)
	{
		m_tableVertDarts.push_back(*di);
		center += m_positions[*di];
	}

	// O(n2) pas terrible !!
	for(typename std::vector<Dart>::const_iterator di = p2.m_tableVertDarts.begin(); di != p2.m_tableVertDarts.end(); ++di)
	{
		unsigned int em = m_map.getEmbedding(*di, VERTEX);

		typename std::vector<Dart>::const_iterator dj=p1.m_tableVertDarts.begin();
		bool found = false;
		while ((dj !=p1.m_tableVertDarts.end()) && (!found))
		{
			unsigned int xm = m_map.getEmbedding(*dj, VERTEX);
			if (xm == em)
				found = true;
			else
				++dj;
		}
		if (!found)
		{
			m_tableVertDarts.push_back(*di);
			center += m_positions[*di];
		}
	}

	m_center = center / typename PFP::REAL(m_tableVertDarts.size());
}

template <typename PFP>
Dart Polyhedron<PFP>::grid_topo(unsigned int x, unsigned int y)
{
	if (m_kind != NONE) return m_dart;

	m_kind = GRID;
	m_nx = x;
	m_ny = y;
	// nb vertices
	int nb = (x+1)*(y+1);

	// vertice reservation
	m_tableVertDarts.reserve(nb);

	// creation of quads and storing vertices
	for (unsigned int i = 0; i < y; ++i)
	{
		for (unsigned int j = 1; j <= x; ++j)
		{
			Dart d = m_map.newFace(4,false);
			m_tableVertDarts.push_back(d);
			if (j == x)
				m_tableVertDarts.push_back(m_map.phi1(d));
		}
	}

	// store last row of vertices
	for (unsigned int i = 0; i < x; ++i)
	{
		m_tableVertDarts.push_back( m_map.phi_1(m_tableVertDarts[(y-1)*(x+1) + i]) );	
	}
	m_tableVertDarts.push_back( m_map.phi1(m_tableVertDarts[(y-1)*(x+1) +x]) );	

	//sewing the quads
	for (unsigned int i = 0; i < y; ++i)
	{
		for (unsigned int j = 0; j < x; ++j)
		{
			if (i > 0) // sew with preceeding row
			{
				int pos = i*(x+1)+j;
				Dart d = m_tableVertDarts[pos];
				Dart e = m_tableVertDarts[pos-(x+1)];
				e = m_map.phi1(m_map.phi1(e));
				m_map.sewFaces(d,e,false);
			}
			if (j > 0) // sew with preceeding column
			{
				int pos = i*(x+1)+j;
				Dart d = m_tableVertDarts[pos];
				d = m_map.phi_1(d);
				Dart e = m_tableVertDarts[pos-1];
				e = m_map.phi1(e);
				m_map.sewFaces(d,e,false);
			}
		}
	}

	// store & return reference dart
	m_dart = m_tableVertDarts[0]; // util ?

	m_map.closeHole(m_dart) ;

	return m_dart;
}

template <typename PFP>
Dart Polyhedron<PFP>::cylinder_topo(unsigned int n, unsigned int z, bool top_closed, bool bottom_closed)
{
	if (m_kind != NONE) return m_dart;

	m_kind = CYLINDER;
	m_nx = n;
	m_nz = z;

	m_bottom_closed = bottom_closed;
	m_top_closed = top_closed;

	int nb = (n)*(z+1);
	if (bottom_closed) nb++;
	if (top_closed) nb++;

	// vertice reservation
	m_tableVertDarts.reserve(nb);

	// creation of quads and storing vertices
	for (unsigned int i = 0; i < z; ++i)
	{
		for (unsigned int j = 0; j < n; ++j)
		{
			Dart d = m_map.newFace(4, false);
			m_tableVertDarts.push_back(d);
		}
	}
	for (unsigned int i = 0; i < n; ++i)
	{
		m_tableVertDarts.push_back( m_map.phi_1(m_tableVertDarts[(z-1)*n+i]) );	
	}

	//sewing the quads
	for (unsigned int i = 0; i < z; ++i)
	{
		for (unsigned int j = 0; j < n; ++j)
		{
			if (i > 0) // sew with preceeding row
			{
				int pos = i*n+j;
				Dart d = m_tableVertDarts[pos];
				Dart e = m_tableVertDarts[pos-n];
				e = m_map.phi1(m_map.phi1(e));
				m_map.sewFaces(d, e, false);
			}
			if (j > 0) // sew with preceeding column
			{
				int pos = i*n+j;
				Dart d = m_tableVertDarts[pos];
				d = m_map.phi_1(d);
				Dart e = m_tableVertDarts[pos-1];
				e = m_map.phi1(e);
				m_map.sewFaces(d, e, false);
			}
			else 
			{
				int pos = i*n;
				Dart d = m_tableVertDarts[pos];
				d = m_map.phi_1(d);
				Dart e = m_tableVertDarts[pos+(n-1)];
				e = m_map.phi1(e);
				m_map.sewFaces(d, e, false);
			}
		}
	}

	if (bottom_closed)
	{
		Dart d = m_tableVertDarts[0];
		if(m_map.closeHole(d, false))
		{
			d = m_map.phi2(d);
			if(m_map.faceDegree(d) > 3)
			{
				Algo::Modelisation::trianguleFace<PFP>(m_map, d);
				m_tableVertDarts.push_back(m_map.phi_1(d));
			}
		}
	}
	else
		m_map.closeHole(m_tableVertDarts[0]);

	if (top_closed)
	{
		Dart d =  m_map.phi_1(m_tableVertDarts[n*z]);
		if(m_map.closeHole(d, false))
		{
			d = m_map.phi2(d);
			if(m_map.faceDegree(d) > 3)
			{
				Algo::Modelisation::trianguleFace<PFP>(m_map, d);
				m_tableVertDarts.push_back(m_map.phi_1(d));
			}
		}
	}
	else
		m_map.closeHole(m_map.phi_1(m_tableVertDarts[n*z]));

	m_dart = m_tableVertDarts.front();
	return m_dart;
}

// template <typename PFP>
// Dart Polyhedron<PFP>::cone_topo(int n, int z, bool bottom_closed)
// {
// 	if (m_kind != NONE) return m_dart;
// 
// 	if (z>1)
// 	{
// 		if  (n>4) // normal case
// 		{
// 			Dart d = cylinder_topo(n,z-1,true,bottom_closed);
// 			m_kind = CONE;
// 			m_nz = z;
// 			return d;
// 		}
// 		else if (n==4)
// 		{
// 			Dart d = cylinder_topo(n,z-1,true,false);
// 			m_kind = CONE;
// 			m_nz = z;
// 			// close bottom with one quad not three triangles
// 			if (bottom_closed)
// 			{
// 				Dart t= m_map.newFace(4,false);
// 				m_map.sewFaces(m_tableVertDarts[0],t);
// 				t=m_map.phi_1(t);
// 				m_map.sewFaces(m_tableVertDarts[1],t);
// 				t=m_map.phi_1(t);
// 				m_map.sewFaces(m_tableVertDarts[2],t);
// 				t=m_map.phi_1(t);
// 				m_map.sewFaces(m_tableVertDarts[3],t);
// 			}
// 			return d;
// 		}
// 		else // n==3 (base is triangle)
// 		{
// 			Dart d = cylinder_topo(n,z-1,true,false);
// 			m_kind = CONE;
// 			m_nz = z;
// 			// close bottom with one triangle not three
// 			if (bottom_closed)
// 			{
// 				Dart t= m_map.newFace(3,false);
// 				m_map.sewFaces(d,t);
// 				d=nextDV(d); t=m_map.phi_1(t);
// 				m_map.sewFaces(d,t);
// 				d=nextDV(d); t=m_map.phi_1(t);
// 				m_map.sewFaces(d,t);
// 			}
// 			return d;
// 		}
// 	}
// 	else //z==1 only on slice
// 	{
// 		m_kind = CONE;		
// 		m_nx = n;
// 		m_nz = z;
// 		if  (n>4) // normal case
// 		{
// 			if (bottom_closed)
// 				m_tableVertDarts.reserve(n+2);
// 			else
// 				m_tableVertDarts.reserve(n+1);
// 
// 			Dart ref = triangleFan_topo<PFP>(m_map,n);
// 			Dart d = ref;
// 			for (int i=0;i<n;++i)
// 			{
// 				m_tableVertDarts.push_back(d);
// 				d = nextDV(d);
// 			}
// 
// 			if (bottom_closed)
// 			{
// 				m_bottom_closed=true;
// 				Dart dd = triangleFan_topo<PFP>(m_map,n);
// 				m_tableVertDarts.push_back(m_map.phi_1(dd));
// 				for (int i=0;i<n;++i)
// 				{
// 					m_map.sewFaces(d,dd);
// 					d=nextDV(d);
// 					dd=precDV(dd);
// 				}
// 			}
// 
// 			m_tableVertDarts.push_back(m_map.phi_1(ref));
// 
// 			m_dart=ref;
// 			return ref;
// 		}
// 		else if (n==4)
// 		{
// 			m_bottom_closed=false; // because no vertex in bottom
// 			m_tableVertDarts.reserve(4);
// 
// 			Dart dd = triangleFan_topo<PFP>(m_map,4);
// 			for (int i=0;i<4;++i)
// 			{
// 				m_tableVertDarts.push_back(dd);
// 				dd = nextDV(dd);
// 			}
// 			m_tableVertDarts.push_back(m_map.phi_1(dd));
// 
// 			if (bottom_closed)
// 			{
// 				Dart t= m_map.newFace(4,false);
// 				m_map.sewFaces(m_tableVertDarts[0],t);
// 				t=m_map.phi_1(t);
// 				m_map.sewFaces(m_tableVertDarts[1],t);
// 				t=m_map.phi_1(t);
// 				m_map.sewFaces(m_tableVertDarts[2],t);
// 				t=m_map.phi_1(t);
// 				m_map.sewFaces(m_tableVertDarts[3],t);
// 			}
// 			m_dart=dd;
// 			return dd;
// 		}
// 		else // n==3 (base is triangle, here we create a tetrahedron)
// 		{
// 			m_bottom_closed=false; // because no vertex in bottom
// 			m_tableVertDarts.reserve(4);
// 
// 			Dart dd = triangleFan_topo<PFP>(m_map,3);
// 			for (int i=0;i<3;++i)
// 			{
// 				m_tableVertDarts.push_back(dd);
// 				dd = nextDV(dd);
// 			}
// 			m_tableVertDarts.push_back(m_map.phi_1(dd));
// 
// 			if (bottom_closed)
// 			{
// 				Dart t= m_map.newFace(3,false);
// 				m_map.sewFaces(m_tableVertDarts[0],t);
// 				t=m_map.phi_1(t);
// 				m_map.sewFaces(m_tableVertDarts[1],t);
// 				t=m_map.phi_1(t);
// 				m_map.sewFaces(m_tableVertDarts[2],t);
// 			}
// 			m_dart=dd;
// 			return dd;
// 		}
// 	}
// }

template <typename PFP>
Dart Polyhedron<PFP>::cube_topo(unsigned int x, unsigned int y, unsigned int z)
{
	if (m_kind != NONE) return m_dart;

	m_dart = cylinder_topo(2*(x+y), z, false, false);
	m_kind = CUBE;
	m_nx = x;
	m_ny = y;
	m_nz = z;

	int nb = 2*(x+y)*(z+1) + 2*(x-1)*(y-1);
	m_tableVertDarts.reserve(nb);
	
	// we now have the 4 sides, just need to create store and sew top & bottom
	// the top
	Polyhedron<PFP> primTop(m_map, m_positions);
	primTop.grid_topo(x,y);
	std::vector<Dart>& tableTop = primTop.getVertexDarts();

	int index_side = 2*(x+y)*z;
	for(unsigned int i = 0; i < x; ++i)
	{
		Dart d = m_map.phi_1(m_tableVertDarts[index_side++]);
		Dart e = tableTop[i];
		m_map.sewFaces(d, e, false);
	}
	for(unsigned int i = 0; i < y; ++i)
	{
		Dart d = m_map.phi_1(m_tableVertDarts[index_side++]);
		Dart e = tableTop[x+i*(x+1)];
		m_map.sewFaces(d, e, false);
	}
	for(unsigned int i = 0; i < x; ++i)
	{
		Dart d = m_map.phi_1(m_tableVertDarts[index_side++]);
		Dart e = tableTop[(x+1)*(y+1)-2 - i];
		e = m_map.phi_1(e);
		m_map.sewFaces(d, e, false);
	}
	for(unsigned int i = 0; i < y; ++i)
	{
		Dart d = m_map.phi_1(m_tableVertDarts[index_side++]);
		Dart e = tableTop[(y-1-i)*(x+1)];
		e = m_map.phi_1(e);
		m_map.sewFaces(d, e, false);
	}

	// the bottom
	Polyhedron<PFP> primBottom(m_map,m_positions);
	primBottom.grid_topo(x,y);
	std::vector<Dart>& tableBottom = primBottom.getVertexDarts();

	index_side = 3*(x+y)+(x-1);
	for(unsigned int i = 0; i < x; ++i)
	{
		Dart d = m_tableVertDarts[(index_side--)%(2*(x+y))];
		Dart e = tableBottom[i];
		m_map.sewFaces(d, e, false);
	}
	for(unsigned int i = 0; i < y; ++i)
	{
		Dart d = m_tableVertDarts[(index_side--)%(2*(x+y))];
		Dart e = tableBottom[x+i*(x+1)];
		m_map.sewFaces(d, e, false);
	}
	for(unsigned int i = 0; i < x; ++i)
	{
		Dart d = m_tableVertDarts[(index_side--)%(2*(x+y))];
		Dart e = tableBottom[(x+1)*(y+1)-2 - i];
		e = m_map.phi_1(e);
		m_map.sewFaces(d, e, false);
	}
	for(unsigned int i = 0; i < y; ++i)
	{
		Dart d = m_tableVertDarts[(index_side--)%(2*(x+y))];
		Dart e = tableBottom[(y-1-i)*(x+1)];
		e = m_map.phi_1(e);
		m_map.sewFaces(d, e, false);
	}

	// and add new vertex in m_tableVertDarts
	//top  first
	for(unsigned int i = 1; i < y; ++i)
	{
		for(unsigned int j = 1; j < x; ++j)
			m_tableVertDarts.push_back(tableTop[i*(x+1)+j]);
	}

	// then bottom
	for(unsigned int i = 1; i < y; ++i)
	{
		for(unsigned int j = 1; j < x; ++j)
			m_tableVertDarts.push_back(tableBottom[i*(x+1)+j]);
	}

	return m_dart;
}

template <typename PFP>
Dart Polyhedron<PFP>::tore_topo(unsigned int m, unsigned int n)
{
	if (m_kind != NONE) return m_dart;

	m_dart = cylinder_topo(n, m, false, false);
	m_nx = n;
	m_ny = m;
	m_kind = TORE;

	// just finish to sew
	for(unsigned int i = 0; i < n; ++i)
	{
		Dart d = m_tableVertDarts[i];
		Dart e = m_tableVertDarts[(m*n)+i];
		e = m_map.phi_1(e);
		m_map.sewFaces(d, e);
	}

	// remove the last n vertex darts that are no more necessary (sewed with n first)
	// memory not freed (but will be when destroy the Polyhedron), not important ??
	m_tableVertDarts.resize(m*n); 

	return m_dart;
}

template <typename PFP>
void Polyhedron<PFP>::embedGrid(float x, float y, float z)
{
	typedef typename PFP::VEC3 VEC3 ;

	if (m_kind != GRID)
	{
		CGoGNerr << "Warning try to embedGrid something that is not a grid"<<CGoGNendl;
		return;
	}

	float dx = x/float(m_nx);
	float dy = y/float(m_ny);

	for(unsigned int i = 0; i <= m_ny; ++i)
	{
		for(unsigned int j = 0; j <= m_nx;++j)
		{
			VEC3 pos(-x/2 + dx*float(j), -y/2 + dy*float(i), z);
			unsigned int em = m_positions.insert(pos);
			Dart d = m_tableVertDarts[i*(m_nx+1)+j];
			m_map.embedOrbit<VERTEX>(d, em);
		}
	}
}

template <typename PFP>
void Polyhedron<PFP>::embedCylinder(float bottom_radius, float top_radius, float height)
{
	typedef typename PFP::VEC3 VEC3 ;

	if (m_kind != CYLINDER)
	{
		CGoGNerr << "Warning try to embedCylinder something that is not a cylnder"<<CGoGNendl;
		return;
	}

	float alpha = float(2.0*M_PI/m_nx);
	float dz = height/float(m_nz);

	for(unsigned int i = 0; i <= m_nz; ++i)
	{
		float a = float(i)/float(m_nz);
		float radius = a*top_radius + (1.0f-a)*bottom_radius;
		for(unsigned int j = 0; j < m_nx; ++j)
		{
	
			float x = radius*cos(alpha*float(j));
			float y = radius*sin(alpha*float(j));
			VEC3 pos(x, y, -height/2 + dz*float(i));
			unsigned int em = m_positions.insert(pos);
			Dart d = m_tableVertDarts[i*(m_nx)+j];
			m_map.embedOrbit<VERTEX>(d, em);
		}
	}

	int indexUmbrella = m_nx*(m_nz+1);
	if (m_bottom_closed)
	{
		VEC3 pos(0.0f, 0.0f, -height/2 );
		unsigned int em = m_positions.insert(pos);
		Dart d = m_tableVertDarts[indexUmbrella++];
		m_map.embedOrbit<VERTEX>(d, em);
	}

	if (m_top_closed)
	{
		VEC3 pos(0.0f ,0.0f, height/2 );
		unsigned int em = m_positions.insert(pos);
		Dart d = m_tableVertDarts[indexUmbrella];
		m_map.embedOrbit<VERTEX>(d, em);
	}
}

template <typename PFP>
void Polyhedron<PFP>::embedCone(float radius, float height)
{
	typedef typename PFP::VEC3 VEC3 ;

	if (m_kind != CONE)
	{
		CGoGNerr << "Warning try to embedCone something that is not a cone"<<CGoGNendl;
		return;
	}

	float alpha = float(2.0*M_PI/m_nx);
	int zcyl = m_nz-1;
	float dz = height/float(m_nz);
	
	for(unsigned int i = 0; i <= zcyl; ++i)
	{
		for(unsigned int j = 0; j < m_nx; ++j)
		{
			float rad = radius * float(m_nz-i)/float(m_nz);
			float h = -height/2 + dz*float(i);

			float x = rad*cos(alpha*float(j));
			float y = rad*sin(alpha*float(j));

			VEC3 pos(x, y, h);
			unsigned int em = m_positions.insert(pos);
			Dart d = m_tableVertDarts[i*(m_nx)+j];
			m_map.embedOrbit<VERTEX>(d, em);
		}
	}

	int indexUmbrella = m_nx*(m_nz);
	if (m_bottom_closed)
	{
		VEC3 pos(0.0f, 0.0f, -height/2 );
		unsigned int em = m_positions.insert(pos);
		Dart d = m_tableVertDarts[indexUmbrella++];
		m_map.embedOrbit<VERTEX>(d, em);
	}

	//  top always closed in cone 
	VEC3 pos(0.0f ,0.0f, height/2.0f );
	unsigned int em = m_positions.insert(pos);
	Dart d = m_tableVertDarts[indexUmbrella];
	m_map.embedOrbit<VERTEX>(d, em);
}

template <typename PFP>
void Polyhedron<PFP>::embedSphere(float radius)
{
	typedef typename PFP::VEC3 VEC3 ;

	if (!((m_kind == CYLINDER) && (m_top_closed) && (m_bottom_closed)))
	{
		CGoGNerr << "Warning try to embedSphere something that is not a sphere (closed cylinder)"<<CGoGNendl;
		return;
	}

	float alpha = float(2.0*M_PI/m_nx);
	float beta = float(M_PI/(m_nz+2));

	for(unsigned int i = 0; i <= m_nz; ++i)
	{
		for(unsigned int j = 0; j < m_nx; ++j)
		{
			float h = float(radius * sin(-M_PI/2.0+(i+1)*beta));
			float rad = float(radius * cos(-M_PI/2.0+(i+1)*beta));

			float x = rad*cos(alpha*float(j));
			float y = rad*sin(alpha*float(j));
			VEC3 pos(x, y, h );
			unsigned int em = m_positions.insert(pos);
			Dart d = m_tableVertDarts[i*(m_nx)+j];
			m_map.embedOrbit<VERTEX>(d, em);
		}
	}

	// bottom  pole
	VEC3 pos(0.0f, 0.0f, -radius);
	unsigned int em = m_positions.insert(pos);
	Dart d = m_tableVertDarts[m_nx*(m_nz+1)];
	m_map.embedOrbit<VERTEX>(d, em);

	//  top pole
	pos = VEC3(0.0f, 0.0f, radius);
	em = m_positions.insert(pos);
	d = m_tableVertDarts[m_nx*(m_nz+1)+1];
	m_map.embedOrbit<VERTEX>(d, em);
}

template <typename PFP>
void Polyhedron<PFP>::embedTore(float big_radius, float small_radius)
{
	typedef typename PFP::VEC3 VEC3 ;

	if (m_kind != TORE)
	{
		CGoGNerr << "Warning try to embedTore something that is not a tore"<<CGoGNendl;
		return;
	}

	float alpha = float(2.0*M_PI/m_nx);
	float beta = float(2.0*M_PI/m_ny);

	for (unsigned int i = 0; i < m_nx; ++i)
	{
		for(unsigned int j = 0; j < m_ny; ++j)
		{
			float z = small_radius*sin(beta*float(j));
			float r = big_radius + small_radius*cos(beta*float(j));
			float x = r*cos(alpha*float(i));
			float y = r*sin(alpha*float(i));
			VEC3 pos(x, y, z);
			unsigned int em = m_positions.insert(pos);
			Dart d = m_tableVertDarts[j*(m_nx)+i];
			m_map.embedOrbit<VERTEX>(d, em);
		}
	}
}

template <typename PFP>
void Polyhedron<PFP>::embedCube(float sx, float sy, float sz)
{
	typedef typename PFP::VEC3 VEC3 ;

	if (m_kind != CUBE)
	{
		CGoGNerr << "Warning try to embedCube something that is not a cube"<<CGoGNendl;
		return;
	}

	float dz = sz/float(m_nz);
	float dy = sy/float(m_ny);
	float dx = sx/float(m_nx);

	// first embedding the sides
	int index = 0;
	for (unsigned int k = 0; k <= m_nz; ++k)
	{
		float z = float(k)*dz - sz/2.0f;
		for (unsigned int i = 0; i < m_nx; ++i)
		{
			float x = float(i)*dx - sx/2.0f;
			VEC3 pos(x, -sy/2.0f, z);
			unsigned int em = m_positions.insert(pos);
			Dart d = m_tableVertDarts[index++];
			m_map.embedOrbit<VERTEX>(d, em);
		}
		for (unsigned int i = 0; i < m_ny; ++i)
		{
			float y = float(i)*dy - sy/2.0f;
			VEC3 pos(sx/2.0f, y, z);
			unsigned int em = m_positions.insert(pos);
			Dart d = m_tableVertDarts[index++];
			m_map.embedOrbit<VERTEX>(d, em);
		}
		for (unsigned int i = 0; i < m_nx; ++i)
		{
			float x = sx/2.0f-float(i)*dx;
			VEC3 pos(x, sy/2.0f, z);
			unsigned int em = m_positions.insert(pos);
			Dart d = m_tableVertDarts[index++];
			m_map.embedOrbit<VERTEX>(d, em);
		}
		for (unsigned int i = 0; i < m_ny ;++i)
		{
			float y = sy/2.0f - float(i)*dy;
			VEC3 pos(-sx/2.0f, y, z);
			unsigned int em = m_positions.insert(pos);
			Dart d = m_tableVertDarts[index++];
			m_map.embedOrbit<VERTEX>(d, em);
		}
	}

	// the top
	for(unsigned int i = 1; i  <m_ny; ++i)
	{
		for(unsigned int j = 1; j < m_nx; ++j)
		{
			VEC3 pos(-sx/2.0f+float(j)*dx, -sy/2.0f+float(i)*dy, sz/2.0f);
			unsigned int em = m_positions.insert(pos);
			Dart d = m_tableVertDarts[index++];
			m_map.embedOrbit<VERTEX>(d, em);
		}
	}

	// the bottom
	for(unsigned int i = 1; i < m_ny; ++i)
	{
		for(unsigned int j = 1; j < m_nx; ++j)
		{
			VEC3 pos(-sx/2.0f+float(j)*dx, sy/2.0f-float(i)*dy, -sz/2.0f);
			unsigned int em = m_positions.insert(pos);
			Dart d = m_tableVertDarts[index++];
			m_map.embedOrbit<VERTEX>(d, em);
		}
	}
}

template <typename PFP>
void Polyhedron<PFP>::computeCenter()
{
	typename PFP::VEC3 center(0);

	for(typename std::vector<Dart>::iterator di = m_tableVertDarts.begin(); di != m_tableVertDarts.end(); ++di)
	{
		center += m_positions[*di];
	}

	m_center = center / typename PFP::REAL(m_tableVertDarts.size());
}

template <typename PFP>
//void Polyhedron<PFP>::transform(float* matrice)
void Polyhedron<PFP>::transform(const Geom::Matrix44f& matrice)
{
//	Geom::Vec4f v1(matrice[0],matrice[4],matrice[8], matrice[12]);
//	Geom::Vec4f v2(matrice[1],matrice[5],matrice[9], matrice[13]);
//	Geom::Vec4f v3(matrice[2],matrice[6],matrice[10],matrice[14]);
//	Geom::Vec4f v4(matrice[3],matrice[7],matrice[11],matrice[15]);

	for(typename std::vector<Dart>::iterator di = m_tableVertDarts.begin(); di != m_tableVertDarts.end(); ++di)
	{

		typename PFP::VEC3& pos = m_positions[*di];
//
//		Geom::Vec4f VA(pos[0],pos[1],pos[2],1.0f);
//
//		Geom::Vec4f VB((VA*v1),(VA*v2),(VA*v3),(VA*v4));
//		VEC3 newPos(VB[0]/VB[3],VB[1]/VB[3],VB[2]/VB[3]);
		
		pos = Geom::transform(pos, matrice);
	}
	// transform the center
	m_center = Geom::transform(m_center, matrice);

}

template <typename PFP>
void Polyhedron<PFP>::mark(CellMarker& m)
{
	for(typename std::vector<Dart>::iterator di = m_tableVertDarts.begin(); di != m_tableVertDarts.end(); ++di)
	{
		m.mark(*di);
	}
}

//template <typename PFP>
//void Polyhedron<PFP>::markEmbVertices(Mark m)
//{
//	AttributeHandler<Mark> markers(VERTEX << 24, m_map);
//	for(typename std::vector<Dart>::iterator di = m_tableVertDarts.begin(); di != m_tableVertDarts.end(); ++di)
//	{
//		markers[*di].setMark(m);
//	}
//}

template <typename PFP>
void Polyhedron<PFP>::embedTwistedStrip(float radius_min,  float radius_max, float turns)
{
	typedef typename PFP::VEC3 VEC3 ;

	float alpha = float(2.0*M_PI/m_ny);
	float beta = turns/float(m_ny);

	float radius = (radius_max + radius_min)/2.0f;
	float rdiff = (radius_max - radius_min)/2.0f;

	for(unsigned int i = 0; i <= m_ny; ++i)
	{
		for(unsigned int j = 0; j <= m_nx; ++j)
		{
			float rw = -rdiff + float(j)*2.0f*rdiff/float(m_nx);
			float r = radius + rw*cos(beta*float(i));
			VEC3 pos(r*cos(alpha*float(i)), r*sin(alpha*float(i)), rw*sin(beta*float(i)));
			unsigned int em = m_positions.insert(pos);
			Dart d = m_tableVertDarts[i*(m_nx+1)+j];
			m_map.embedOrbit<VERTEX>(d, em);
		}
	}
}

template <typename PFP>
void Polyhedron<PFP>::embedHelicoid(float radius_min, float radius_max, float maxHeight, float nbTurn, int orient)
{
	typedef typename PFP::VEC3 VEC3 ;

	float alpha = float(2.0*M_PI/m_nx)*nbTurn;
	float hS = maxHeight/m_nx;

// 	float radius = (radius_max + radius_min)/2.0f;
// 	float rdiff = (radius_max - radius_min)/2.0f;

	for(unsigned int i = 0; i <= m_ny; ++i)
	{
		for(unsigned int j = 0; j <= m_nx; ++j)
		{
// 			float r = radius_max + radius_min*cos(beta*float(j));
			float r,x,y;
// 			if(i==1) {
// 				r = radius_max;
// 			}
// 			else {
				r = radius_min+(radius_max-radius_min)*float(i)/float(m_ny);
// 			}
			x = orient*r*sin(alpha*float(j));
			y = orient*r*cos(alpha*float(j));

			VEC3 pos(x, y, j*hS);
			Dart d = m_tableVertDarts[i*(m_nx+1)+j];
			m_positions[d]=pos;
		}
	}
}

// template <typename PFP>
// void onlyTriangles(typename PFP::MAP& the_map, Dart primd)
// {
// 	DartMarker m(the_map);
// 
// 	// list of faces to process and processed(before pos iterator)
// 	std::list<Dart> ld;
// 	ld.push_back(primd);
// 	// current position in list
// 	typename std::list<Dart>::iterator pos = ld.begin();
// 	do
// 	{
// 		Dart d = *pos;
// 		
// 		// cut the face of first dart of list
// 		Dart d1 = the_map.phi1(d);
// 		Dart e = the_map.phi1(d1);
// 		Dart e1 = the_map.phi1(e);
// 		Dart f = the_map.phi1(e1);
// 		m.markOrbit<FACE>(d);
// 		if (f==d) // quad
// 		{
// 			Dart n = the_map.cutFace(d,e);
// 			Dart nn = the_map.phi2(n);
// 			// mark the face
// 			m.mark(n);
// 			m.mark(nn);
// 		}
// 
// 		// and store neighbours faces in the list
// 		d = the_map.phi2(d);
// 		e = the_map.phi2(e);
// 		d1 = the_map.phi1(the_map.phi2(d1));
// 		e1 = the_map.phi1(the_map.phi2(e1));
// 
// 		if (!m.isMarked(d))
// 			ld.push_back(d);
// 		if (!m.isMarked(e))
// 			ld.push_back(e);
// 		if (!m.isMarked(d1))
// 			ld.push_back(d1);
// 		if ((f==d) && (!m.isMarked(e1)))
// 			ld.push_back(e1);
// 		pos++;
// 	}while (pos!=ld.end()); // stop when no more face to process
// }

// template <typename PFP>
// Dart triangleFan_topo(typename PFP::MAP& the_map, int n)
// {
// 	Dart d = the_map.newFace(3,false);
// 	Dart e = the_map.phi1(d);
// 	for(int i=1;i<n;++i)
// 	{
// 		Dart f = the_map.newFace(3,false);
// 		the_map.sewFaces(the_map.phi_1(f),e);
// 		e = the_map.phi1(f);
// 	}
// 	the_map.sewFaces(the_map.phi_1(d),e);
// 	return d;
// }

} // namespace Modelisation

} // namespace Algo

} // namespace CGoGN
