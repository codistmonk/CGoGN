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



#include <iostream>

#include "volumeExplorer.h"
#include "volumeExplorerTools.h"

#include "Algo/Modelisation/primitives3d.h"
#include "Algo/Modelisation/polyhedron.h"
#include "Algo/Import/import.h"
#include "Algo/Geometry/volume.h"

#include "Utils/chrono.h"
#include "Utils/debug.h"

using namespace CGoGN::VolumeExplorerTools;

PFP::MAP myMap;
VertexAttribute<PFP::VEC3> position ;
VolumeAttribute<PFP::VEC4> color ;

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
		m_topo_render->updateData<PFP>(::myMap, ::position, 0.8f, m_explode_factorf-0.05f, m_explode_factor, nb);
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
		m_topo_render->updateData<PFP>(::myMap, ::position, 0.8f, m_explode_factorf-0.05f, m_explode_factor, nb);
	}
	updateGL();
}

void MyQT::slider_opacity(int const x)
{
	m_opacity = 0.01f * x;
	m_explode_render->setAlpha(m_opacity); DEBUG_GL;
	computeColorsUsingDepths<PFP>(::myMap, ::color, m_opacity, m_opacity_gradient, allDarts, m_depths, m_lastDepth);
	m_explode_render->updateData<PFP>(::myMap, ::position, color);
	updateGL(); DEBUG_GL;
}

void MyQT::slider_opacity_gradient(int const x)
{
	m_opacity_gradient = 0.01f * x;
	computeColorsUsingDepths<PFP>(::myMap, ::color, m_opacity, m_opacity_gradient, allDarts, m_depths, m_lastDepth);
	m_explode_render->updateData<PFP>(::myMap, ::position, color);
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

	size_t pos = filename.rfind(".");    // ::position of "." in filename
	std::string extension = filename.substr(pos);

	if(extension == std::string(".tet"))
	{
		if(!Algo::Import::importTet<PFP>(::myMap,filename,attrNames))
		{
			CGoGNerr << "could not import " << filename << CGoGNendl ;
			return;
		}
		else
			::position = ::myMap.getAttribute<PFP::VEC3, VERTEX>(attrNames[0]) ;
	}

	if(extension == std::string(".node"))
	{
		if(!Algo::Import::importMeshV<PFP>(::myMap, filename, attrNames, Algo::Import::ImportVolumique::NODE))
		{
			std::cerr << "could not import " << filename << std::endl ;
			return ;
		}
		else
			::position = ::myMap.getAttribute<PFP::VEC3,VERTEX>(attrNames[0]) ;
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
			::position = ::myMap.getAttribute<PFP::VEC3, VERTEX>(attrNames[0]) ;
			::myMap.closeMap();
		}
	}

	::color = ::myMap.addAttribute<PFP::VEC4, VOLUME>("color");

	TraversorCell<PFP::MAP, VOLUME> tra(::myMap);
	float maxV = 0.0f;
	for (Dart d = tra.begin(); d != tra.end(); d = tra.next())
	{
		float v = Algo::Geometry::tetrahedronVolume<PFP>(::myMap, d, ::position);
		::color[d] = PFP::VEC4(v,0,0,1);
		if (v>maxV)
			maxV=v;
	}
	for (unsigned int i = ::color.begin(); i != ::color.end(); ::color.next(i))
	{
		::color[i][0] /= maxV;
		::color[i][2] = 1.0f - ::color[i][0];
	}

	centerModel<PFP>(::myMap, ::position);

	SelectorDartNoBoundary<PFP::MAP> nb(::myMap);
	m_topo_render->updateData<PFP>(::myMap, ::position,  0.8f, 0.8f, 0.8f, nb);
	updateDepths();
	computeColorsUsingDepths<PFP>(::myMap, ::color, m_opacity, m_opacity_gradient, allDarts, m_depths, m_lastDepth);
	m_explode_render->updateData<PFP>(::myMap, ::position, ::color);

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
	m_topo_render->updateData<PFP>(::myMap, ::position,  0.8f, 0.8f, 0.8f, nb);
	updateDepths();
	computeColorsUsingDepths<PFP>(::myMap, ::color, m_opacity, m_opacity_gradient, allDarts, m_depths, m_lastDepth);
	m_explode_render->updateData<PFP>(::myMap, ::position, color);
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
			unsigned int const n = m_explode_render->nbTris();
			static std::vector<unsigned int> indices(n);
			static std::vector<GLuint> triangles(n * 4);

			sortData(m_explode_render, indices, triangles, viewpoint());
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
	DEBUG_OUT << "Updating depths..." << std::endl;
	m_depths.clear();
	m_depths.resize(::myMap.getNbDarts(), -1);
	m_lastDepth = computeDepths<PFP>(::myMap, allDarts, m_depths);
	DEBUG_OUT << "Depths are up-to-date" << std::endl;
}

