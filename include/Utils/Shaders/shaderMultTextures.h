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

#ifndef __CGOGN_SHADER_MULTTEXTURES__
#define __CGOGN_SHADER_MULTTEXTURES__


#include "Geometry/vector_gen.h"
#include "Utils/GLSLShader.h"
#include "Utils/clippingShader.h"
#include "Utils/textures.h"
#include "Utils/gl_def.h"

namespace CGoGN
{

namespace Utils
{

class ShaderMultTextures : public ClippingShader
{

public:
	ShaderMultTextures();

	void setTexture1Unit(GLenum textureUnit);
	void setTexture2Unit(GLenum textureUnit);
	
	void activeTexture1(CGoGNGLuint texId);
	void activeTexture2(CGoGNGLuint texId);

	unsigned int setAttributePosition(VBO* vbo);
	unsigned int setAttributeTexCoord(VBO* vbo);

protected:

	static std::string vertexShaderText;
	static std::string fragmentShaderText;

	CGoGNGLuint m_unifTex1Unit;
	CGoGNGLuint m_unifTex2Unit;
	
	int m_tex1Unit;
	int m_tex2Unit;
	
	VBO* m_vboPos;
	VBO* m_vboTexCoord;

	void restoreUniformsAttribs();
};

} // namespace Utils

} // namespace CGoGN


#endif /* __CGOGN_SHADER_MULTTEXTURES__ */
