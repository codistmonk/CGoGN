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
#include "Utils/Shaders/shaderComputeSSAO.h"


namespace CGoGN
{

namespace Utils
{

#include "shaderComputeSSAO.vert"
#include "shaderComputeSSAO.frag"

ShaderComputeSSAO::ShaderComputeSSAO()
{
	std::string glxvert(*GLSLShader::DEFINES_GL);
	glxvert.append(vertexShaderText);

	std::string glxfrag(*GLSLShader::DEFINES_GL);
	glxfrag.append(fragmentShaderText);

	loadShadersFromMemory(glxvert.c_str(), glxfrag.c_str());

	m_unifNormalTexUnit = glGetUniformLocation(this->program_handler(), "normalTextureUnit");
	m_unifDepthTexUnit = glGetUniformLocation(this->program_handler(), "depthTextureUnit");
}

void ShaderComputeSSAO::setNormalTextureUnit(GLenum textureUnit)
{
	this->bind();
	int unit = textureUnit - GL_TEXTURE0;
	glUniform1iARB(*m_unifNormalTexUnit, unit);
	m_normalTexUnit = unit;
}

void ShaderComputeSSAO::setDepthTextureUnit(GLenum textureUnit)
{
	this->bind();
	int unit = textureUnit - GL_TEXTURE0;
	glUniform1iARB(*m_unifDepthTexUnit, unit);
	m_depthTexUnit = unit;
}

void ShaderComputeSSAO::activeNormalTexture(CGoGNGLuint texId)
{
	glActiveTexture(GL_TEXTURE0 + m_normalTexUnit);
	glBindTexture(GL_TEXTURE_2D, *texId);
}

void ShaderComputeSSAO::activeDepthTexture(CGoGNGLuint texId)
{
	glActiveTexture(GL_TEXTURE0 + m_depthTexUnit);
	glBindTexture(GL_TEXTURE_2D, *texId);
}

unsigned int ShaderComputeSSAO::setAttributePosition(VBO* vbo)
{
	m_vboPos = vbo;
	return bindVA_VBO("VertexPosition", vbo);
}

unsigned int ShaderComputeSSAO::setAttributeTexCoord(VBO* vbo)
{
	m_vboTexCoord = vbo;
	return bindVA_VBO("VertexTexCoord", vbo);
}

void ShaderComputeSSAO::restoreUniformsAttribs()
{
	bindVA_VBO("VertexPosition", m_vboPos);
	bindVA_VBO("VertexTexCoord", m_vboTexCoord);
	glUniform1iARB(*m_unifNormalTexUnit, m_normalTexUnit);
	glUniform1iARB(*m_unifDepthTexUnit, m_depthTexUnit);
}

} // namespace Utils

} // namespace CGoGN
#else
#pragma message(__FILE__ " not compiled because of mising Qt")
#endif