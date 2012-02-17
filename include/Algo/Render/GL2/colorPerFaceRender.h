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

#ifndef _COLOR_PER_FACE_RENDER
#define _COLOR_PER_FACE_RENDER

#include <GL/glew.h>

#include "Topology/generic/dart.h"
#include "Topology/generic/attributeHandler.h"
#include "Topology/generic/functor.h"
#include "Utils/vbo.h"
#include "Utils/GLSLShader.h"


namespace CGoGN
{

namespace Algo
{

namespace Render
{

namespace GL2
{

/**
 * Class that allors
 */
class ColorPerFaceRender
{
protected:
	Utils::VBO& m_vboPos;

	Utils::VBO& m_vboColors;

	GLuint m_nbTris;


public:
	/**
	* Constructor
	*/
	ColorPerFaceRender() ;

	/**
	* update drawing buffers
	* @param map the map
	* @param positions  attribute of position vertices
	* @param colorPerFace attribute of color (per face, or per vertex per face)
	* @param good selector
	*/
	template<typename PFP>
	void updateData(typename PFP::MAP& map, typename PFP::TVEC3& positions, typename PFP::TVEC3& colorPerFace, const FunctorSelect& good = allDarts) ;

	/**
	 * draw
	 */
	void draw(Utils::GLSLShader* sh) ;

};

}//end namespace GL2

}//end namespace Algo

}//end namespace Render

}//end namespace CGoGN

#include "Algo/Render/GL2/colorPerFaceRender.hpp"

#endif
