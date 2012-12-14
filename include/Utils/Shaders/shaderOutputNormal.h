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

#ifndef __CGOGN_SHADER_OUTPUTNORMAL__
#define __CGOGN_SHADER_OUTPUTNORMAL__


#include "Geometry/vector_gen.h"
#include "Utils/GLSLShader.h"
#include "Utils/clippingShader.h"

namespace CGoGN
{

namespace Utils
{

class ShaderOutputNormal : public ClippingShader
{

public:

	ShaderOutputNormal();

	unsigned int setAttributePosition(VBO* vbo);

	unsigned int setAttributeNormal(VBO* vbo);

protected:

	static std::string vertexShaderText;
	static std::string fragmentShaderText;

	VBO* m_vboPos;
	VBO* m_vboNormal;

	void restoreUniformsAttribs();
};

} // namespace Utils

} // namespace CGoGN


#endif /* __CGOGN_SHADER_OUTPUTNORMAL__ */
