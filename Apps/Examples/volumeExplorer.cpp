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


#include "volumeExplorer.h"
#include <iostream>

#include "Algo/Modelisation/primitives3d.h"
#include "Algo/Modelisation/polyhedron.h"
#include "Algo/Import/import.h"
#include "Algo/Geometry/volume.h"

#include "Utils/chrono.h"
#include "Utils/debug.h"

PFP::MAP myMap;
VertexAttribute<PFP::VEC3> position ;
VolumeAttribute<PFP::VEC4> color ;

template<typename PFP>
static int computeDepths(typename PFP::MAP& map, const FunctorSelect& good, std::vector<int> & depths)
{
	TraversorCell<typename PFP::MAP, PFP::MAP::FACE_OF_PARENT> modelFaces(map, good);

	int depth = 0;
	bool notDone = false;

	for (Dart d = modelFaces.begin(); d != modelFaces.end(); d = modelFaces.next())
	{
		if (map.isBoundaryFace(d))
		{
			notDone = true;
			Traversor3WF<typename PFP::MAP> cellFaces(map, d);

			for (Dart e = cellFaces.begin(); e != cellFaces.end(); e = cellFaces.next())
			{
				Traversor3FE<typename PFP::MAP> faceEdges(map, e);

				for (Dart f = faceEdges.begin(); f != faceEdges.end(); f = faceEdges.next())
				{
					depths[f.label()] = depth;
				}
			}
		}
	}

	++depth;

	while (notDone)
	{
		notDone = false;

		for (Dart d = modelFaces.begin(); d != modelFaces.end(); d = modelFaces.next())
		{
			if (depths[d.label()] == -1 && depths[map.phi3(d).label()] == depth - 1)
			{
				notDone = true;
				Traversor3WF<typename PFP::MAP> cellFaces(map, d);

				for (Dart e = cellFaces.begin(); e != cellFaces.end(); e = cellFaces.next())
				{
					Traversor3FE<typename PFP::MAP> faceEdges(map, e);

					for (Dart f = faceEdges.begin(); f != faceEdges.end(); f = faceEdges.next())
					{
						depths[f.label()] = depth;
					}
				}
			}
		}

		if (notDone)
		{
			++depth;
		}
	}

	return depth - 1;
}

template<typename VEC3>
static VEC3 const & jetColor3(int const i, int const iMax)
{
	static VEC3 const defaultColor = VEC3(1, 1, 1);
	static VEC3 const colors[] = {
			VEC3(0.0, 0.0, 0.5),
			VEC3(0.0, 0.0, 0.6),
			VEC3(0.0, 0.0, 0.7),
			VEC3(0.0, 0.0, 0.8),
			VEC3(0.0, 0.0, 0.9),
			VEC3(0.0, 0.0, 1.0),
			VEC3(0.0, 0.1, 1.0),
			VEC3(0.0, 0.2, 1.0),
			VEC3(0.0, 0.3, 1.0),
			VEC3(0.0, 0.4, 1.0),
			VEC3(0.0, 0.5, 1.0),
			VEC3(0.0, 0.6, 1.0),
			VEC3(0.0, 0.7, 1.0),
			VEC3(0.0, 0.8, 1.0),
			VEC3(0.0, 0.9, 1.0),
			VEC3(0.0, 1.0, 1.0),
			VEC3(0.1, 1.0, 0.9),
			VEC3(0.2, 1.0, 0.8),
			VEC3(0.3, 1.0, 0.7),
			VEC3(0.4, 1.0, 0.6),
			VEC3(0.5, 1.0, 0.5),
			VEC3(0.6, 1.0, 0.4),
			VEC3(0.7, 1.0, 0.3),
			VEC3(0.8, 1.0, 0.2),
			VEC3(0.9, 1.0, 0.1),
			VEC3(1.0, 1.0, 0.0),
			VEC3(1.0, 0.9, 0.0),
			VEC3(1.0, 0.8, 0.0),
			VEC3(1.0, 0.7, 0.0),
			VEC3(1.0, 0.6, 0.0),
			VEC3(1.0, 0.5, 0.0),
			VEC3(1.0, 0.4, 0.0),
			VEC3(1.0, 0.3, 0.0),
			VEC3(1.0, 0.2, 0.0),
			VEC3(1.0, 0.1, 0.0),
			VEC3(1.0, 0.0, 0.0),
			VEC3(0.9, 0.0, 0.0),
			VEC3(0.8, 0.0, 0.0),
			VEC3(0.7, 0.0, 0.0),
			VEC3(0.6, 0.0, 0.0),
			VEC3(0.5, 0.0, 0.0),
	};
	static unsigned int const lastColorIndex = sizeof(colors) / sizeof(VEC3) - 1;

	return i < 0 ? defaultColor : colors[iMax <= 0 ? lastColorIndex : i * lastColorIndex / iMax];
}

