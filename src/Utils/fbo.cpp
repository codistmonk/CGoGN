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
#include "Utils/fbo.h"

namespace CGoGN
{

namespace Utils
{

bool FBO::s_anyFboBound = false;

FBO::FBO(unsigned int width, unsigned int height)
{
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &m_maxColorAttachments);
	*m_colorAttachmentPoints = new GLenum[m_maxColorAttachments];

	glGenFramebuffers(1, &(*m_fboID));

	m_width = width;
	m_height = height;
	
	m_bound = false;
}

FBO::~FBO()
{
	GLuint tex_id;

	for (unsigned int i = 0; i < m_colorTexID.size(); i++)
	{
		tex_id = *(m_colorTexID.at(i));
		glDeleteTextures(1, &tex_id);
	}
	for (unsigned int i = 0; i < m_depthTexID.size(); i++)
	{
		tex_id = *(m_depthTexID.at(i));
		glDeleteTextures(1, &tex_id);
	}

	glDeleteFramebuffers(1, &(*m_fboID));
	delete[] *m_colorAttachmentPoints;
}

void FBO::AttachRenderbuffer(GLenum internalFormat)
{
	if (s_anyFboBound)
	{
		CGoGNerr << "FBO::AttachRenderbuffer : No Fbo should be bound when attaching a render buffer." << CGoGNendl;
		return;
	}

	GLenum attachment;
	GLuint renderID;

	switch (internalFormat)
	{
		case GL_DEPTH_COMPONENT :
			attachment = GL_DEPTH_ATTACHMENT;
			break;
			
		case GL_STENCIL_INDEX :
			attachment = GL_STENCIL_ATTACHMENT;
			break;
			
		case GL_DEPTH_STENCIL :
			attachment = GL_DEPTH_STENCIL_ATTACHMENT;
			break;
			
		default :
			CGoGNerr << "FBO::AttachRenderbuffer : Bad internal format." << CGoGNendl;
			return;
			break;
	}

	glGenRenderbuffers(1, &renderID);
	glBindFramebuffer(GL_FRAMEBUFFER, *m_fboID);
	glBindRenderbuffer(GL_RENDERBUFFER, renderID);
	glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, m_width, m_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, renderID);

	*m_renderID = renderID;
    
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FBO::AttachColorTexture(GLenum internalFormat, GLint filter)
{
	if (s_anyFboBound)
	{
		CGoGNerr << "FBO::AttachColorTexture : No Fbo should be bound when attaching a texture." << CGoGNendl;
		return;
	}
	
	if ((int) m_colorTexID.size() == m_maxColorAttachments)
	{
		CGoGNerr << "FBO::AttachColorTexture : The maximum number of color textures has been exceeded." << CGoGNendl;
		return;
	}

	GLenum attachment;
	GLenum format;
	GLenum type;
	GLuint texID;

	attachment = GL_COLOR_ATTACHMENT0 + m_colorTexID.size();

	switch (internalFormat)
	{
		case GL_RGBA :
			format = GL_RGBA;
			type = GL_FLOAT;
			break;
			
		case GL_RGB :
			format = GL_RGB;
			type = GL_FLOAT;
			break;
			
		case GL_RGBA32F :
			format = GL_RGBA;
			type = GL_FLOAT;
			break;
			
		case GL_RGB32F :
			format = GL_RGB;
			type = GL_FLOAT;
			break;
			
		default :
			CGoGNerr << "FBO::AttachColorTexture : Specified internal format not handled." << CGoGNendl;
			return;
			break;
	}

	glGenTextures(1, &texID);
	glBindFramebuffer(GL_FRAMEBUFFER, *m_fboID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0, format, type, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);

	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texID, 0);

	m_colorTexID.push_back(CGoGNGLuint(texID));
	(*m_colorAttachmentPoints)[m_colorTexID.size() - 1] = attachment;
    
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FBO::AttachDepthTexture(GLint filter)
{
	if (s_anyFboBound)
	{
		CGoGNerr << "FBO::AttachDepthTexture : No Fbo should be bound when attaching a texture." << CGoGNendl;
		return;
	}
	
	if( int(m_depthTexID.size()) == 1 )
	{
		std::cout << "FBO::AttachDepthTexture : Only one depth texture can be attached." << std::endl;
		return;
	}

	GLenum attachment;
	GLenum internalFormat;
	GLenum format;
	GLenum type;
	GLuint texID;

	attachment = GL_DEPTH_ATTACHMENT;
	internalFormat = GL_DEPTH_COMPONENT24;
	format = GL_RGB;
	type = GL_FLOAT;

	glGenTextures(1, &texID);
	glBindFramebuffer(GL_FRAMEBUFFER, *m_fboID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0, format, type, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);

	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texID, 0);

	m_depthTexID.push_back(CGoGNGLuint(texID));
    
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FBO::EnableColorAttachments()
{
	if (!m_bound)
	{
		CGoGNerr << "FBO::EnableColorAttachments : Fbo must be bound when enabling color attachments." << CGoGNendl;
		return;
	}

	glDrawBuffers(m_colorTexID.size(), *m_colorAttachmentPoints);
}

void FBO::EnableColorAttachment(int num)
{
	if (!m_bound)
	{
		CGoGNerr << "FBO::EnableColorAttachments : Fbo must be bound when enabling color attachments." << CGoGNendl;
		return;
	}

	glDrawBuffers(1, &(*m_colorAttachmentPoints)[num]);
}

void FBO::Bind()
{
	if (s_anyFboBound)
	{
		CGoGNerr << "FBO::Bind : Only one Fbo can be bound at the same time." << CGoGNendl;
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, *m_fboID);
	m_bound = true;
	s_anyFboBound = true;
}

void FBO::Unbind()
{
	if (m_bound)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		m_bound = false;
		s_anyFboBound = false;
	}
}

void FBO::CheckFBO()
{
	if (s_anyFboBound)
	{
		CGoGNerr << "FBO::CheckFBO : No Fbo should be bound when checking a Fbo's status." << CGoGNendl;
		return;
	}

	GLenum status;

	glBindFramebuffer(GL_FRAMEBUFFER, *m_fboID);
	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Fbo status error : " << status << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

} // namespace Utils

} // namespace CGoGN

