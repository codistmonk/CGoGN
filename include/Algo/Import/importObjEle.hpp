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

#include "Algo/Modelisation/polyhedron.h"
#include <vector>

namespace CGoGN
{

namespace Algo
{

namespace Import
{

template<typename PFP>
bool importOFFWithELERegions(typename PFP::MAP& map, const std::string& filenameOFF, const std::string& filenameELE, std::vector<std::string>& attrNames)
{
	typedef typename PFP::VEC3 VEC3;

	AttributeHandler<VEC3> position = map.template addAttribute<VEC3>(VERTEX, "position") ;
	attrNames.push_back(position.name()) ;

	AttributeContainer& container = map.getAttributeContainer(VERTEX) ;

	unsigned int m_nbVertices = 0, m_nbFaces = 0, m_nbEdges = 0, m_nbVolumes = 0;

	AutoAttributeHandler<  NoMathIONameAttribute< std::vector<Dart> > > vecDartsPerVertex(map, VERTEX, "incidents");

	// open files
	std::ifstream foff(filenameOFF.c_str(), std::ios::in);
	if (!foff.good())
	{
		CGoGNerr << "Unable to open OFF file " << CGoGNendl;
		return false;
	}

	std::ifstream fele(filenameELE.c_str(), std::ios::in);
	if (!fele.good())
	{
		CGoGNerr << "Unable to open ELE file " << CGoGNendl;
		return false;
	}

	std::string line;

	//OFF reading
	std::getline(foff, line);
	if(line.rfind("OFF") == std::string::npos)
	{
		CGoGNerr << "Problem reading off file: not an off file"<<CGoGNendl;
		CGoGNerr << line << CGoGNendl;
		return false;
	}

	//Reading number of vertex/faces/edges in OFF file
	unsigned int nbe;
	{
		do
		{
			std::getline(foff,line);
		}while(line.size() == 0);

		std::stringstream oss(line);
		oss >> m_nbVertices;
		oss >> m_nbFaces;
		oss >> m_nbEdges;
		oss >> nbe;
	}

	//Reading number of tetrahedra in ELE file
	unsigned int nbv;
	{
		do
		{
			std::getline(fele,line);
		}while(line.size() == 0);

		std::stringstream oss(line);
		oss >> m_nbVolumes;
		oss >> nbv ; oss >> nbv;
	}

	CGoGNout << "nb points = " << m_nbVertices << " / nb faces = " << m_nbFaces << " / nb edges = " << m_nbEdges << " / nb tet = " << m_nbVolumes << CGoGNendl;

	//Reading vertices
	std::vector<unsigned int> verticesID;
	verticesID.reserve(m_nbVertices);

	for(unsigned int i = 0 ; i < m_nbVertices ; ++i)
	{
		do
		{
			std::getline(foff,line);
		}while(line.size() == 0);

		std::stringstream oss(line);

		float x,y,z;
		oss >> x;
		oss >> y;
		oss >> z;
		//we can read colors informations if exists
		VEC3 pos(x,y,z);

		unsigned int id = container.insertLine();
		position[id] = pos;

//		CGoGNout << "emb : " << pos << " / id = " << id << CGoGNendl;
		verticesID.push_back(id);
	}

	std::vector<std::vector<Dart> > vecDartPtrEmb;
	vecDartPtrEmb.reserve(m_nbVertices);

	//Read and embed tetrahedra TODO
	for(unsigned i = 0; i < m_nbVolumes ; ++i)
	{
		do
		{
			std::getline(fele,line);
		} while(line.size() == 0);

		std::stringstream oss(line);
		oss >> nbe;
//		CGoGNout << "tetra number : " << nbe << CGoGNendl;

		//Algo::Modelisation::Polyhedron<PFP>::createOrientedTetra(map);
		Dart d = Algo::Modelisation::Polyhedron<PFP>::createOrientedPolyhedron(map, 4);
		Geom::Vec4ui pt;
		oss >> pt[0];
		oss >> pt[1];
		oss >> pt[2];
		oss >> pt[3];

		//regions ?
		oss >> nbe;

//		CGoGNout << "\t embedding number : " << pt[0] << " " << pt[1] << " " << pt[2] << " " << pt[3] << CGoGNendl;

		// Embed three vertices
		for(unsigned int j = 0 ; j < 3 ; ++j)
		{
//			CGoGNout << "\t embedding number : " << pt[j];

			FunctorSetEmb<typename PFP::MAP> femb(map, VERTEX, verticesID[pt[j]]);

			Dart dd = d;
			do {
				femb(dd);
				//vecDartPtrEmb[pt[j]].push_back(dd);
				vecDartsPerVertex[pt[j]].push_back(dd);
				dd = map.phi1(map.phi2(dd));
			} while(dd!=d);

			d = map.phi1(d);

//			CGoGNout << " done" << CGoGNendl;
		}

		//Embed the last vertex
//		CGoGNout << "\t embedding number : " << pt[3] << CGoGNendl;
		d = map.phi_1(map.phi2(d));

		FunctorSetEmb<typename PFP::MAP> femb(map, VERTEX, verticesID[pt[3]]);
		Dart dd = d;
		do {
			femb(dd);
//			CGoGNout << "embed" << CGoGNendl;
			//vecDartPtrEmb[pt[3]].push_back(dd);
			vecDartsPerVertex[pt[3]].push_back(dd);
			dd = map.phi1(map.phi2(dd));
		} while(dd != d);

//		CGoGNout << "end tetra" << CGoGNendl;
	}

//	CGoGNout << "end 1/2" << CGoGNendl;

	foff.close();
	fele.close();

	//Association des phi3
	for (Dart d = map.begin(); d != map.end(); map.next(d))
	{
		std::vector<Dart>& vec = vecDartsPerVertex[d];

		for(typename std::vector<Dart>::iterator it = vec.begin(); it != vec.end(); ++it)
		{
			if(map.phi3(*it)==*it)
			{
				bool sewn=false;
				for(typename std::vector<Dart>::iterator itnext = it+1; itnext != vec.end() && !sewn; ++itnext)
				{
					if(map.getDartEmbedding(VERTEX,map.phi1(*it))==map.getDartEmbedding(VERTEX,map.phi_1(*itnext))
					&& map.getDartEmbedding(VERTEX,map.phi_1(*it))==map.getDartEmbedding(VERTEX,map.phi1(*itnext)))
					{
						map.sewVolumes(*it,map.phi_1(*itnext));
						sewn = true;
					}
				}
			}
		}
	}

//	CGoGNout << "end 2/2" << CGoGNendl;

	return true;
}

} // namespace Import

} // namespace Algo

} // namespace CGoGN