glm::vec4 MyQT::viewpoint() const
{
	return glm::inverse(modelViewMatrix()) * glm::vec4(0.0, 0.0, 0.0, 1.0);
}

namespace Debug
{

std::ostream & operator<<(std::ostream & out, glm::vec4 const & v)
{
	out << '[' << v[0] << ' ' << v[1] << ' ' << v[2] << ' ' << v[3] << ']';

	return out;
}

std::ostream & operator<<(std::ostream & out, QPointF const & p)
{
	out << '[' << p.x() << ' ' << p.y() << ']';

	return out;
}

} // Debug

static glm::vec4 explode(glm::vec4 const & v, glm::vec4 const & faceCenter, float const faceScale, glm::vec4 const & volumeCenter, float const volumeScale)
{
	glm::vec4 const tmp = faceScale * v + (1.0 - faceScale) * faceCenter;

	return volumeScale * tmp + (1.0 - volumeScale) * volumeCenter;
}

static glm::vec4 project(glm::vec4 const & v, glm::mat4 const & mvp, glm::vec4 const & viewportCenter, glm::vec4 const & viewportScale)
{
	glm::vec4 const tmp(mvp * v);
	return viewportScale * tmp / tmp[3]  + viewportCenter;
}

// Scans a side of a triangle setting min X and max X in ContourX
// using the Bresenham's line drawing algorithm.
void updateCountourX(int * const contourX, int const screenHeight, int x1, int y1, int x2, int y2)
{
	int const sx = x2 - x1;
	int const sy = y2 - y1;
	int const dx1 = (0 < sx) - (sx < 0);
	int const dy1 = (0 < sy) - (sy < 0);
	int m = std::abs(sx);
	int n = std::abs(sy);
	int dx2 = dx1;
	int dy2 = 0;

	if (m < n)
	{
		std::swap(m, n);
		dx2 = 0;
		dy2 = dy1;
	}

	int x = x1;
	int y = y1;
	int pixelCount = m + 2;
	int k = n / 2;

	while (--pixelCount)
	{
		if (0 <= y && y < screenHeight)
		{
			if (x < contourX[y * 2 + 0])
			{
				contourX[y * 2 + 0] = x;
			}

			if (contourX[y * 2 + 1] < x)
			{
				contourX[y * 2 + 1] = x;
			}
		}

		k += n;

		if (k < m)
		{
			x += dx2;
			y += dy2;
		}
		else
		{
			k -= m;
			x += dx1;
			y += dy1;
		}
	}
}

void rasterizeTriangle(int * const contourX, int const screenHeight, glm::vec4 const & p0, glm::vec4 const & p1, glm::vec4 const & p2, QImage & image, unsigned int color)
{
	int const firstY = std::max(0.0f, std::min(std::min(p0.y, p1.y), p2.y));
	int const lastY = std::min(screenHeight - 1.0f, std::max(std::max(p0.y, p1.y), p2.y));

	for (int y = firstY; y <= lastY; ++y)
	{
		contourX[y * 2 + 0] = INT_MAX; // min X
		contourX[y * 2 + 1] = INT_MIN; // max X
	}

	updateCountourX(contourX, screenHeight, p0.x, p0.y, p1.x, p1.y);
	updateCountourX(contourX, screenHeight, p1.x, p1.y, p2.x, p2.y);
	updateCountourX(contourX, screenHeight, p2.x, p2.y, p0.x, p0.y);

	for (int y = firstY; y <= lastY; ++y)
	{
		int const lastX = contourX[y * 2 + 1];

		for (int x = contourX[y * 2 + 0]; x <= lastX; ++x)
		{
			image.setPixel(x, y, color);
		}
	}
}

