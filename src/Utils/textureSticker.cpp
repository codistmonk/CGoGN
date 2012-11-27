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

#include <vector>

namespace CGoGN
{

namespace Utils
{

TextureSticker::TextureSticker()
{
}

TextureSticker::~TextureSticker()
{
}

void TextureSticker::StickTextureOnWholeScreen()
{
}

void TextureSticker::InitializeElements()
{
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

	// Initialisation des Vbos
	glGenBuffers(1, &m_quadPositionsVboId);
	glGenBuffers(1, &m_quadTexCoordsVboId);
	glBindBuffer(GL_ARRAY_BUFFER, m_quadPositionsVboId);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), &positions, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, m_quadTexCoordsVboId);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), &texCoords, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
}

} // namespace Utils

} // namespace CGoGN

