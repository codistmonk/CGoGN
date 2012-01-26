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

#ifndef MARCHINGCUBE_H
#define MARCHINGCUBE_H

#include "Algo/MC/image.h"
#include "Algo/MC/buffer.h"
#include "Algo/MC/tables.h"

#include "Geometry/vector_gen.h"

namespace CGoGN
{

namespace Algo
{

namespace MC {
/**
 * Marching Cube
 *
 * @param DataType the type of voxel image
 * @param Windowing the windowing class which allow to distinguish inside from outside
 */
template< typename  DataType, template < typename D2 > class Windowing, typename PFP >
class MarchingCube
{
protected:
	typedef typename PFP::MAP L_MAP;
	typedef Dart L_DART;

	/**
	* voxel image
	*/
	Image<DataType>* m_Image;

	/**
	 *  the windowing class that define inside from outside
	 */
	Windowing<DataType> m_windowFunc;

	/**
	* Pointer on buffer
	*/
	Buffer<L_DART>* m_Buffer;

	/**
	* map to store the mesh
	*/
	L_MAP* m_map;

	typename PFP::TVEC3 m_positions;

	/**
	* Origin of image
	*/
	typename PFP::VEC3 m_fOrigin;

	/**
	* scale of image
	*/
	typename PFP::VEC3 m_fScal;

	/**
	* compute the index of a cube:\n
	* The index is a eight bit index, one bit for each vertex of the cube.\n
	* If the vertex is inside the objet (val < isovalue) the bit is set to 1, else to 0
	*/
	unsigned char computeIndex(const DataType* const _ucData) const;

	/**
	 * tag boundary to b removed or not
	 */
	bool m_brem;

	/**
	* @name Creation of face
	* 8 different cases -> 8 functions
	*
	* create the faces of the current cube from index and coordinates of cubes in the eight different cases:
	* -# Z=0 Y=0 X=0
	* -# Z=0 Y=0 X=1..
	* -# Z=0 Y=1.. X=0
	* -# Z=0 Y=1.. X=1..
	* -# Z=1.. Y=0 X=0
	* -# Z=1.. Y=0 X=1..
	* -# Z=1.. Y=1.. X=0
	* -# Z=1.. Y=1.. X=1..
	*/
	//@{

	/**
	* create the faces of the current cube from index and coordinates
	* of cube.
	**************************************************************
	* @param 	vox pointer on current voxel
	* @param   _lX  coordinate X of the cube
	* @param   _lY  coordinate Y of the cube
	* @param   _lZ  coordinate Z of the cube
	* @param   tag  the boundary tag (NOT USE FOR THE MOMENT)
	*/
	void createFaces_1(DataType *vox, const int _lX, const int _lY, const int _lZ, unsigned char tag);
	void createFaces_2(DataType *vox, const int _lX, const int _lY, const int _lZ, unsigned char tag);
	void createFaces_3(DataType *vox, const int _lX, const int _lY, const int _lZ, unsigned char tag);
	void createFaces_4(DataType *vox, const int _lX, const int _lY, const int _lZ, unsigned char tag);
	void createFaces_5(DataType *vox, const int _lX, const int _lY, const int _lZ, unsigned char tag);
	void createFaces_6(DataType *vox, const int _lX, const int _lY, const int _lZ, unsigned char tag);
	void createFaces_7(DataType *vox, const int _lX, const int _lY, const int _lZ, unsigned char tag);
	void createFaces_8(DataType *vox, const int _lX, const int _lY, const int _lZ, unsigned char tag);
//@}

	/**
	* create the faces of the current cube from index, coordinates, list of vertex and  face mask
	*
	* @param   _ucCubeIndex the index of the cube
	* @param   _lX  coordinate X of the cube
	* @param   _lY  coordinate Y of the cube
	* @param   _lZ  coordinate Z of the cube
	* @param   _lVertTable list of index of created vertices
	* @param   _ucMask the mask which describes in whose neighbours cube we have to search neighbours faces
	* @param   curv curvature of face (NOT USE FOR THE MOMENT)
	* @param   tag the boundary tag value (NOT USE FOR THE MOMENT)
	*
	* @todo use the member (of struct HalfCube) number of faces instead of fill with -1
	*/
	void createLocalFaces(const unsigned char _ucCubeIndex, const int _lX, const int _lY, const int _lZ, unsigned int const *_lVertTable, const unsigned short _ucMask, float curv, unsigned char tag);