void MyQT::button_render_software()
{
	DEBUG_OUT << "Software rendering..." << std::endl;
	static bool const debugRasterization = false;
	static bool const debugUseQtRasterization = false;
	static bool const debugDrawQtWireframe = true;

	Algo::Render::GL2::ExplodeVolumeAlphaRender const * const evr = m_explode_render;

	VBODataPointer const colors(evr->colors());
	VBODataPointer const vertices(evr->vertices());

	if (colors && vertices)
	{
		Utils::Chrono timer;

		timer.start();

		assert(colors.elementCount() == vertices.elementCount());
		GLint viewport[4];

		glGetIntegerv(GL_VIEWPORT, viewport);
		GLint const viewportX = viewport[0];
		GLint const viewportY = viewport[1];
		GLint const viewportWidth = viewport[2];
		GLint const viewportHeight = viewport[3];

		DEBUG_OUT << viewportX << ' ' << viewportY << ' ' << viewportWidth << ' ' << viewportHeight << std::endl;

		glm::vec4 const viewportCenter(viewportX + viewportWidth / 2.0, viewportY + viewportHeight / 2.0, 0.0, 0.0);
		glm::vec4 const viewportScale(viewportWidth / 2.0, viewportHeight / 2.0, 1.0, 1.0);
		QImage image(viewportWidth, viewportHeight, QImage::Format_ARGB32);
		image.fill(QColor(0, 0, 0));
		QPainter painter(&image);
		glm::mat4 const mvp = projectionMatrix() * modelViewMatrix() * transfoMatrix();

		painter.setPen(Qt::NoPen);

		int * ContourX = new int[viewportHeight * 2];

		for (unsigned int i = 0; i < vertices.elementCount(); i += 4)
		{
//			using namespace Debug;
			glm::vec4 const faceCenter(colors[i * 4 + 0 + 0], colors[i * 4 + 0 + 1], colors[i * 4 + 0 + 2], 1.0);
			glm::vec4 const volumeCenter(vertices[i * 3 + 0 + 0], vertices[i * 3 + 0 + 1], vertices[i * 3 + 0 + 2], 1.0);
			glm::vec4 v1(vertices[i * 3 + 3 + 0], vertices[i * 3 + 3 + 1], vertices[i * 3 + 3 + 2], 1.0);
			glm::vec4 v2(vertices[i * 3 + 6 + 0], vertices[i * 3 + 6 + 1], vertices[i * 3 + 6 + 2], 1.0);
			glm::vec4 v3(vertices[i * 3 + 9 + 0], vertices[i * 3 + 9 + 1], vertices[i * 3 + 9 + 2], 1.0);
//			DEBUG_OUT << v1 << ' ' << v2 << ' ' << v3 << std::endl;
			v1 = project(explode(v1, faceCenter, m_explode_factorf, volumeCenter, m_explode_factor),
					mvp, viewportCenter, viewportScale);
			v2 = project(explode(v2, faceCenter, m_explode_factorf, volumeCenter, m_explode_factor),
					mvp, viewportCenter, viewportScale);
			v3 = project(explode(v3, faceCenter, m_explode_factorf, volumeCenter, m_explode_factor),
					mvp, viewportCenter, viewportScale);
//			DEBUG_OUT << v1 << ' ' << v2 << ' ' << v3 << std::endl;
			v1.y = viewportHeight - 1 - v1.y;
			v2.y = viewportHeight - 1 - v2.y;
			v3.y = viewportHeight - 1 - v3.y;

			QColor const color(colors[i * 4 + 4 + 0] * 255.0, colors[i * 4 + 4 + 1] * 255.0, colors[i * 4 + 4 + 2] * 255.0);

			if (!debugUseQtRasterization)
			{
				rasterizeTriangle(ContourX, viewportHeight, v1, v2, v3, image, color.rgba());
			}
			else
			{
				painter.setBrush(color);
			}

			if (debugDrawQtWireframe)
			{
				painter.setPen(QColor(255, 0, 255));
			}

			if (debugUseQtRasterization || debugDrawQtWireframe)
			{
				QPointF const triangle[3] = { QPointF(v1.x, v1.y), QPointF(v2.x, v2.y), QPointF(v3.x, v3.y) };

				painter.drawConvexPolygon(triangle, 3);
			}

			if (debugRasterization)
			{
				DEBUG_OUT << i << " / " << vertices.elementCount() << '\r';
			}
		}

		delete[] ContourX;

		DEBUG_OUT << "Software rendering done in " << timer.elapsed() << " ms" << std::endl;

		m_imageComponent.setPixmap(QPixmap::fromImage(image));
		m_imageViewer.setWidget(&m_imageComponent);
		m_imageViewer.show();
	}
	else
	{
		DEBUG_OUT << "Software rendering aborted" << std::endl;
	}
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
				::position = ::myMap.getAttribute<PFP::VEC3, VERTEX>(attrNames[0]) ;
		}

		if(extension == std::string(".node"))
		{
			if(!Algo::Import::importMeshV<PFP>(::myMap, argv[1], attrNames, Algo::Import::ImportVolumique::NODE))
			{
				std::cerr << "could not import " << argv[1] << std::endl ;
				return 1;
			}
			else
				::position = ::myMap.getAttribute<PFP::VEC3,VERTEX>(attrNames[0]) ;
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
				::position = ::myMap.getAttribute<PFP::VEC3, VERTEX>(attrNames[0]) ;
				::myMap.closeMap();
			}
		}

		::color = ::myMap.addAttribute<PFP::VEC4, VOLUME>("color");

		TraversorCell<PFP::MAP, VOLUME> tra(::myMap);
		float maxV = 0.0f;
		for (Dart d = tra.begin(); d != tra.end(); d = tra.next())
		{
			float v = Algo::Geometry::tetrahedronVolume<PFP>(::myMap, d, ::position);
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
		::position = ::myMap.addAttribute<PFP::VEC3, VERTEX>("position");
		Algo::Modelisation::Primitive3D<PFP> prim(::myMap, ::position);
		int nb = 8;
		prim.hexaGrid_topo(nb,nb,nb);
		prim.embedHexaGrid(1.0f,1.0f,1.0f);

		::color = ::myMap.addAttribute<PFP::VEC4, VOLUME>("color");
		TraversorW<PFP::MAP> tra(::myMap);
		for (Dart d = tra.begin(); d != tra.end(); d = tra.next())
		{
			PFP::VEC3 tmp = ::position[d] + PFP::VEC3(0.5,0.5,0.5);
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
    Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(::myMap, ::position);
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

	sqt.setCallBack( dock.button_render_software, SIGNAL(released()), SLOT(button_render_software()) );


	sqt.show();
	dock.slider_explode->setValue(80);
	dock.slider_explode_face->setValue(80);
	sqt.clipping_onoff(true);

	Utils::Chrono ch;

	std::cout << "Compute Volume ->"<< std::endl;
	ch.start();
	float vol = Algo::Geometry::totalVolume<PFP>(::myMap, ::position);
	vol += Algo::Geometry::totalVolume<PFP>(::myMap, ::position);
	vol += Algo::Geometry::totalVolume<PFP>(::myMap, ::position);
	vol += Algo::Geometry::totalVolume<PFP>(::myMap, ::position);
	std::cout << ch.elapsed()<< " ms  val="<<vol<< std::endl;

	ch.start();
	vol = Algo::Geometry::Parallel::totalVolume<PFP>(::myMap, ::position);
	vol += Algo::Geometry::Parallel::totalVolume<PFP>(::myMap, ::position);
	vol += Algo::Geometry::Parallel::totalVolume<PFP>(::myMap, ::position);
	vol += Algo::Geometry::Parallel::totalVolume<PFP>(::myMap, ::position);
	std::cout << ch.elapsed()<< " ms //  val="<<vol<< std::endl;


	// et on attend la fin.
	return app.exec();
}
