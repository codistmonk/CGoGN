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

#ifndef _CGOGN_TEXTURESTICKER_H_
#define _CGOGN_TEXTURESTICKER_H_

#include "Utils/vbo.h"
#include "Utils/Shaders/shaderSimpleTexture.h"
#include "Utils/gl_def.h"

namespace CGoGN
{

namespace Utils
{

class TextureSticker
{

private :

	TextureSticker();
	
	~TextureSticker();
	
public :

	static void StickTextureOnWholeScreen(CGoGNGLuint texId);
	
private :

	static void InitializeElements();
	
	static bool m_isInitialized;
	
	static Utils::VBO* m_quadPositionsVbo;
	
	static Utils::VBO* m_quadTexCoordsVbo;
	
	static Utils::ShaderSimpleTexture* m_textureMappingShader;
};

} // namespace Utils

} // namespace CGoGN

#endif

