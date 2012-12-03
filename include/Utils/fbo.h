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

#ifndef __CGoGN_GLSL_FBO__
#define __CGoGN_GLSL_FBO__

#include <GL/glew.h>
#include "Utils/gl_def.h"

#include <iostream>
#include <vector>

namespace CGoGN
{

namespace Utils
{

class FBO
{

public:

	FBO(unsigned int width, unsigned int height);

	~FBO();

	void AttachRender(GLenum internalformat);
	void AttachColorTexture(GLenum internalformat, GLint filter = GL_LINEAR);
	void AttachDepthTexture(GLint filter = GL_LINEAR);

	void BindColorTexOutput();
	void BindColorTexOutput(int num);
	CGoGNGLuint GetColorTexId(int num) { return m_colorTexID[num]; }
	CGoGNGLuint GetDepthTexId() { return m_depthTexID[0]; }

	void Unbind();

	void CheckFBO();

protected:

	unsigned int m_width;
	unsigned int m_height;

	int m_maxColorAttachments;

	CGoGNGLuint m_fboID;
	CGoGNGLuint m_renderID;
	std::vector<CGoGNGLuint> m_colorTexID;
	std::vector<CGoGNGLuint> m_depthTexID;

	CGoGNGLenumTable m_colorAttachmentPoints;

};

}
}

#endif	/* FRAMEBUFFER_HPP */