template<typename VEC4>
static VEC4 const & jetColor4(int const i, int const iMax)
{
	static VEC4 const defaultColor = VEC4(1, 1, 1, 1);
	static VEC4 const colors[] = {
			VEC4(0.0, 0.0, 0.5, 1.0),
			VEC4(0.0, 0.0, 0.6, 1.0),
			VEC4(0.0, 0.0, 0.7, 1.0),
			VEC4(0.0, 0.0, 0.8, 1.0),
			VEC4(0.0, 0.0, 0.9, 1.0),
			VEC4(0.0, 0.0, 1.0, 1.0),
			VEC4(0.0, 0.1, 1.0, 1.0),
			VEC4(0.0, 0.2, 1.0, 1.0),
			VEC4(0.0, 0.3, 1.0, 1.0),
			VEC4(0.0, 0.4, 1.0, 1.0),
			VEC4(0.0, 0.5, 1.0, 1.0),
			VEC4(0.0, 0.6, 1.0, 1.0),
			VEC4(0.0, 0.7, 1.0, 1.0),
			VEC4(0.0, 0.8, 1.0, 1.0),
			VEC4(0.0, 0.9, 1.0, 1.0),
			VEC4(0.0, 1.0, 1.0, 1.0),
			VEC4(0.1, 1.0, 0.9, 1.0),
			VEC4(0.2, 1.0, 0.8, 1.0),
			VEC4(0.3, 1.0, 0.7, 1.0),
			VEC4(0.4, 1.0, 0.6, 1.0),
			VEC4(0.5, 1.0, 0.5, 1.0),
			VEC4(0.6, 1.0, 0.4, 1.0),
			VEC4(0.7, 1.0, 0.3, 1.0),
			VEC4(0.8, 1.0, 0.2, 1.0),
			VEC4(0.9, 1.0, 0.1, 1.0),
			VEC4(1.0, 1.0, 0.0, 1.0),
			VEC4(1.0, 0.9, 0.0, 1.0),
			VEC4(1.0, 0.8, 0.0, 1.0),
			VEC4(1.0, 0.7, 0.0, 1.0),
			VEC4(1.0, 0.6, 0.0, 1.0),
			VEC4(1.0, 0.5, 0.0, 1.0),
			VEC4(1.0, 0.4, 0.0, 1.0),
			VEC4(1.0, 0.3, 0.0, 1.0),
			VEC4(1.0, 0.2, 0.0, 1.0),
			VEC4(1.0, 0.1, 0.0, 1.0),
			VEC4(1.0, 0.0, 0.0, 1.0),
			VEC4(0.9, 0.0, 0.0, 1.0),
			VEC4(0.8, 0.0, 0.0, 1.0),
			VEC4(0.7, 0.0, 0.0, 1.0),
			VEC4(0.6, 0.0, 0.0, 1.0),
			VEC4(0.5, 0.0, 0.0, 1.0),
	};
	static unsigned int const lastColorIndex = sizeof(colors) / sizeof(VEC4) - 1;

	return i < 0 ? defaultColor : colors[iMax <= 0 ? lastColorIndex : i * lastColorIndex / iMax];
}

