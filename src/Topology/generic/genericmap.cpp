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

#include "Topology/generic/genericmap.h"
#include "Topology/generic/attributeHandler.h"
#include "Topology/generic/dartmarker.h"
#include "Topology/generic/traversorCell.h"

#include "Geometry/vector_gen.h"
#include "Geometry/matrix.h"
#include "Container/registered.h"

namespace CGoGN
{

std::map<std::string, RegisteredBaseAttribute*>* GenericMap::m_attributes_registry_map = NULL ;

GenericMap::GenericMap() : m_nbThreads(1)
{
	if(m_attributes_registry_map == NULL)
		m_attributes_registry_map = new std::map<std::string, RegisteredBaseAttribute*> ;

	// register all known types
	registerAttribute<Dart>("Dart");
	registerAttribute<Mark>("Mark");

	registerAttribute<long>("long");
	registerAttribute<int>("int");
	registerAttribute<short>("short");
	registerAttribute<char>("char");

	registerAttribute<unsigned long>("unsigned long");
	registerAttribute<unsigned int>("unsigned int");
	registerAttribute<unsigned short>("unsigned short");
	registerAttribute<unsigned char>("unsigned char");

	registerAttribute<Geom::Vec2f>(Geom::Vec2f::CGoGNnameOfType());
	registerAttribute<Geom::Vec3f>(Geom::Vec3f::CGoGNnameOfType());
	registerAttribute<Geom::Vec4f>(Geom::Vec4f::CGoGNnameOfType());

	registerAttribute<Geom::Vec2d>(Geom::Vec2d::CGoGNnameOfType());
	registerAttribute<Geom::Vec3d>(Geom::Vec3d::CGoGNnameOfType());
	registerAttribute<Geom::Vec4d>(Geom::Vec4d::CGoGNnameOfType());

	registerAttribute<Geom::Matrix33f>(Geom::Matrix33f::CGoGNnameOfType());
	registerAttribute<Geom::Matrix44f>(Geom::Matrix44f::CGoGNnameOfType());

	registerAttribute<Geom::Matrix33d>(Geom::Matrix33d::CGoGNnameOfType());
	registerAttribute<Geom::Matrix44d>(Geom::Matrix44d::CGoGNnameOfType());

	for(unsigned int i = 0; i < NB_ORBITS; ++i)
	{
		m_attribs[i].setOrbit(i) ;
		m_attribs[i].setRegistry(m_attributes_registry_map) ;
		m_embeddings[i] = NULL ;
		for(unsigned int j = 0; j < NB_THREAD; ++j)
		{
			m_marksets[i][j].clear() ;
			m_markTables[i][j] = NULL ;
		}
	}

	dartMarkers.reserve(16) ;
	cellMarkers.reserve(16) ;

	// get & lock marker for boundary
	m_boundaryMarker =  m_marksets[DART][0].getNewMark();

#ifndef CGoGN_FORCE_MR
	m_isMultiRes = false;
#endif
}

GenericMap::~GenericMap()
{
	// release marker for boundary
	m_marksets[DART][0].releaseMark(m_boundaryMarker);

	for(unsigned int i = 0; i < NB_ORBITS; ++i)
	{
		if(isOrbitEmbedded(i))
			m_attribs[i].clear(true) ;
	}

	for(std::multimap<AttributeMultiVectorGen*, AttributeHandlerGen*>::iterator it = attributeHandlers.begin(); it != attributeHandlers.end(); ++it)
		(*it).second->setInvalid() ;
	attributeHandlers.clear() ;

	for(std::vector<DartMarkerGen*>::iterator it = dartMarkers.begin(); it != dartMarkers.end(); ++it)
		(*it)->setReleaseOnDestruct(false) ;
	dartMarkers.clear() ;

	for(std::vector<CellMarkerGen*>::iterator it = cellMarkers.begin(); it != cellMarkers.end(); ++it)
		(*it)->setReleaseOnDestruct(false) ;
	cellMarkers.clear() ;

	if(m_attributes_registry_map)
	{
		delete m_attributes_registry_map;
		m_attributes_registry_map = NULL;
	}
}

void GenericMap::initMR()
{
#ifndef CGoGN_FORCE_MR
	m_isMultiRes = true;
#endif

	m_mrattribs.clear(true) ;
	m_mrDarts.clear() ;
	m_mrDarts.reserve(16) ;
	m_mrLevelStack.clear() ;
	m_mrLevelStack.reserve(16) ;

	m_mrLevels = m_mrattribs.addAttribute<unsigned int>("MRLevel") ;
	addLevel() ;
	setCurrentLevel(0) ;
}

void GenericMap::clear(bool removeAttrib)
{
	if (removeAttrib)
	{
		for(unsigned int i = 0; i < NB_ORBITS; ++i)
		{
			m_attribs[i].clear(true) ;
			m_embeddings[i] = NULL ;
		}
		for(std::multimap<AttributeMultiVectorGen*, AttributeHandlerGen*>::iterator it = attributeHandlers.begin(); it != attributeHandlers.end(); ++it)
			(*it).second->setInvalid() ;
		attributeHandlers.clear() ;
	}
	else
	{
		for(unsigned int i = 0; i < NB_ORBITS; ++i)
		{
			m_attribs[i].clear(false) ;
		}
	}

	if (m_isMultiRes)
		initMR() ;
}

/****************************************
 *           MULTIRES                   *
 ****************************************/

void GenericMap::addLevel()
{
	unsigned int newLevel = m_mrDarts.size() ;
	std::stringstream ss ;
	ss << "MRdart_"<< newLevel ;
	AttributeMultiVector<unsigned int>* amvMR = m_mrattribs.addAttribute<unsigned int>(ss.str()) ;

	m_mrDarts.push_back(amvMR) ;

	// copy the darts pointers of the previous level
	if(m_mrDarts.size() > 1)
		m_mrattribs.copyAttribute(amvMR->getIndex(), m_mrDarts[newLevel - 1]->getIndex()) ;

	// duplicate all the darts in the new level
	for(unsigned int i = m_mrattribs.begin(); i != m_mrattribs.end(); m_mrattribs.next(i))
	{
		unsigned int oldi = amvMR->operator[](i) ;
		unsigned int newi = m_attribs[DART].insertLine() ;
		m_attribs[DART].copyLine(newi, oldi) ;
		amvMR->operator[](i) = newi ;
		for(unsigned int t = 0; t < m_nbThreads; ++t)
			m_markTables[DART][t]->operator[](newi).clear() ;
	}
}

/****************************************
 *         EMBEDDING MANAGEMENT         *
 ****************************************/

void GenericMap::setDartEmbedding(unsigned int orbit, Dart d, unsigned int emb)
{
	assert(isOrbitEmbedded(orbit) || !"Invalid parameter: orbit not embedded");
	unsigned int old = getEmbedding(orbit, d);

	// if same emb nothing to do
	if (old == emb)
		return;
	// if different then unref the old emb
	if (old != EMBNULL)
	{
		if(m_attribs[orbit].unrefLine(old))
		{
			for (unsigned int t = 0; t < m_nbThreads; ++t)
				m_markTables[orbit][t]->operator[](old).clear();
		}
	}
	// ref the new emb
	if (emb != EMBNULL)
		m_attribs[orbit].refLine(emb);
	// affect the embedding to the dart
	(*m_embeddings[orbit])[dartIndex(d)] = emb ;
}

/****************************************
 *        ATTRIBUTES MANAGEMENT         *
 ****************************************/

void GenericMap::swapEmbeddingContainers(unsigned int orbit1, unsigned int orbit2)
{
	assert(orbit1 != orbit2 || !"Cannot swap a container with itself") ;
	assert((orbit1 != DART && orbit2 != DART) || !"Cannot swap the darts container") ;

	m_attribs[orbit1].swap(m_attribs[orbit2]) ;
	m_attribs[orbit1].setOrbit(orbit1) ;	// to update the orbit information
	m_attribs[orbit2].setOrbit(orbit2) ;	// in the contained AttributeMultiVectors

	m_embeddings[orbit1]->swap(m_embeddings[orbit2]) ;

	for(unsigned int t = 0; t < m_nbThreads; ++t)
	{
		AttributeMultiVector<Mark>* m = m_markTables[orbit1][t] ;
		m_markTables[orbit1][t] = m_markTables[orbit2][t] ;
		m_markTables[orbit2][t] = m ;

		MarkSet ms = m_marksets[orbit1][t] ;
		m_marksets[orbit1][t] = m_marksets[orbit2][t] ;
		m_marksets[orbit2][t] = ms ;
	}

	for(std::vector<CellMarkerGen*>::iterator it = cellMarkers.begin(); it != cellMarkers.end(); ++it)
	{
		if((*it)->m_cell == orbit1)
			(*it)->m_cell = orbit2 ;
		else if((*it)->m_cell == orbit2)
			(*it)->m_cell = orbit1 ;
	}
}

template <typename R>
bool GenericMap::registerAttribute(const std::string &nameType)
{
	RegisteredBaseAttribute* ra = new RegisteredAttribute<R>;
	if (ra == NULL)
	{
		CGoGNerr << "Erreur enregistrement attribut" << CGoGNendl;
		return false;
	}

	ra->setTypeName(nameType);

	m_attributes_registry_map->insert(std::pair<std::string, RegisteredBaseAttribute*>(nameType,ra));
	return true;
}

void GenericMap::initOrbitEmbedding(unsigned int orbit, bool realloc)
{
	assert(isOrbitEmbedded(orbit) || !"Invalid parameter: orbit not embedded") ;
	DartMarker mark(*this) ;
	for(Dart d = begin(); d != end(); next(d))
	{
		if(!mark.isMarked(d))
		{
			mark.markOrbit(orbit, d) ;
			if(realloc || getEmbedding(orbit, d) == EMBNULL)
				embedNewCell(orbit, d) ;
		}
	}
}

/****************************************
 *   EMBEDDING ATTRIBUTES MANAGEMENT    *
 ****************************************/

void GenericMap::addEmbedding(unsigned int orbit)
{
	assert(!isOrbitEmbedded(orbit) || !"Invalid parameter: orbit already embedded") ;

	std::ostringstream oss;
	oss << "EMB_" << orbit;

	AttributeContainer& dartCont = m_attribs[DART] ;
	AttributeMultiVector<unsigned int>* amv = dartCont.addAttribute<unsigned int>(oss.str()) ;
	m_embeddings[orbit] = amv ;

	// set new embedding to EMBNULL for all the darts of the map
	for(unsigned int i = dartCont.begin(); i < dartCont.end(); dartCont.next(i))
		amv->operator[](i) = EMBNULL ;
}

/****************************************
 *          THREAD MANAGEMENT           *
 ****************************************/

void GenericMap::addThreadMarker(unsigned int nb)
{
	unsigned int th ;

	for (unsigned int j = 0; j < nb; ++j)
	{
		th = m_nbThreads ;
		m_nbThreads++ ;

		for (unsigned int i = 0; i < NB_ORBITS; ++i)
		{
			std::stringstream ss ;
			ss << "Mark_"<< th ;
			AttributeContainer& cellCont = m_attribs[i] ;
			AttributeMultiVector<Mark>* amvMark = cellCont.addAttribute<Mark>(ss.str()) ;
			m_markTables[i][th] = amvMark ;
		}
	}
}

unsigned int GenericMap::getNbThreadMarkers()
{
	return m_nbThreads;
}

void GenericMap::removeThreadMarker(unsigned int nb)
{
	unsigned int th = 0;
	while ((m_nbThreads > 1) && (nb > 0))
	{
		th = --m_nbThreads ;
		--nb;
		for (unsigned int i = 0; i < NB_ORBITS; ++i)
		{
			std::stringstream ss ;
			ss << "Mark_"<< th ;
			AttributeContainer& cellCont = m_attribs[i] ;
			cellCont.removeAttribute<Mark>(ss.str()) ;
			m_markTables[i][th] = NULL ;
		}
	}
}

/****************************************
 *             SAVE & LOAD              *
 ****************************************/

bool GenericMap::saveMapBin(const std::string& filename)
{
	CGoGNostream fs(filename.c_str(), std::ios::out|std::ios::binary);
	if (!fs)
	{
		CGoGNerr << "Unable to open file for writing: " << filename << CGoGNendl;
		return false;
	}

	// Entete
	char* buff = new char[256];
	for (int i = 0; i < 256; ++i)
		buff[i] = char(255);
	if (m_isMultiRes)
	{
		const char* cgogn = "CGoGN_MRMap";
		memcpy(buff, cgogn, 12);
	}
	else
	{
		const char* cgogn = "CGoGN_Map";
		memcpy(buff, cgogn, 10);
	}
	std::string mt = mapTypeName();
	const char* mtc = mt.c_str();
	memcpy(buff+32, mtc, mt.size()+1);
	unsigned int *buffi = reinterpret_cast<unsigned int*>(buff + 64);
	*buffi = NB_ORBITS;
	fs.write(reinterpret_cast<const char*>(buff), 256);
	delete buff;

	// save all attribs
	for (unsigned int i = 0; i < NB_ORBITS; ++i)
		m_attribs[i].saveBin(fs, i);

	if (m_isMultiRes)
		m_mrattribs.saveBin(fs, 00);

	return true;
}

bool GenericMap::loadMapBin(const std::string& filename)
{

	CGoGNistream fs(filename.c_str(), std::ios::in|std::ios::binary);
	if (!fs)
	{
		CGoGNerr << "Unable to open file for loading" << CGoGNendl;
		return false;
	}

	GenericMap::clear(true);

	// read info
	char* buff = new char[256];
	fs.read(reinterpret_cast<char*>(buff), 256);

	std::string buff_str(buff);
	// Check file type
	if (m_isMultiRes)
	{
		if (buff_str == "CGoGN_Map")
		{
			CGoGNerr<< "Wrong binary file format, file is not a MR-Map"<< CGoGNendl;
			return false;
		}
		if (buff_str != "CGoGN_MRMap")
		{
			CGoGNerr<< "Wrong binary file format"<< CGoGNendl;
			return false;
		}
	}
	else
	{
		if (buff_str == "CGoGN_MRMap")
		{
			CGoGNerr<< "Wrong binary file format, file is a MR-Map"<< CGoGNendl;
			return false;
		}
		if (buff_str != "CGoGN_Map")
		{
			CGoGNerr<< "Wrong binary file format"<< CGoGNendl;
			return false;
		}
	}


	// Check map type
	buff_str = std::string(buff + 32);

	std::string localType = this->mapTypeName();

	std::string fileType = buff_str;

	if (fileType != localType)
	{
		CGoGNerr << "Not possible to load "<< fileType << " into " << localType << " object" << CGoGNendl;
		return false;
	}

	// Check max nb orbit
	unsigned int *ptr_nbo = reinterpret_cast<unsigned int*>(buff + 64);
	unsigned int nbo = *ptr_nbo;
	if (nbo != NB_ORBITS)
	{
		CGoGNerr << "Wrond max orbit number in file" << CGoGNendl;
		return  false;
	}

	// load attrib container
	for (unsigned int i = 0; i < NB_ORBITS; ++i)
	{
		unsigned int id = AttributeContainer::loadBinId(fs);
		m_attribs[id].loadBin(fs);
	}

	if (m_isMultiRes)
		m_mrattribs.loadBin(fs);

	// retrieve m_embeddings (from m_attribs)
	update_m_emb_afterLoad();

	// recursive call from real type of map (for topo relation attributes pointers) down to GenericMap (for Marker_cleaning & pointers)
	update_topo_shortcuts();



	return true;
}

void GenericMap::update_m_emb_afterLoad()
{
	// get container of dart orbit
	AttributeContainer& cont = m_attribs[DART] ;

	// get the list of attributes
	std::vector<std::string> listeNames;
	cont.getAttributesNames(listeNames);

	// check if there are EMB_X attributes
	for (unsigned int i = 0;  i < listeNames.size(); ++i)
	{
		std::string sub = listeNames[i].substr(0, listeNames[i].size() - 1);
		if (sub == "EMB_")
		{
			unsigned int orb = listeNames[i][4]-'0'; // easy atoi computation for one char;
			AttributeMultiVector<unsigned int>* amv = cont.getDataVector<unsigned int>(i);
			m_embeddings[orb] = amv ;
		}
	}
}

void GenericMap::update_topo_shortcuts()
{
	for(unsigned int orbit = 0; orbit < NB_ORBITS; ++orbit)
	{
		AttributeContainer& cont = m_attribs[orbit];

		// get the list of attributes of orbit container
		std::vector<std::string> listeNames;
		cont.getAttributesNames(listeNames);

		for (unsigned int i = 0;  i < listeNames.size(); ++i)
		{
			std::string sub = listeNames[i].substr(0, 5);
			if (sub == "Mark_")
			{
				// get thread number
				unsigned int thread = listeNames[i][5]-'0';
				if (listeNames[i].size() > 6) 					// thread number is >9
					thread = 10*thread + listeNames[i][6]-'0';

				AttributeMultiVector<Mark>* amvMark = cont.getDataVector<Mark>(i);
				m_markTables[orbit][thread] = amvMark ;

				if ((orbit == DART) && (thread == 0))	// for Marker of dart of thread O keep the boundary marker
				{
					Mark m(m_boundaryMarker);
					m.invert();
					for (unsigned int i = cont.begin(); i != cont.end(); cont.next(i))
						amvMark->operator[](i).unsetMark(m);
				}
				else								// for others clear all
				{
					for (unsigned int i = cont.begin(); i != cont.end(); cont.next(i))
						amvMark->operator[](i).clear();
				}
			}
		}
	}

	if (m_isMultiRes)
	{
		std::vector<std::string> names;
		m_mrattribs.getAttributesNames(names);
		m_mrDarts.resize(names.size()-1);
		for (unsigned int i=0; i<m_mrDarts.size(); ++i)
			m_mrDarts[i] = NULL;

		for (unsigned int i = 0;  i < names.size(); ++i)
		{
			std::string sub = names[i].substr(0, 7);

			if (sub=="MRLevel")
				m_mrLevels = m_mrattribs.getDataVector<unsigned int>(i);

			if (sub=="MRdart_")
			{
				sub = names[i].substr(7);	// compute number following MT_Dart_
				unsigned int idx=0;
				for (unsigned int j=0; j < sub.length(); j++)
					idx = 10*idx+(sub[j]-'0');
				if (idx < names.size()-1)
					m_mrDarts[idx] = m_mrattribs.getDataVector<unsigned int>(i);
				else
					CGoGNerr<<"Warning problem updating MR_DARTS" << CGoGNendl;
			}
		}
		// check if all pointers are != NULL
		for (unsigned int i=0; i<m_mrDarts.size(); ++i)
		{
			if (m_mrDarts[i] == NULL)
				CGoGNerr<<"Warning problem MR_DARTS = NULL" << CGoGNendl;
		}
	}
}

void GenericMap::dumpAttributesAndMarkers()
{
	for (unsigned int i = 0; i < NB_ORBITS; ++i)
	{
		std::vector<std::string> names;
		names.reserve(32); 				//just to limit reallocation
		m_attribs[i].getAttributesNames(names);
		unsigned int nb = names.size();
		if (nb > 0)
		{
			CGoGNout << "ORBIT "<< i << CGoGNendl;
			std::vector<std::string> types;
			types.reserve(nb);
			m_attribs[i].getAttributesTypes(types);
			for (unsigned int j = 0; j < nb; ++j)
				CGoGNout << "    " << j << " : " << types[j] << " " << names[j] << CGoGNendl;
		}
	}
	CGoGNout << "RESERVED MARKERS "<< CGoGNendl;
	for (unsigned int i = 0; i < NB_ORBITS; ++i)
	{
		for (unsigned int j = 0; j < NB_THREAD; ++j)
		{
			MarkSet ms = m_marksets[i][j];
			if (!ms.isClear())
			{
				CGoGNout << "Orbit " << i << "  thread " << j << " : ";
				Mark m(1);
				for (unsigned i = 0; i < Mark::getNbMarks(); ++i)
				{
					if (ms.testMark(m))
						CGoGNout << m.getMarkVal() << ", ";
					m.setMarkVal(m.getMarkVal()<<1);
				}
				CGoGNout << CGoGNendl;
			}
		}
	}
}

void GenericMap::compact()
{
	// if MR compact the MR attrib container
	std::vector<unsigned int> oldnewMR;
	if (m_isMultiRes)
		m_mrattribs.compact(oldnewMR);

	// compacting the orbits attributes
//	for (unsigned int orbit = 0; orbit < NB_ORBITS; ++orbit)
//	{
//		if ((orbit != DART) && (isOrbitEmbedded(orbit)))
//		{
//			m_attribs[orbit].compact(oldnew);
//
//			for (unsigned int i = m_attribs[DART].begin(); i != m_attribs[DART].end(); m_attribs[DART].next(i))
//			{
//				unsigned int& idx = m_embeddings[orbit]->operator [](i);
//				unsigned int jdx = oldnew[idx];
//				if ((jdx != 0xffffffff) && (jdx != idx))
//					idx = jdx;
//			}
//		}
//	}

	// compact embedding attribs
	std::vector< std::vector<unsigned int>* > oldnews;
	oldnews.resize(NB_ORBITS);
	for (unsigned int orbit = 0; orbit < NB_ORBITS; ++orbit)
	{
		if ((orbit != DART) && (isOrbitEmbedded(orbit)))
		{
			oldnews[orbit] = new std::vector<unsigned int>;
			m_attribs[orbit].compact(*(oldnews[orbit]));
		}
	}

	// update embedding indices of topo
	for (unsigned int i = m_attribs[DART].begin(); i != m_attribs[DART].end(); m_attribs[DART].next(i))
	{
		for (unsigned int orbit = 0; orbit < NB_ORBITS; ++orbit)
		{
			if ((orbit != DART) && (isOrbitEmbedded(orbit)))
			{
				unsigned int& idx = m_embeddings[orbit]->operator[](i);
				unsigned int jdx = oldnews[orbit]->operator[](idx);
				if ((jdx != 0xffffffff) && (jdx != idx))
					idx = jdx;
			}
		}
	}
	// delete allocated vectors
	for (unsigned int orbit = 0; orbit < NB_ORBITS; ++orbit)
		if ((orbit != DART) && (isOrbitEmbedded(orbit)))
			delete[] oldnews[orbit];


	//compacting the topo
	std::vector<unsigned int> oldnew;
	m_attribs[DART].compact(oldnew);


	// update MR indices to attribs[DART]
	if (m_isMultiRes)
	{
		unsigned int nbl = m_mrDarts.size();
		for (unsigned int i = m_mrattribs.begin(); i != m_mrattribs.end(); m_mrattribs.next(i))
		{
			for (unsigned int j=0; j<nbl; ++j)
			{
				unsigned int d_index = m_mrDarts[j]->operator[](i);
				if (d_index != oldnew[d_index])
					m_mrDarts[j]->operator[](i) = oldnew[d_index];
			}
		}
	}

		// update topo relations from real map
	compactTopoRelations(oldnewMR);

//	dumpAttributesAndMarkers();
}

/****************************************
 *           DARTS TRAVERSALS           *
 ****************************************/

bool GenericMap::foreach_dart(FunctorType& f, const FunctorSelect& good)
{
	for (Dart d = begin(); d != end(); next(d))
	{
		if(good(d))
			if (f(d))
				return true;
	}
	return false;
}

bool GenericMap::foreach_dart_of_orbit(unsigned int orbit, Dart d, FunctorType& f, unsigned int thread)
{
	switch(orbit)
	{
//		case DART: return f(d);
//		case VERTEX: return foreach_dart_of_vertex(d, f, thread);
//		case EDGE: return foreach_dart_of_edge(d, f, thread);
//		case ORIENTED_FACE: return foreach_dart_of_oriented_face(d, f, thread);
//		case FACE: return foreach_dart_of_face(d, f, thread);
//		case VOLUME: return foreach_dart_of_volume(d, f, thread);

		case DART:		return f(d);
		case VERTEX: 	return foreach_dart_of_vertex(d, f, thread);
		case EDGE: 		return foreach_dart_of_edge(d, f, thread);
		case FACE: 		return foreach_dart_of_face(d, f, thread);
		case VOLUME: 	return foreach_dart_of_volume(d, f, thread);
		case VERTEX1: 	return foreach_dart_of_vertex1(d, f, thread);
		case EDGE1: 	return foreach_dart_of_edge1(d, f, thread);
		case VERTEX2: 	return foreach_dart_of_vertex2(d, f, thread);
		case EDGE2:		return foreach_dart_of_edge2(d, f, thread);
		case FACE2:		return foreach_dart_of_face2(d, f, thread);
		default: assert(!"Cells of this dimension are not handled");break;

	}
	return false;
}

bool GenericMap::foreach_orbit(unsigned int orbit, FunctorType& fonct, const FunctorSelect& good, unsigned int thread)
{
	TraversorCell<GenericMap> trav(*this, orbit, good, true, thread);
	bool found = false;

	for (Dart d = trav.begin(); !found && d != trav.end(); d = trav.next())
	{
		if ((fonct)(d))
			found = true;
	}
	return found;
}

unsigned int GenericMap::getNbOrbits(unsigned int orbit, const FunctorSelect& good)
{
	FunctorCount fcount;
	foreach_orbit(orbit, fcount, good);
	return fcount.getNb();
}

void GenericMap::viewAttributesTables()
{
	std::cout << "======================="<< std::endl;
	for (unsigned int i = 0; i < NB_ORBITS; ++i)
	{
		std::cout << "ATTRIBUTE_CONTAINER " << i << std::endl;
		AttributeContainer& cont = m_attribs[i] ;

		// get the list of attributes
		std::vector<std::string> listeNames;
		cont.getAttributesNames(listeNames);
		for (std::vector<std::string>::iterator it = listeNames.begin(); it != listeNames.end(); ++it)
			std::cout << "    " << *it << std::endl;
		std::cout << "-------------------------" << std::endl;
	}
	std::cout << "m_embeddings: " << std::hex;
	for (unsigned int i = 0; i < NB_ORBITS; ++i)
		std::cout << (long)(m_embeddings[i]) << " / ";
	std::cout << std::endl << "-------------------------" << std::endl;

	std::cout << "m_markTables: ";
	for (unsigned int i = 0; i < NB_ORBITS; ++i)
		std::cout << (long)(m_markTables[i][0]) << " / ";
	std::cout << std::endl << "-------------------------" << std::endl << std::dec;
}

void GenericMap::boundaryMark(Dart d)
{
	m_markTables[DART][0]->operator[](dartIndex(d)).setMark(m_boundaryMarker);
}

void GenericMap::boundaryUnmark(Dart d)
{
	m_markTables[DART][0]->operator[](dartIndex(d)).unsetMark(m_boundaryMarker);
}

bool GenericMap::isBoundaryMarked(Dart d)
{
	return m_markTables[DART][0]->operator[](dartIndex(d)).testMark(m_boundaryMarker);
}

void GenericMap::boundaryMarkOrbit(unsigned int orbit, Dart d)
{
	FunctorMark<GenericMap> fm(*this, m_boundaryMarker, m_markTables[DART][0]) ;
	foreach_dart_of_orbit(orbit, d, fm, 0) ;
}

void GenericMap::boundaryUnmarkOrbit(unsigned int orbit, Dart d)
{
	FunctorUnmark<GenericMap> fm(*this, m_boundaryMarker, m_markTables[DART][0]) ;
	foreach_dart_of_orbit(orbit, d, fm, 0) ;
}

void GenericMap::boundaryUnmarkAll()
{
	AttributeContainer& cont = getAttributeContainer(DART) ;
	for (unsigned int i = cont.begin(); i != cont.end(); cont.next(i))
		m_markTables[DART][0]->operator[](i).unsetMark(m_boundaryMarker);
}

} // namespace CGoGN

