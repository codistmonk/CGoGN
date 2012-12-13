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

#ifndef __CGOGN_SHADER_EXPLODE_VOLUMES_ALPHA__
#define __CGOGN_SHADER_EXPLODE_VOLUMES_ALPHA__

#include "Utils/GLSLShader.h"
#include "Geometry/vector_gen.h"

namespace CGoGN
{

namespace Utils
{

class ShaderExplodeVolumesAlpha : public GLSLShader
{
protected:
	// shader sources
    static std::string vertexShaderText;
    static std::string fragmentShaderText;
    static std::string geometryShaderText;

    // uniform locations
	CGoGNGLuint m_unif_ambient;
	CGoGNGLuint m_unif_backColor;
	CGoGNGLuint m_unif_lightPos;
	CGoGNGLuint m_unif_plane;
	CGoGNGLuint m_unif_alpha;
	CGoGNGLuint m_unif_unit;
	CGoGNGLuint m_unif_depthPeeling;

//	local storage for uniforms
	float m_explodeV;
	float m_explodeF;
	Geom::Vec4f m_ambient;
	Geom::Vec4f m_backColor;
	Geom::Vec3f m_light_pos;
	Geom::Vec4f m_plane;
	int m_depthPeeling;

	// VBO
	VBO* m_vboPos;
	VBO* m_vboColors;

	bool m_wcpf;

	void getLocations();

	void restoreUniformsAttribs();

public:
	ShaderExplodeVolumesAlpha(bool withColorPerFace=false);

	void setAmbient(const Geom::Vec4f& ambient);

	void setBackColor(const Geom::Vec4f& backColor);

	void setLightPosition(const Geom::Vec3f& lp);

	void setClippingPlane(const Geom::Vec4f& plane);

	void setDepthPeeling(bool depthPeeling);

	void setAttributePosition(VBO* vbo);

	void setAttributeColor(VBO* vbo);
};

} // namespace Utils

} // namespace CGoGN

#endif