template<typename PFP>
static void computeColorsUsingDepths(typename PFP::MAP & map, VolumeAttribute<typename PFP::VEC4> & colorPerXXX,
		float const opacity, float const opacityGradient, FunctorSelect const & good,
		std::vector<int> const & depths, int const depthEnd)
{
//	std::vector<int> depths(map.getNbDarts(), -1);
//	int const depthEnd = computeDepths<PFP>(map, good, depths);
	TraversorCell<typename PFP::MAP, PFP::MAP::FACE_OF_PARENT> faces(map, good);

	for (Dart d = faces.begin(); d != faces.end(); d = faces.next())
	{
		int const depth = depths[d.label()];
		float alpha = opacity;

		if (opacityGradient < 0.5)
		{
			alpha *= pow(opacityGradient * 2.0, depthEnd - depth);
		}
		else
		{
			alpha *= pow((1.0 - opacityGradient) * 2.0, depth);
		}

		Dart a = d;
		Dart b = map.phi1(a);
		Dart c = map.phi1(b);
		colorPerXXX[a] = jetColor4<typename PFP::VEC4>(depth, depthEnd);
		colorPerXXX[a][3] = alpha;
		colorPerXXX[b] = jetColor4<typename PFP::VEC4>(depth, depthEnd);
		colorPerXXX[b][3] = alpha;
		colorPerXXX[c] = jetColor4<typename PFP::VEC4>(depth, depthEnd);
		colorPerXXX[c][3] = alpha;
	}
}

void MyQT::volumes_onoff(bool x)
{
	render_volumes = !render_volumes;
	updateGL();
}

void MyQT::edges_onoff(bool x)
{
	render_edges = !render_edges;
	updateGL();
}

void MyQT::topo_onoff(bool x)
{
	render_topo = !render_topo;
	if (render_topo)
	{
		SelectorDartNoBoundary<PFP::MAP> nb(::myMap);
		m_topo_render->updateData<PFP>(::myMap, position, 0.8f, m_explode_factorf-0.05f, m_explode_factor, nb);
	}

	updateGL();
}

void MyQT::clipping_onoff(bool x)
{
	clip_volume = x;

	if (clip_volume)
	{
		Geom::Vec3f pos = m_PlanePick->getPosition();
		float pipo;
		Geom::Vec3f normal = m_PlanePick->getAxisScale(2, pipo); // 2 = Z axis = plane normal
		float d = -(pos*normal);
		m_explode_render->setClippingPlane(Geom::Vec4f(normal[0],normal[1],normal[2],d));
		m_topo_render->shader1()->setClipPlaneParamsAll(clip_id1, normal, pos);
		m_topo_render->shader2()->setClipPlaneParamsAll(clip_id2, normal, pos);
	}
	else
	{
		m_explode_render->setNoClippingPlane();
		m_topo_render->shader1()->setClipPlaneParamsAll(clip_id1, Geom::Vec3f(0,0,1), Geom::Vec3f(0,0,999999.9f));
		m_topo_render->shader2()->setClipPlaneParamsAll(clip_id2, Geom::Vec3f(0,0,1), Geom::Vec3f(0,0,999999.9f));
		m_topo_render->shader1()->setClipColorAttenuationFactorRelative(0.0f,0.0f);
		m_topo_render->shader2()->setClipColorAttenuationFactorRelative(0.0f,0.0f);
	}
	updateGL();
}

void MyQT::hide_onoff(bool x)
{
	hide_clipping = !hide_clipping;
	updateGL();
}


void MyQT::slider_explode(int x)
{
	m_explode_factor = 0.01f*(x+1)-0.0001f;
	m_explode_render->setExplodeVolumes(m_explode_factor);
	updateGL();
}

void MyQT::slider_explodeF(int x)
{
	m_explode_factorf = 0.01f*(x+1);
	m_explode_render->setExplodeFaces(m_explode_factorf);
	updateGL();
}


void MyQT::slider_pressed()
{
	render_topoTemp = render_topo;
	render_topo = false;
	updateGL();
}


void MyQT::slider_released()
{

	render_topo = render_topoTemp;
	if (render_topo)
	{
		SelectorDartNoBoundary<PFP::MAP> nb(::myMap);
		m_topo_render->updateData<PFP>(::myMap, position, 0.8f, m_explode_factorf-0.05f, m_explode_factor, nb);
	}
	updateGL();
}