//
//bool GenericMap::saveMapXml(const std::string& filename, bool compress)
//{
//	xmlTextWriterPtr writer = xmlNewTextWriterFilename(filename.c_str(), 0);
//	xmlTextWriterStartDocument(writer,NULL,NULL,NULL);
//
//	// Entete
//	int rc = xmlTextWriterStartElement(writer, BAD_CAST "CGoGN_Map");
//	rc = xmlTextWriterWriteAttribute(writer,  BAD_CAST "type", BAD_CAST this->mapTypeName().c_str());
//	rc = xmlTextWriterWriteFormatAttribute(writer,  BAD_CAST "nb_max_orbits","%u",NB_ORBITS);
//
//	// save m_attribs
//	for (unsigned int i=0; i<NB_ORBITS; ++i)
//	{
//		m_attribs[i].saveXml(writer,i);
//	}
//
//	// save m_orbMarker
//	rc = xmlTextWriterStartElement(writer, BAD_CAST "Orbit_MarkerSet");
//	for (unsigned int i=0; i<NB_ORBITS; ++i)
//	{
//		int rc = xmlTextWriterStartElement(writer, BAD_CAST "MarkerSet");
//		rc = xmlTextWriterWriteFormatAttribute(writer,  BAD_CAST "orbit", "%u", i);
//		rc = xmlTextWriterWriteFormatAttribute(writer,  BAD_CAST "val", "%u", m_marksets[i][0].getMarkVal());
////		rc = xmlTextWriterWriteAttribute(writer,  BAD_CAST "bin", BAD_CAST m_marksets[i][0].getMarkerAsBinaryString().c_str());
////		m_marksets[i] ;
//		rc = xmlTextWriterEndElement(writer);
//	}
//	rc = xmlTextWriterEndElement(writer);
//
//	// save m_BoundaryMarkerVal
////	rc = xmlTextWriterStartElement(writer, BAD_CAST "Boundary_Marker");
////	rc = xmlTextWriterWriteFormatAttribute(writer,  BAD_CAST "val", "%u", m_BoundaryMarkerVal.getMarkVal());
////	rc = xmlTextWriterEndElement(writer);
//
//	rc = xmlTextWriterEndElement(writer); // of map
//
//	xmlTextWriterEndDocument(writer);
//	xmlFreeTextWriter(writer);
//
//	return true;
//}
//
//
//bool GenericMap::loadMapXml(const std::string& filename, bool compress)
//{
//
//	xmlDocPtr doc = xmlReadFile(filename.c_str(), NULL, 0);
//	xmlNodePtr map_node = xmlDocGetRootElement(doc);
//
//	// Check if it is a CGoGN_Map file
////	if (strcmp((char*)(map_node->name),(char*)"CGoGN_Map")!=0)
//	if (!chechXmlNode(map_node,"CGoGN_Map"))
//	{
//		CGoGNerr << "Wrong xml format: Root node != CGoGN_Map"<< CGoGNendl;
//		return false;
//	}
//
//	// check the file type
//	xmlChar *prop = xmlGetProp(map_node, BAD_CAST "type");
//	CGoGNout << "Loading "<< prop <<" xml file"<< CGoGNendl;
//
//	// check the nb max orbits
//	prop = xmlGetProp(map_node, BAD_CAST "nb_max_orbits");
//	unsigned int nbo = atoi((char*)prop);
//	if (nbo != NB_ORBITS)
//	{
//		CGoGNerr << "Wrong nb max orbits in xml map"<< CGoGNendl;
//		return false;
//	}
//
//	/***********************************************
//	*	 			 load attributs
//	************************************************/
//	for (xmlNode* cur_node = map_node->children; cur_node; cur_node = cur_node->next)
//	{
//		// for each attribute
////		if (strcmp((char*)(cur_node->name),(char*)"Attributes_Container")==0)
//		if (chechXmlNode(cur_node, "Attributes_Container"))
//		{
//			CGoGNout << "LOAD ATTRIBUT"<< CGoGNendl;
//			// get the orbit id
//			unsigned int id = AttributeContainer::getIdXmlNode(cur_node);
//			// and load container
//			unsigned int nba = m_attribs[id].getNbAttributes();
//
//
//			CGoGNout << "attribut "<<id<<" size="<< m_attribs[id].size()<< "  nbatt="<< nba<< CGoGNendl;
//
//			m_attribs[id].loadXml(cur_node);
//		}
//	}
//
//	/***********************************************
//	*   creation of the m_embeddings pointers table
//	************************************************/
////	// get attribute names of dart orbit
////	AttributeContainer& contDart = m_attribs[DART] ;
////	std::vector< std::string > tableNames;
////	contDart.getAttributesStrings(tableNames);
////
////	// find orbit frome name and store pointer in right place
////	for (unsigned int i = 0; i< tableNames.size(); ++i)
////	{
//////		CGoGNout << i <<" : "<< tableNames[i]<<CGoGNendl;
////
////		std::string& name = tableNames[i];
////		std::string is_an_emb = name.substr(0,4);
////		if (is_an_emb == "EMB_")
////		{
////			AttributeMultiVector<unsigned int>& amv = contDart.getDataVector<unsigned int>(i) ;
////
////			std::string orbitname = name.substr(4, name.size()-4);
////			std::istringstream iss(orbitname);
////			unsigned int orbit;
////			iss >> orbit;
////
////			m_embeddings[orbit] = &amv ;
////		}
////	}
////
////
//	update_m_emb_afterLoad();
//
//	/***********************************************
//	*	 load Orbit_MarkerSet & BoundaryMarker
//	************************************************/
//	xmlNode* cur_node = map_node->children;
//	bool read1=false;
//	bool read2=false;
//	while (!(read1 || read2) && cur_node)// scan nodes to find the one with right name
//	{
////		if (strcmp((char*)(cur_node->name),(char*)"Orbit_MarkerSet") == 0)
//		if (chechXmlNode(cur_node, "Orbit_MarkerSet"))
//		{
//			for (xmlNode* mark_node = cur_node->children; mark_node; mark_node = mark_node->next)
//			{
//				xmlChar* prop = xmlGetProp(mark_node, BAD_CAST "orbit");
//				unsigned int orb = atoi((char*)prop);
//				prop = xmlGetProp(mark_node, BAD_CAST "val");
//				unsigned int val = atoi((char*)prop);
//				m_marksets[orb][0].setMarkVal(val);
//			}
//			read1 =true;
//		}
//		else
//		{
////			if (strcmp((char*)(cur_node->name),(char*)"Boundary_Marker") == 0)
//			CGoGNout << "Orbit_MarkerSet"<<CGoGNendl;
////			if (chechXmlNode(cur_node, "Boundary_Marker"))
////			{
////				xmlChar* prop = xmlGetProp(cur_node, BAD_CAST "val");
////				unsigned int val = atoi((char*)prop);
////				m_BoundaryMarkerVal.setMarkVal(val);
////				read2 =true;
////			}
//		}
//		// next node
//		cur_node = cur_node->next;
//	}
//
//	if (!(read1 && read2))
//	{
//		CGoGNerr <<"Error reading Marker in xml node"<<CGoGNendl;
//		return false;
//	}
//
//	xmlFreeDoc(doc);
//
//	update_m_emb_afterLoad();
//
//	return true ;
//}
