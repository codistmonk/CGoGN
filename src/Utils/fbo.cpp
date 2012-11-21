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

FBO::FBO()
{
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &m_maxColorAttachments);
    *m_colorAttachmentPoints = new GLenum[m_maxColorAttachments];

    glGenFramebuffers(1, &(*m_fboID));
}

FBO::FBO(unsigned int width, unsigned int height)
{
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &m_maxColorAttachments);
    *m_colorAttachmentPoints = new GLenum[m_maxColorAttachments];

    glGenFramebuffers(1, &(*m_fboID));

    m_width = width;
    m_height = height;
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

void FBO::attachRender(GLenum internalformat)
{
    GLenum attachment;
    GLuint renderID;

    if( internalformat == GL_DEPTH_COMPONENT ) {
        attachment = GL_DEPTH_ATTACHMENT;
    }

    else if( internalformat == GL_STENCIL_INDEX ) {
        attachment = GL_STENCIL_ATTACHMENT;
    }

    else if( internalformat == GL_DEPTH_STENCIL ) {
        attachment = GL_DEPTH_STENCIL_ATTACHMENT;
    }

    else {
        std::cout << "FBO::attachRender : Bad Internal Format" << std::endl;
        return;
    }

    glGenRenderbuffers(1, &renderID);
    glBindFramebuffer(GL_FRAMEBUFFER, *m_fboID);
    glBindRenderbuffer(GL_RENDERBUFFER, renderID);
    glRenderbufferStorage(GL_RENDERBUFFER, internalformat, m_width, m_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment,
                              GL_RENDERBUFFER, renderID);

    *m_renderID = renderID;
}

void FBO::attachColorTexture(GLenum internalformat, GLint filter)
{
    GLenum attachment;
    GLenum format;
    GLenum type;
    GLuint texID;

    if( int(m_colorTexID.size()) == m_maxColorAttachments ) {
        std::cout << "FBO::attachColorTexture : The number of color textures has been exceeded" << std::endl;
        return;
    }

    attachment = GL_COLOR_ATTACHMENT0 + m_colorTexID.size();

    if( internalformat == GL_RGBA ) {
        format = GL_RGBA;
        type = GL_FLOAT;
    }

    if( internalformat == GL_RGB ) {
        format = GL_RGB;
        type = GL_FLOAT;
    }

    if( internalformat == GL_RGBA32F ) {
        format = GL_RGBA;
        type = GL_FLOAT;
    }

    if( internalformat == GL_RGB32F ) {
        format = GL_RGB;
        type = GL_FLOAT;
    }

    glGenTextures(1, &texID);
    glBindFramebuffer(GL_FRAMEBUFFER, *m_fboID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexImage2D(GL_TEXTURE_2D, 0, internalformat, m_width,
                 m_height, 0, format, type, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);

    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment,
                           GL_TEXTURE_2D, texID, 0);

    m_colorTexID.push_back(CGoGNGLuint(texID));
    (*m_colorAttachmentPoints)[m_colorTexID.size() - 1] = attachment;
}

void FBO::attachDepthTexture(GLint filter)
{
    GLenum attachment;
    GLenum internalFormat;
    GLenum format;
    GLenum type;
    GLuint texID;

    if( int(m_depthTexID.size()) == 1 ) {
        std::cout << "FBO::attachDepthTexture : Only one depth texture can be attached" << std::endl;
        return;
    }

    attachment = GL_DEPTH_ATTACHMENT;
    internalFormat = GL_DEPTH_COMPONENT24;
    format = GL_RGB;
    type = GL_FLOAT;

    glGenTextures(1, &texID);
    glBindFramebuffer(GL_FRAMEBUFFER, *m_fboID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width,
                 m_height, 0, format, type, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);

    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment,
                           GL_TEXTURE_2D, texID, 0);

    m_depthTexID.push_back(CGoGNGLuint(texID));
}

void FBO::bindColorTexInput()
{
    for( unsigned int i = 0; i < m_colorTexID.size(); ++i )
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, *(m_colorTexID[i]));
    }
}

void FBO::bindColorTexInput(int num)
{
    glBindTexture(GL_TEXTURE_2D, *(m_colorTexID[num]));
}

void FBO::bindColorTexOutput()
{
    glBindFramebuffer(GL_FRAMEBUFFER, *m_fboID);

    if( m_colorTexID.size() == 1 ) {
        glDrawBuffer((*m_colorAttachmentPoints)[0]); // TODO : Pourquoi pas glDrawBuffers pour m_colorTexID.size == 1 ?
    }

    else {
        glDrawBuffers(m_colorTexID.size(), *m_colorAttachmentPoints);
    }
}

void FBO::bindColorTexOutput(int num)
{
    glBindFramebuffer(GL_FRAMEBUFFER, *m_fboID);
    glDrawBuffer((*m_colorAttachmentPoints)[num]);
}

void FBO::bindDepthTexInput()
{
    if (m_depthTexID.size() != 0)
        glBindTexture(GL_TEXTURE_2D, *(m_depthTexID)[1]);
}

void FBO::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FBO::checkFBO()
{
    GLenum status;

    glBindFramebuffer(GL_FRAMEBUFFER, *m_fboID);
    status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if(status != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "FBO STATUS ERROR : " << status << std::endl;
    }
}

}
}