void MyQT::slider_opacity(int const x)
{
	m_opacity = 0.01f * x;
	m_explode_render->setAlpha(m_opacity); DEBUG_GL;
	computeColorsUsingDepths<PFP>(::myMap, ::color, m_opacity, m_opacity_gradient, allDarts, m_depths, m_depthEnd);
	m_explode_render->updateData<PFP>(::myMap, position, color);
	updateGL(); DEBUG_GL;
}

void MyQT::slider_opacity_gradient(int const x)
{
	m_opacity_gradient = 0.01f * x;
	computeColorsUsingDepths<PFP>(::myMap, ::color, m_opacity, m_opacity_gradient, allDarts, m_depths, m_depthEnd);
	m_explode_render->updateData<PFP>(::myMap, position, color);
	updateGL(); DEBUG_GL;
}


void MyQT::cb_Open()
{
	std::string filters("all (*.*);; trian (*.trian);; ctm (*.ctm);; off (*.off);; ply (*.ply)") ;
	std::string filename = selectFile("Open Mesh", "", filters) ;
	if (filename.empty())
		return ;

	::myMap.clear(true);

	std::vector<std::string> attrNames ;

	size_t pos = filename.rfind(".");    // position of "." in filename
	std::string extension = filename.substr(pos);

	if(extension == std::string(".tet"))
	{
		if(!Algo::Import::importTet<PFP>(::myMap,filename,attrNames))
		{
			CGoGNerr << "could not import " << filename << CGoGNendl ;
			return;
		}
		else
			position = ::myMap.getAttribute<PFP::VEC3, VERTEX>(attrNames[0]) ;
	}

	if(extension == std::string(".node"))
	{
		if(!Algo::Import::importMeshV<PFP>(::myMap, filename, attrNames, Algo::Import::ImportVolumique::NODE))
		{
			std::cerr << "could not import " << filename << std::endl ;
			return ;
		}
		else
			position = ::myMap.getAttribute<PFP::VEC3,VERTEX>(attrNames[0]) ;
	}


	if(extension == std::string(".off"))
	{
		if(!Algo::Import::importMeshToExtrude<PFP>(::myMap, filename, attrNames))
		{
			std::cerr << "could not import " << filename << std::endl ;
			return ;
		}
		else
		{
			position = ::myMap.getAttribute<PFP::VEC3, VERTEX>(attrNames[0]) ;
			::myMap.closeMap();
		}
	}

	::color = ::myMap.addAttribute<PFP::VEC4, VOLUME>("color");

	TraversorCell<PFP::MAP, VOLUME> tra(::myMap);
	float maxV = 0.0f;
	for (Dart d = tra.begin(); d != tra.end(); d = tra.next())
	{
		float v = Algo::Geometry::tetrahedronVolume<PFP>(::myMap, d, position);
		::color[d] = PFP::VEC4(v,0,0,1);
		if (v>maxV)
			maxV=v;
	}
	for (unsigned int i = ::color.begin(); i != ::color.end(); ::color.next(i))
	{
		::color[i][0] /= maxV;
		::color[i][2] = 1.0f - ::color[i][0];
	}

	SelectorDartNoBoundary<PFP::MAP> nb(::myMap);
	m_topo_render->updateData<PFP>(::myMap, position,  0.8f, 0.8f, 0.8f, nb);
	updateDepths();
	computeColorsUsingDepths<PFP>(::myMap, ::color, m_opacity, m_opacity_gradient, allDarts, m_depths, m_depthEnd);
	m_explode_render->updateData<PFP>(::myMap, position, ::color);

	updateGL() ;
}


