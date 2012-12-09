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

#include <string.h>
#include <GL/glew.h>
#include "Utils/Shaders/shaderExplodeVolumesAlpha.h"

namespace CGoGN
{

namespace Utils
{
#include "shaderExplodeVolumesAlpha.vert"
#include "shaderExplodeVolumesAlpha.frag"
#include "shaderExplodeVolumesAlpha.geom"


ShaderExplodeVolumesAlpha::ShaderExplodeVolumesAlpha(bool withColorPerFace):
m_wcpf(withColorPerFace)
{
	m_nameVS = "ShaderExplodeVolumes_vs";
	m_nameFS = "ShaderExplodeVolumes_fs";
	m_nameGS = "ShaderExplodeVolumes_gs";

	std::string glxvert(*GLSLShader::DEFINES_GL);
	glxvert.append(vertexShaderText);

	std::string glxgeom;
	glxgeom.append(GLSLShader::defines_Geom("lines_witw_adjacency", "triangle_strip", 3));

	if (withColorPerFace)
		glxgeom.append("#define WITH_COLORPF 1\n");
	glxgeom.append(geometryShaderText);

	std::string glxfrag(*GLSLShader::DEFINES_GL);
	glxfrag.append(fragmentShaderText);

	loadShadersFromMemory(glxvert.c_str(), glxfrag.c_str(), glxgeom.c_str(), GL_LINES_ADJACENCY_EXT , GL_TRIANGLE_STRIP,4);

	getLocations();

	//Default values
	m_ambient = Geom::Vec4f(0.05f, 0.05f, 0.1f, 0.0f);
	m_backColor = Geom::Vec4f(1.0f, 0.1f, 0.1f, 0.0f);
	m_light_pos = Geom::Vec3f(10.0f, 10.0f, 1000.0f);
	m_plane   = Geom::Vec4f(0.0f, 0.0f, 1000.f, 1000000000000000000000000000.0f);

//	setParams(m_ambient, m_backColor, m_light_pos, m_plane);
	setAmbient(m_ambient);
	setBackColor(m_backColor);
	setLightPosition(m_light_pos);
	setClippingPlane(m_plane);
}

void ShaderExplodeVolumesAlpha::getLocations()
{
	*m_unif_ambient  = glGetUniformLocation(program_handler(),"ambient");
	*m_unif_backColor  = glGetUniformLocation(program_handler(),"backColor");
	*m_unif_lightPos = glGetUniformLocation(program_handler(),"lightPosition");
	*m_unif_plane   = glGetUniformLocation(program_handler(),"plane");
	*m_unif_unit   = glGetUniformLocation(program_handler(),"textureUnit");
}

void ShaderExplodeVolumesAlpha::setAttributePosition(VBO* vbo)
{
	m_vboPos = vbo;
	bindVA_VBO("VertexPosition", vbo);
}

void ShaderExplodeVolumesAlpha::setAttributeColor(VBO* vbo)
{
	m_vboColors = vbo;
	bindVA_VBO("VertexColor", vbo);
}

/*
void ShaderExplodeVolumesAlpha::setParams(const Geom::Vec4f& ambient, const Geom::Vec4f& backColor, const Geom::Vec3f& lightPos, const Geom::Vec4f& plane)
{
	bind();
	m_ambient = ambient;
	glUniform4fv(*m_unif_ambient, 1, ambient.data());
	m_backColor = backColor;
	glUniform4fv(*m_unif_backColor, 1, backColor.data());

	m_light_pos = lightPos;
	glUniform3fv(*m_unif_lightPos, 1, lightPos.data());

	m_plane = plane;
	glUniform4fv(*m_unif_plane,    1, m_plane.data());

	unbind(); // ??
}
*/

void ShaderExplodeVolumesAlpha::setAmbient(const Geom::Vec4f& ambient)
{
	m_ambient = ambient;
	bind();
	glUniform4fv(*m_unif_ambient,1, ambient.data());
}

void ShaderExplodeVolumesAlpha::setBackColor(const Geom::Vec4f& backColor)
{
	m_backColor = backColor;
	bind();
	glUniform4fv(*m_unif_backColor, 1, backColor.data());
}

void ShaderExplodeVolumesAlpha::setLightPosition(const Geom::Vec3f& lp)
{
	m_light_pos = lp;
	bind();
	glUniform3fv(*m_unif_lightPos,1,lp.data());
}


void ShaderExplodeVolumesAlpha::setClippingPlane(const Geom::Vec4f& plane)
{
	m_plane = plane;
	bind();
	glUniform4fv(*m_unif_plane,1, plane.data());
}

void ShaderExplodeVolumesAlpha::restoreUniformsAttribs()
{
	bind();

	*m_unif_ambient   = glGetUniformLocation(program_handler(),"ambient");
	glUniform4fv(*m_unif_ambient,  1, m_ambient.data());

	*m_unif_backColor = glGetUniformLocation(program_handler(),"backColor");
	glUniform4fv(*m_unif_backColor, 1, m_backColor.data());

	*m_unif_lightPos =  glGetUniformLocation(program_handler(),"lightPosition");
	glUniform3fv(*m_unif_lightPos, 1, m_light_pos.data());

	*m_unif_plane   = glGetUniformLocation(program_handler(),"plane");
	glUniform4fv(*m_unif_plane,    1, m_plane.data());

	*m_unif_unit   = glGetUniformLocation(program_handler(),"textureUnit");
	glUniform1iARB(*m_unif_unit, 0);

	bindVA_VBO("VertexPosition", m_vboPos);
	bindVA_VBO("VertexColor", m_vboColors);
	unbind();
}

} // namespace Utils

} // namespace CGoGN