	/**
	* @name create vertices on edges of cube
	* 12 edges -> 12 functions
	*/
//@{
	/**
	* create the faces of the current cube from index, coordinates, list of vertex and  face mask
	*
	* @param   _ucCubeIndex the index of the cube
	* @param   _lX  coordinate X of the cube
	* @param   _lY  coordinate Y of the cube
	* @param   _lZ  coordinate Z of the cube
	* @param   lVertTable list of index of created vertices
	* @param   vPos the position in "real world"
	*
	*/
	void createPointEdge0(const unsigned char _ucCubeIndex, const int _lX, const int _lY, const int _lZ, unsigned int* const lVertTable, const typename PFP::VEC3& vPos);
	void createPointEdge1(const unsigned char _ucCubeIndex, const int _lX, const int _lY, const int _lZ, unsigned int* const lVertTable, const typename PFP::VEC3& vPos);
	void createPointEdge2(const unsigned char _ucCubeIndex, const int _lX, const int _lY, const int _lZ, unsigned int* const lVertTable, const typename PFP::VEC3& vPos);
	void createPointEdge3(const unsigned char _ucCubeIndex, const int _lX, const int _lY, const int _lZ, unsigned int* const lVertTable, const typename PFP::VEC3& vPos);
	void createPointEdge4(const unsigned char _ucCubeIndex, const int _lX, const int _lY, const int _lZ, unsigned int* const lVertTable, const typename PFP::VEC3& vPos);
	void createPointEdge5(const unsigned char _ucCubeIndex, const int _lX, const int _lY, const int _lZ, unsigned int* const lVertTable, const typename PFP::VEC3& vPos);
	void createPointEdge6(const unsigned char _ucCubeIndex, const int _lX, const int _lY, const int _lZ, unsigned int* const lVertTable, const typename PFP::VEC3& vPos);
	void createPointEdge7(const unsigned char _ucCubeIndex, const int _lX, const int _lY, const int _lZ, unsigned int* const lVertTable, const typename PFP::VEC3& vPos);
	void createPointEdge8(const unsigned char _ucCubeIndex, const int _lX, const int _lY, const int _lZ, unsigned int* const lVertTable, const typename PFP::VEC3& vPos);
	void createPointEdge9(const unsigned char _ucCubeIndex, const int _lX, const int _lY, const int _lZ, unsigned int* const lVertTable, const typename PFP::VEC3& vPos);
	void createPointEdge10(const unsigned char _ucCubeIndex, const int _lX, const int _lY, const int _lZ, unsigned int* const lVertTable, const typename PFP::VEC3& vPos);
	void createPointEdge11(const unsigned char _ucCubeIndex, const int _lX, const int _lY, const int _lZ, unsigned int* const lVertTable, const typename PFP::VEC3& vPos);
//@}

	/**
	 * modify position of vertex to obtain real world coordinate
	 * @param _P initial position
	 * @param _dec shifting due to interpolation in the cube
	 * @return new position
	 */
	typename PFP::VEC3 recalPoint( const typename PFP::VEC3& _P,  const typename PFP::VEC3& _dec ) const;

	void setNeighbourSimple(L_DART d1, L_DART d2);

	void setNeighbour(L_DART d1, L_DART d2);

	L_DART createTriEmb(unsigned int e1, unsigned int e2, unsigned int e3) {
		L_DART d = m_map->newFace(3);
		//TODO change this which work only with 2-maps
		m_map->setDartEmbedding(VERTEX,d,e1); d = m_map->phi1(d);
		m_map->setDartEmbedding(VERTEX,d,e2); d = m_map->phi1(d);
		m_map->setDartEmbedding(VERTEX,d,e3); d = m_map->phi1(d);
		return d;
	}

public:
	/**
	* constructor from filename
	* @param _cName name of file to open
	*/
	MarchingCube(const char* _cName);

	/**
	* constructor from image
	* @param img voxel image
	* @param wind the windowing class (for inside/outside distinguish)
	* @param boundRemoved true is bound is going to be removed
	*/
	MarchingCube(Image<DataType>* img, Windowing<DataType> wind, bool boundRemoved);

	/**
	* constructor from filename
	* @param img voxel image
	* @param map ptr to the map use to store the mesh
	* @param idPos id of attribute position
	* @param wind the windowing class (for inside/outside distinguish)
	* @param boundRemoved true is bound is going to be removed
	*/
	MarchingCube(Image<DataType>* img, L_MAP* map, const typename PFP::TVEC3& position,  Windowing<DataType> wind, bool boundRemoved);

	/**
	* destructor
	*/
	~MarchingCube();

	/**
	* delete the mesh
	*/
	void deleteMesh();

	/**
	* simple version of Marching Cubes algorithm
	*/
	void simpleMeshing();

	/**
	 * get pointer on result mesh after processing
	 * @return the mesh
	 */
	L_MAP* getMesh() const { return m_map;}

	/**
	 * get the image
	 */
	Image<DataType>* getImg() {return m_Image;}

	/**
	 * Get the lower corner of bounding AABB
	 */
	Geom::Vec3f boundMin() const {return m_Image->boundMin();}

	/**
	 * Get the upper corner of bounding AABB
	 */
	Geom::Vec3f boundMax() const {return m_Image->boundMax();}

	void removeFacesOfBoundary(AttributeHandler<unsigned char>& boundVertices, unsigned int frameWidth);

	void recalPoints();
};


} // end namespace
} // end namespace
} // end namespace

#include "Algo/MC/marchingcube.hpp"
#endif