void MyQT::cb_initGL()
{
	// choose to use GL version 2
	Utils::GLSLShader::setCurrentOGLVersion(2);

	// create the renders
    m_topo_render = new Algo::Render::GL2::Topo3Render();
    m_explode_render = new Algo::Render::GL2::ExplodeVolumeAlphaRender(true,true);

	SelectorDartNoBoundary<PFP::MAP> nb(::myMap);
	m_topo_render->updateData<PFP>(::myMap, position,  0.8f, 0.8f, 0.8f, nb);
	updateDepths();
	computeColorsUsingDepths<PFP>(::myMap, ::color, m_opacity, m_opacity_gradient, allDarts, m_depths, m_depthEnd);
	m_explode_render->updateData<PFP>(::myMap, position, color);
	m_explode_render->setExplodeVolumes(0.8f);
	m_explode_render->setExplodeFaces(0.9f);
	m_explode_render->setAmbiant(Geom::Vec4f(0.2f,0.2f,0.2f,1.0f));
	m_explode_render->setBackColor(Geom::Vec4f(0.9f,0.9f,0.9f,1.0f));
	m_explode_render->setColorLine(Geom::Vec4f(0.8f,0.2f,0.2f,1.0f));
	m_explode_render->setAlpha(m_opacity);

	registerShader(m_explode_render->shaderFaces());
	registerShader(m_explode_render->shaderLines());

    m_PlanePick = new Utils::Pickable(Utils::Pickable::GRID,1);
	m_frame = new Utils::FrameManipulator();
	m_frame->setSize(m_WidthObj/2.0f);


	m_topo_render->shader1()->insertClippingCode();
	m_topo_render->shader2()->insertClippingCode();

	clip_id1 = m_topo_render->shader1()->addClipPlane();
	clip_id2 = m_topo_render->shader2()->addClipPlane();

	m_topo_render->shader1()->setClipPlaneParamsAll(clip_id1, Geom::Vec3f(0,0,1), m_PosObj);
	m_topo_render->shader2()->setClipPlaneParamsAll(clip_id2, Geom::Vec3f(0,0,1), m_PosObj);
	m_explode_render->setClippingPlane(Geom::Vec4f(0,0,1,m_PosObj*Geom::Vec3f(0,0,-1)));

}

static inline float square(float const x)
{
	return x * x;
}

static inline float squaredNorm(float const x, float const y, float const z)
{
	return square(x) + square(y) + square(z);
}

static glm::vec4 viewpoint;

class Comparator
{
	static std::vector<float> & distances()
	{
		static std::vector<float> result;

		return result;
	}
public:
	Comparator(float const * const colorBufferWithFaceCenters, float const * const vertexBufferWithVolumeCenters, unsigned int const elementCount)
	{
		distances().resize(elementCount / 4);

		for (unsigned int i = 0; i < distances().size(); ++i)
		{
			distances()[i] = squaredNorm(
				(colorBufferWithFaceCenters[4 * 4 * i + 0] + vertexBufferWithVolumeCenters[4 * 3 * i + 0]) / 2 - ::viewpoint.x,
				(colorBufferWithFaceCenters[4 * 4 * i + 1] + vertexBufferWithVolumeCenters[4 * 3 * i + 1]) / 2 - ::viewpoint.y,
				(colorBufferWithFaceCenters[4 * 4 * i + 2] + vertexBufferWithVolumeCenters[4 * 3 * i + 2]) / 2 - ::viewpoint.z);
		}
	}

	bool operator() (unsigned int const i, unsigned int const j) const
	{
		return distances()[i] > distances()[j];
	}
};

static void sortData(Algo::Render::GL2::ExplodeVolumeAlphaRender const * const evr, std::vector<unsigned int> & permutation, std::vector<GLuint> & triangles)
{
	unsigned int const n = evr->nbTris();

	permutation.resize(n);

	for (GLuint i = 0; i < n; ++i)
	{
		permutation[i] = i;
	}

	Utils::VBO const * const colorVBO = evr->colors();
	Utils::VBO const * const vertexVBO = evr->vertices();

	float const * const colors = static_cast<float const *>(colorVBO->lockPtr());
	float const * const vertices = static_cast<float const *>(vertexVBO->lockPtr());

	if (colors && vertices)
	{
		assert(colorVBO->nbElts() == vertexVBO->nbElts());

		std::sort(permutation.begin(), permutation.end(), Comparator(colors, vertices, colorVBO->nbElts()));
	}

	if (colors)
	{
		colorVBO->releasePtr();
	}

	if (vertices)
	{
		vertexVBO->releasePtr();
	}

	triangles.resize(n * 4);

	for (GLuint i = 0; i < n * 4; i += 4)
	{
		for (int j = 0; j < 4; ++j)
		{
			triangles[i + j] = permutation[i / 4] * 4 + j;
		}
	}
}



