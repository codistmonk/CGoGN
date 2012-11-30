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

#include "GL/glew.h"
#include "Utils/textureSticker.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_projection.hpp"

namespace CGoGN
{

namespace Utils
{

bool TextureSticker::m_isInitialized                               = false;
Utils::VBO* TextureSticker::m_quadPositionsVbo                     = NULL;
Utils::VBO* TextureSticker::m_quadTexCoordsVbo                     = NULL;
Utils::ShaderSimpleTexture* TextureSticker::m_textureMappingShader = NULL;

TextureSticker::TextureSticker()
{
}

TextureSticker::~TextureSticker()
{
}

void TextureSticker::StickTextureOnWholeScreen(CGoGNGLuint texId)
{
	// Check if TextureSticker's elements have been initialized before
	if (!m_isInitialized)
	{
		InitializeElements();
		m_isInitialized = true;
	}

	// Bind texture mapping shader
	m_textureMappingShader->bind();
	
	// Set texture uniform
	m_textureMappingShader->setTextureUnit(GL_TEXTURE0);
	m_textureMappingShader->activeTexture(texId);
	
	// Set matrices uniforms
	glm::mat4 projMatrix = glm::ortho(-0.5f, 0.5f, -0.5f, 0.5f, -1.0f, 1.0f);
	glm::mat4 viewMatrix(1.0f);
	m_textureMappingShader->updateMatrices(projMatrix, viewMatrix);

	// Draw quad	
	m_textureMappingShader->enableVertexAttribs();
	glDrawArrays(GL_QUADS, 0, 4);
	m_textureMappingShader->disableVertexAttribs();

	// Unbind texture mapping shader
	m_textureMappingShader->unbind();
}

void TextureSticker::InitializeElements()
{
	// Initialize positions and texture coords Vbos
	
	m_quadPositionsVbo = new Utils::VBO();
	m_quadTexCoordsVbo = new Utils::VBO();
	m_quadPositionsVbo->setDataSize(3);
	m_quadTexCoordsVbo->setDataSize(2);
	m_quadPositionsVbo->allocate(4);
	m_quadTexCoordsVbo->allocate(4);

	GLfloat positions[] = {
		-0.5f, -0.5f, 0.0f,
		+0.5f, -0.5f, 0.0f,
		+0.5f, +0.5f, 0.0f,
		-0.5f, +0.5f, 0.0f
		};
	GLfloat texCoords[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
		};
		
	GLfloat* positionsPtr = (GLfloat*) m_quadPositionsVbo->lockPtr();
	memcpy(positionsPtr, positions, 3 * 4 * sizeof(GLfloat));
	m_quadPositionsVbo->releasePtr();
	GLfloat* texCoordsPtr = (GLfloat*) m_quadTexCoordsVbo->lockPtr();
	memcpy(texCoordsPtr, texCoords, 2 * 4 * sizeof(GLfloat));
	m_quadTexCoordsVbo->releasePtr();
	
	// Initialize simple texture mapping shader
	
	m_textureMappingShader = new Utils::ShaderSimpleTexture();
	m_textureMappingShader->setAttributePosition(m_quadPositionsVbo);
	m_textureMappingShader->setAttributeTexCoord(m_quadTexCoordsVbo);
}

} // namespace Utils

} // namespace CGoGN

