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

#include <GL/glew.h>
#include "Utils/Shaders/shaderPhong.h"

namespace CGoGN
{

namespace Utils
{
#include "shaderPhong.vert"
#include "shaderPhong.frag"


ShaderPhong::ShaderPhong(bool doubleSided):
	m_with_color(false),
	m_ambiant(Geom::Vec4f(0.05f,0.05f,0.1f,0.0f)),
	m_diffuse(Geom::Vec4f(0.1f,1.0f,0.1f,0.0f)),
	m_specular(Geom::Vec4f(1.0f,1.0f,1.0f,0.0f)),
	m_shininess(100.0f),
	m_lightPos(Geom::Vec3f(10.0f,10.0f,1000.0f)),
	m_vboPos(NULL),
	m_vboNormal(NULL),
	m_vboColor(NULL)
{

	m_nameVS = "ShaderPhong_vs";
	m_nameFS = "ShaderPhong_fs";
	m_nameGS = "ShaderPhong_gs";

	// get choose GL defines (2 or 3)
	// ans compile shaders
	std::string glxvert(*GLSLShader::DEFINES_GL);
	glxvert.append(vertexShaderText);
	std::string glxfrag(*GLSLShader::DEFINES_GL);
	// Use double sided lighting if set
	if (doubleSided)
		glxfrag.append("#define DOUBLE_SIDED\n");
	glxfrag.append(fragmentShaderText);

	loadShadersFromMemory(glxvert.c_str(), glxfrag.c_str());

	// and get and fill uniforms
	bind();
	getLocations();
	sendParams();
}

void ShaderPhong::getLocations()
{
	*m_unif_ambiant   = glGetUniformLocation(this->program_handler(), "materialAmbient");
	*m_unif_diffuse   = glGetUniformLocation(this->program_handler(), "materialDiffuse");
	*m_unif_specular  = glGetUniformLocation(this->program_handler(), "materialSpecular");
	*m_unif_shininess = glGetUniformLocation(this->program_handler(), "shininess");
	*m_unif_lightPos  = glGetUniformLocation(this->program_handler(), "lightPosition");
}

void ShaderPhong::sendParams()
{
	glUniform4fv(*m_unif_ambiant,  1, m_ambiant.data());
	glUniform4fv(*m_unif_diffuse,  1, m_diffuse.data());
	glUniform4fv(*m_unif_specular, 1, m_specular.data());
	glUniform1f(*m_unif_shininess,    m_shininess);
	glUniform3fv(*m_unif_lightPos, 1, m_lightPos.data());
}

void ShaderPhong::setAmbiant(const Geom::Vec4f& ambiant)
{
	this->bind();
	glUniform4fv(*m_unif_ambiant,1, ambiant.data());
	m_ambiant = ambiant;
}

void ShaderPhong::setDiffuse(const Geom::Vec4f& diffuse)
{
	this->bind();
	glUniform4fv(*m_unif_diffuse,1, diffuse.data());
	m_diffuse = diffuse;
}

void ShaderPhong::setSpecular(const Geom::Vec4f& specular)
{
	this->bind();
	glUniform4fv(*m_unif_specular,1,specular.data());
	m_specular = specular;
}

void ShaderPhong::setShininess(float shininess)
{
	this->bind();
	glUniform1f (*m_unif_shininess, shininess);
	m_shininess = shininess;
}

void ShaderPhong::setLightPosition( Geom::Vec3f lightPos)
{
	this->bind();
	glUniform3fv(*m_unif_lightPos,1,lightPos.data());
	m_lightPos = lightPos;
}

void ShaderPhong::setParams(const Geom::Vec4f& ambiant, const Geom::Vec4f& diffuse, const Geom::Vec4f& specular, float shininess, const Geom::Vec3f& lightPos)
{
	m_ambiant = ambiant;
	m_diffuse = diffuse;
	m_specular = specular;
	m_shininess = shininess;
	m_lightPos = lightPos;
	bind();
	sendParams();
}

unsigned int ShaderPhong::setAttributeColor(VBO* vbo)
{
	m_vboColor = vbo;
	if (!m_with_color)
	{
		m_with_color=true;
		// set the define and recompile shader
		std::string gl3vert(*GLSLShader::DEFINES_GL);
		gl3vert.append("#define WITH_COLOR 1\n");
		gl3vert.append(vertexShaderText);
		std::string gl3frag(*GLSLShader::DEFINES_GL);
		gl3frag.append("#define WITH_COLOR 1\n");
		gl3frag.append(fragmentShaderText);
		loadShadersFromMemory(gl3vert.c_str(), gl3frag.c_str());

		// and treat uniforms
		bind();
		getLocations();
		sendParams();
	}
	// bind th VA with WBO
	return bindVA_VBO("VertexColor", vbo);
}

void ShaderPhong::unsetAttributeColor()
{
	m_vboColor = NULL;
	if (m_with_color)
	{
		m_with_color=false;
		// unbind the VA
		unbindVA("VertexColor");
		// recompile shader
		std::string gl3vert(*GLSLShader::DEFINES_GL);
		gl3vert.append(vertexShaderText);
		std::string gl3frag(*GLSLShader::DEFINES_GL);
		gl3frag.append(fragmentShaderText);
		loadShadersFromMemory(gl3vert.c_str(), gl3frag.c_str());
		// and treat uniforms
		bind();
		getLocations();
		sendParams();
	}
}

void ShaderPhong::restoreUniformsAttribs()
{
	getLocations();

	bind();
	sendParams();

	bindVA_VBO("VertexPosition", m_vboPos);
	bindVA_VBO("VertexNormal", m_vboNormal);
	if (m_vboColor)
		bindVA_VBO("VertexColor", m_vboColor);
	unbind();
}

unsigned int ShaderPhong::setAttributePosition(VBO* vbo)
{
	m_vboPos = vbo;
	return bindVA_VBO("VertexPosition", vbo);
}

unsigned int ShaderPhong::setAttributeNormal(VBO* vbo)
{
	m_vboNormal = vbo;
	return bindVA_VBO("VertexNormal", vbo);
}

} // namespace Utils

} // namespace CGoGN