void MyQT::cb_redraw()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); DEBUG_GL;

	glEnable(GL_POLYGON_OFFSET_FILL); DEBUG_GL;
	glPolygonOffset(1.0f, 1.0f); DEBUG_GL;

	if (render_topo)
		m_topo_render->drawTopo(); DEBUG_GL;

	if (render_edges)
	{
		glLineWidth(2.0f);
		m_explode_render->drawEdges(); DEBUG_GL;
	}

	glDisable(GL_POLYGON_OFFSET_FILL);

	if (render_volumes)
	{
		if (m_opacity < 1)
		{
			::viewpoint = glm::inverse(modelViewMatrix()) * glm::vec4(0.0, 0.0, 0.0, 1.0);
			unsigned int const n = m_explode_render->nbTris();
			static std::vector<unsigned int> indices(n);
			static std::vector<GLuint> triangles(n * 4);

			sortData(m_explode_render, indices, triangles);
			glEnable(GL_BLEND); DEBUG_GL;
			glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD); DEBUG_GL;
			glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO); DEBUG_GL;
			m_explode_render->shaderFaces()->enableVertexAttribs();
			glDrawElements(GL_LINES_ADJACENCY_EXT, n * 4, GL_UNSIGNED_INT, &triangles[0]); DEBUG_GL;
			m_explode_render->shaderFaces()->disableVertexAttribs();
			glDisable(GL_BLEND); DEBUG_GL;
		}
		else
		{
			m_explode_render->drawFaces();
		}
	}

	if (clip_volume && !hide_clipping)
	{
		m_frame->draw(); DEBUG_GL;
		m_PlanePick->draw(); DEBUG_GL;
	}
}


void  MyQT::cb_mousePress(int button, int x, int y)
{
	if (!Shift())
		return;

	if (hide_clipping || !clip_volume)
		return;

	m_begX = x;
	m_begY = y;

	// get ray of selection
	Geom::Vec3f rayA,rayB;
	float dist = getOrthoScreenRay(x,y,rayA,rayB);
	Geom::Vec3f AB = rayB-rayA;

	unsigned int fr_picked =0;
	// picking the frame -> axis
	fr_picked = m_frame->pick(rayA,AB,dist);

	if (fr_picked != 0)
	{
		m_pickedAxis=fr_picked;
		m_frame->highlight(m_pickedAxis);
		m_frame->storeProjection(m_pickedAxis);
		updateGL();
	}
}

void  MyQT::cb_mouseRelease(int button, int x, int y)
{

	if (hide_clipping || !clip_volume)
		return;

	m_pickedAxis=0;
	m_frame->highlight(m_pickedAxis);
	updateGL();

}

void  MyQT::cb_mouseMove(int buttons, int x, int y)
{
	if (!Shift())
		return;

	if (hide_clipping || !clip_volume)
		return;

	// rotation selected ?
	if (Utils::FrameManipulator::rotationAxis(m_pickedAxis))
	{
		if (buttons&1)
		{
			float angle = m_frame->angleFromMouse(x,y,x-m_begX, y-m_begY);
			m_frame->rotate(m_pickedAxis, angle);
		}
		else if (buttons&2)
			m_frame->rotateInScreen(x-m_begX, y-m_begY);

		m_PlanePick->transfo() = m_frame->transfo();
	}
	// translation selected
	else if (Utils::FrameManipulator::translationAxis(m_pickedAxis))
	{
		if (buttons&1)
		{
			float dist =  m_frame->distanceFromMouse(x-m_begX, y-m_begY);
			m_frame->translate(m_pickedAxis, dist);
		}
		else if (buttons&2)
			m_frame->translateInScreen(x-m_begX, y-m_begY);

		m_PlanePick->transfo() = m_frame->transfo();
	}
	// scale selected
	else if (Utils::FrameManipulator::scaleAxis(m_pickedAxis) )
	{
		float scale = m_frame->scaleFromMouse(x-m_begX, y-m_begY);
		m_frame->scale(m_pickedAxis, scale );
		m_PlanePick->transfo() = m_frame->transfo();
	}

	Geom::Vec3f pos = m_PlanePick->getPosition();
	float pipo;
	Geom::Vec3f normal = m_PlanePick->getAxisScale(2, pipo); // 2 = Z axis = plane normal
	float d = -(pos*normal);
	m_explode_render->setClippingPlane(Geom::Vec4f(normal[0],normal[1],normal[2],d));

	m_topo_render->shader1()->setClipPlaneParamsAll(clip_id1, normal, pos);
	m_topo_render->shader2()->setClipPlaneParamsAll(clip_id2, normal, pos);

	m_begX = x;
	m_begY = y;
	updateGL();
	return;

}

