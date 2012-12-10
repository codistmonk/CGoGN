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

#ifdef WITH_QT

#include <GL/glew.h>
#include "Utils/Shaders/shaderMultTextures.h"


namespace CGoGN
{

namespace Utils
{

#include "shaderMultTextures.vert"
#include "shaderMultTextures.frag"

ShaderMultTextures::ShaderMultTextures()
{
	std::string glxvert(*GLSLShader::DEFINES_GL);
	glxvert.append(vertexShaderText);

	std::string glxfrag(*GLSLShader::DEFINES_GL);
	glxfrag.append(fragmentShaderText);

	loadShadersFromMemory(glxvert.c_str(), glxfrag.c_str());

	m_unifTex1Unit = glGetUniformLocation(this->program_handler(), "texture1Unit");
	m_unifTex2Unit = glGetUniformLocation(this->program_handler(), "texture2Unit");
}

void ShaderMultTextures::setTexture1Unit(GLenum textureUnit)
{
	this->bind();
	int unit = textureUnit - GL_TEXTURE0;
	glUniform1iARB(*m_unifTex1Unit, unit);
	m_tex1Unit = unit;
}

void ShaderMultTextures::setTexture2Unit(GLenum textureUnit)
{
	this->bind();
	int unit = textureUnit - GL_TEXTURE0;
	glUniform1iARB(*m_unifTex2Unit, unit);
	m_tex2Unit = unit;
}

void ShaderMultTextures::activeTexture1(CGoGNGLuint texId)
{
	glActiveTexture(GL_TEXTURE0 + m_tex1Unit);
	glBindTexture(GL_TEXTURE_2D, *texId);
}

void ShaderMultTextures::activeTexture2(CGoGNGLuint texId)
{
	glActiveTexture(GL_TEXTURE0 + m_tex2Unit);
	glBindTexture(GL_TEXTURE_2D, *texId);
}

unsigned int ShaderMultTextures::setAttributePosition(VBO* vbo)
{
	m_vboPos = vbo;
	return bindVA_VBO("VertexPosition", vbo);
}

unsigned int ShaderMultTextures::setAttributeTexCoord(VBO* vbo)
{
	m_vboTexCoord = vbo;
	return bindVA_VBO("VertexTexCoord", vbo);
}

void ShaderMultTextures::restoreUniformsAttribs()
{
	bindVA_VBO("VertexPosition", m_vboPos);
	bindVA_VBO("VertexTexCoord", m_vboTexCoord);
	glUniform1iARB(*m_unifTex1Unit, m_tex1Unit);
	glUniform1iARB(*m_unifTex2Unit, m_tex2Unit);
}

} // namespace Utils

} // namespace CGoGN
#else
#pragma message(__FILE__ " not compiled because of mising Qt")
#endif