void MyQT::updateDepths()
{
	m_depths.clear();
	m_depths.resize(::myMap.getNbDarts(), -1);
	m_depthEnd = computeDepths<PFP>(::myMap, allDarts, m_depths);
}

int main(int argc, char **argv)
{
	if (argc>1)
	{
		std::vector<std::string> attrNames ;
		std::string filename(argv[1]);
		size_t pos = filename.rfind(".");    // position of "." in filename
		std::string extension = filename.substr(pos);

		if(extension == std::string(".tet"))
		{
			if(!Algo::Import::importTet<PFP>(::myMap,argv[1],attrNames))
			{
				CGoGNerr << "could not import " << argv[1] << CGoGNendl ;
				return 1;
			}
			else
				position = ::myMap.getAttribute<PFP::VEC3, VERTEX>(attrNames[0]) ;
		}

		if(extension == std::string(".node"))
		{
			if(!Algo::Import::importMeshV<PFP>(::myMap, argv[1], attrNames, Algo::Import::ImportVolumique::NODE))
			{
				std::cerr << "could not import " << argv[1] << std::endl ;
				return 1;
			}
			else
				position = ::myMap.getAttribute<PFP::VEC3,VERTEX>(attrNames[0]) ;
		}


		if(extension == std::string(".off"))
		{
			if(!Algo::Import::importMeshToExtrude<PFP>(::myMap, argv[1], attrNames))
			{
				std::cerr << "could not import "  << std::endl ;
				return 1;
			}
			else
			{
				position = ::myMap.getAttribute<PFP::VEC3, VERTEX>(attrNames[0]) ;
				::myMap.closeMap();
			}
		}

		::color = ::myMap.addAttribute<PFP::VEC4, VOLUME>("color");

		TraversorCell<PFP::MAP, VOLUME> tra(::myMap);
		float maxV = 0.0f;
		for (Dart d = tra.begin(); d != tra.end(); d = tra.next())
		{
			float v = Algo::Geometry::tetrahedronVolume<PFP>(::myMap, d, position);
			::color[d] = PFP::VEC4(v,0,0,1);
			if (v>maxV)
				maxV=v;
		}
		for (unsigned int i = ::color.begin(); i != ::color.end(); ::color.next(i))
		{
			::color[i][0] /= maxV;
			::color[i][2] = 1.0f - color[i][0];
		}
	}
	else
	{
		position = ::myMap.addAttribute<PFP::VEC3, VERTEX>("position");
		Algo::Modelisation::Primitive3D<PFP> prim(::myMap, position);
		int nb = 8;
		prim.hexaGrid_topo(nb,nb,nb);
		prim.embedHexaGrid(1.0f,1.0f,1.0f);

		::color = ::myMap.addAttribute<PFP::VEC4, VOLUME>("color");
		TraversorW<PFP::MAP> tra(::myMap);
		for (Dart d = tra.begin(); d != tra.end(); d = tra.next())
		{
			PFP::VEC3 tmp = position[d] + PFP::VEC3(0.5,0.5,0.5);
			::color[d] = PFP::VEC4(tmp[0], tmp[1], tmp[2], 1.0);
		}
	}

    // un peu d'interface
	QApplication app(argc, argv);
	MyQT sqt;

	// interface de tuto5.ui
    Utils::QT::uiDockInterface dock;
    sqt.setDock(&dock);


	//  bounding box
    Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(::myMap, position);
    sqt.m_WidthObj = std::max<PFP::REAL>(std::max<PFP::REAL>(bb.size(0), bb.size(1)), bb.size(2));
    sqt.m_PosObj = (bb.min() +  bb.max()) / PFP::REAL(2);

    // envoit info BB a l'interface
	sqt.setParamObject(sqt.m_WidthObj, sqt.m_PosObj.data());

	sqt.setCallBack( dock.checkBox_volumes, SIGNAL(toggled(bool)), SLOT(volumes_onoff(bool)) );
	sqt.setCallBack( dock.checkBox_edges, SIGNAL(toggled(bool)), SLOT(edges_onoff(bool)) );
	sqt.setCallBack( dock.checkBox_topo, SIGNAL(toggled(bool)), SLOT(topo_onoff(bool)) );

	sqt.setCallBack( dock.checkBox_hide, SIGNAL(toggled(bool)), SLOT(hide_onoff(bool)) );
	sqt.setCallBack( dock.checkBox_plane, SIGNAL(toggled(bool)), SLOT(clipping_onoff(bool)) );

	sqt.setCallBack( dock.slider_explode, SIGNAL(valueChanged(int)), SLOT(slider_explode(int)) );
	sqt.setCallBack( dock.slider_explode, SIGNAL(sliderPressed()), SLOT(slider_pressed()) );
	sqt.setCallBack( dock.slider_explode, SIGNAL(sliderReleased()), SLOT(slider_released()) );

	sqt.setCallBack( dock.slider_explode_face, SIGNAL(valueChanged(int)), SLOT(slider_explodeF(int)) );
	sqt.setCallBack( dock.slider_explode_face, SIGNAL(sliderPressed()), SLOT(slider_pressed()) );
	sqt.setCallBack( dock.slider_explode_face, SIGNAL(sliderReleased()), SLOT(slider_released()) );

	sqt.setCallBack( dock.slider_opacity, SIGNAL(valueChanged(int)), SLOT(slider_opacity(int)) );
	sqt.setCallBack( dock.slider_opacity, SIGNAL(sliderPressed()), SLOT(slider_pressed()) );
	sqt.setCallBack( dock.slider_opacity, SIGNAL(sliderReleased()), SLOT(slider_released()) );

	sqt.setCallBack( dock.slider_opacity_gradient, SIGNAL(valueChanged(int)), SLOT(slider_opacity_gradient(int)) );
	sqt.setCallBack( dock.slider_opacity_gradient, SIGNAL(sliderPressed()), SLOT(slider_pressed()) );
	sqt.setCallBack( dock.slider_opacity_gradient, SIGNAL(sliderReleased()), SLOT(slider_released()) );


	sqt.show();
	dock.slider_explode->setValue(80);
	dock.slider_explode_face->setValue(80);
	sqt.clipping_onoff(true);

	Utils::Chrono ch;

	std::cout << "Compute Volume ->"<< std::endl;
	ch.start();
	float vol = Algo::Geometry::totalVolume<PFP>(::myMap, position);
	vol += Algo::Geometry::totalVolume<PFP>(::myMap, position);
	vol += Algo::Geometry::totalVolume<PFP>(::myMap, position);
	vol += Algo::Geometry::totalVolume<PFP>(::myMap, position);
	std::cout << ch.elapsed()<< " ms  val="<<vol<< std::endl;

	ch.start();
	vol = Algo::Geometry::Parallel::totalVolume<PFP>(::myMap, position);
	vol += Algo::Geometry::Parallel::totalVolume<PFP>(::myMap, position);
	vol += Algo::Geometry::Parallel::totalVolume<PFP>(::myMap, position);
	vol += Algo::Geometry::Parallel::totalVolume<PFP>(::myMap, position);
	std::cout << ch.elapsed()<< " ms //  val="<<vol<< std::endl;


	// et on attend la fin.
	return app.exec();
}
