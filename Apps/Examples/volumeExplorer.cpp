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
#include "Utils/textureSticker.h"

using namespace CGoGN::VolumeExplorerTools;

PFP::MAP myMap;
VertexAttribute<PFP::VEC3> position ;
VolumeAttribute<PFP::VEC4> color ;

void MyQT::volumes_onoff(bool UNUSED(x))
{
	render_volumes = !render_volumes;
	updateGL();
}

void MyQT::edges_onoff(bool UNUSED(x))
{
	render_edges = !render_edges;
	updateGL();
}

void MyQT::topo_onoff(bool UNUSED(x))
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

void MyQT::hide_onoff(bool UNUSED(x))
{
	hide_clipping = !hide_clipping;
	updateGL();
}


void MyQT::slider_explode(int x)
{
	m_explode_factor = 0.01f*(x+1)-0.0001f;
	m_explode_render->setExplodeVolumes(m_explode_factor);
	explodeModel<PFP>(::myMap, ::position, allDarts, m_explode_factor, m_explode_factorf, m_explode_render);
	updateGL();
}

void MyQT::slider_explodeF(int x)
{
	m_explode_factorf = 0.01f*(x+1);
	m_explode_render->setExplodeFaces(m_explode_factorf);
	explodeModel<PFP>(::myMap, ::position, allDarts, m_explode_factor, m_explode_factorf, m_explode_render);
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
	computeColorsUsingDepths<PFP>(::myMap, ::color, m_opacity, m_opacity_gradient, allDarts, m_depths, m_lastDepth);
	updateColorVBO<PFP>(::myMap, ::color, allDarts, m_explode_render);
	updateGL(); DEBUG_GL;
}

void MyQT::slider_opacity_gradient(int const x)
{
	m_opacity_gradient = 0.01f * x;
	computeColorsUsingDepths<PFP>(::myMap, ::color, m_opacity, m_opacity_gradient, allDarts, m_depths, m_lastDepth);
	updateColorVBO<PFP>(::myMap, ::color, allDarts, m_explode_render);
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
	DEBUG_GL;
	 // choose to use GL version 2
	Utils::GLSLShader::setCurrentOGLVersion(2); DEBUG_GL;

	// create the renders
    m_topo_render = new Algo::Render::GL2::Topo3Render(); DEBUG_GL;
    m_explode_render = new Algo::Render::GL2::ExplodeVolumeAlphaRender(true); DEBUG_GL;

	SelectorDartNoBoundary<PFP::MAP> nb(::myMap);
	m_topo_render->updateData<PFP>(::myMap, ::position,  0.8f, 0.8f, 0.8f, nb);
	updateDepths();
	computeColorsUsingDepths<PFP>(::myMap, ::color, m_opacity, m_opacity_gradient, allDarts, m_depths, m_lastDepth);
	m_explode_render->updateData<PFP>(::myMap, ::position, color);
	m_explode_render->setExplodeVolumes(0.8f);
	m_explode_render->setExplodeFaces(0.9f);
	m_explode_render->setAmbient(Geom::Vec4f(0.2f,0.2f,0.2f,1.0f));
	m_explode_render->setBackColor(Geom::Vec4f(0.9f,0.9f,0.9f,1.0f));
	m_explode_render->setColorLine(Geom::Vec4f(0.8f,0.2f,0.2f,1.0f));
	DEBUG_GL;

	registerShader(m_explode_render->shaderFaces());
	registerShader(m_explode_render->shaderLines());
	DEBUG_GL;

    m_PlanePick = new Utils::Pickable(Utils::Pickable::GRID,1);
	m_frame = new Utils::FrameManipulator();
	m_frame->setSize(m_WidthObj/2.0f);
	DEBUG_GL;

	m_topo_render->shader1()->insertClippingCode();
	m_topo_render->shader2()->insertClippingCode();
	DEBUG_GL;

	clip_id1 = m_topo_render->shader1()->addClipPlane(); DEBUG_GL;
	clip_id2 = m_topo_render->shader2()->addClipPlane(); DEBUG_GL;

	m_topo_render->shader1()->setClipPlaneParamsAll(clip_id1, Geom::Vec3f(0,0,1), m_PosObj);
	m_topo_render->shader2()->setClipPlaneParamsAll(clip_id2, Geom::Vec3f(0,0,1), m_PosObj);
	m_explode_render->setClippingPlane(Geom::Vec4f(0,0,1,m_PosObj*Geom::Vec3f(0,0,-1)));
	DEBUG_GL;

	resetFbo();

	DEBUG_OUT << std::endl;
}

static CGoGN::Utils::FBO * newFrontDepthPeelingFBO(unsigned int const width, unsigned int const height)
{
	CGoGN::Utils::FBO * const result = new Utils::FBO(width, height); DEBUG_GL;

	result->AttachRenderbuffer(GL_DEPTH_COMPONENT); DEBUG_GL;
	result->AttachColorTexture(GL_RGBA); DEBUG_GL;
	result->AttachDepthTexture(); DEBUG_GL;

	return result;
}

void MyQT::resetFbo()
{
	Viewport const viewport;
	unsigned int const width = std::max(1U, static_cast<unsigned int>(viewport.width()));
	unsigned int const height = std::max(1U, static_cast<unsigned int>(viewport.height()));

	if (m_fbo1 && (m_fbo1->GetWidth() != width || m_fbo1->GetHeight() != height)) {
		delete m_fbo1;
		delete m_fbo2;
		m_fbo1 = NULL;
		m_fbo2 = NULL;
	}

	if (m_fbo1 == NULL) {
		using namespace CGoGN::VolumeExplorerTools::Debug;
		DEBUG_OUT << viewport << std::endl;
		m_fbo1 = newFrontDepthPeelingFBO(width, height);
		m_fbo2 = newFrontDepthPeelingFBO(width, height);
	}

	bindClearUnbind(m_fbo1);
	bindClearUnbind(m_fbo2);
}

void MyQT::cb_redraw()
{
	glDisable(GL_BLEND); DEBUG_GL;
	glEnable(GL_DEPTH_TEST); DEBUG_GL;
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
		resetFbo();
		glBindTexture(GL_TEXTURE_2D, *(m_fbo1->GetDepthTexId())); DEBUG_GL;

		if (m_opacity < 1)
		{
			unsigned int const n = m_explode_render->nbTris();
			static std::vector<GLuint> triangles(n * 4);

			sortData(m_explode_render, triangles, viewpoint());
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

		glBindTexture(GL_TEXTURE_2D, 0); DEBUG_GL;
	}

	if (clip_volume && !hide_clipping)
	{
		m_frame->draw(); DEBUG_GL;
		m_PlanePick->draw(); DEBUG_GL;
	}
}

void  MyQT::cb_mousePress(int UNUSED(button), int x, int y)
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

void  MyQT::cb_mouseRelease(int UNUSED(button), int UNUSED(x), int UNUSED(y))
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

void MyQT::button_render_software()
{
	DEBUG_OUT << "Software rendering..." << std::endl;

	Utils::Chrono timer;

	timer.start();

	Viewport const viewport;

	{
		using namespace CGoGN::VolumeExplorerTools::Debug;
		DEBUG_OUT << "viewport: " << viewport << std::endl;
	}

	FragmentBuffer fragmentBuffer;

	DEBUG_OUT << "Rasterizing triangles and accumulating fragments..." << std::endl;

	rasterizeTrianglesAndAccumulateFragments(m_explode_render, viewport,
			projectionMatrix() * modelViewMatrix() * transfoMatrix(), fragmentBuffer);

	if (!fragmentBuffer.empty()) {
		DEBUG_OUT << "Sorting and blending fragments..." << std::endl;

		QImage image(viewport.width(), viewport.height(), QImage::Format_ARGB32);
		image.fill(0);

		sortAndBlend(fragmentBuffer, image);

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

void MyQT::button_depth_peeling()
{
	DEBUG_OUT << "Depth peeling..." << std::endl;

	Utils::Chrono timer;

	timer.start();

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); DEBUG_GL;

	resetFbo();

	m_fbo2->Bind(); DEBUG_GL;
	m_fbo2->EnableColorAttachments(); DEBUG_GL;
	glClearColor(0, 0, 0, 0); DEBUG_GL;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); DEBUG_GL;
	glFlush(); DEBUG_GL;
	m_fbo2->Unbind(); DEBUG_GL;

	// Enable Fbo before rendering
	m_fbo1->Bind(); DEBUG_GL;
	m_fbo1->EnableColorAttachments(); DEBUG_GL;

	// Clear old Fbo buffers content
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); DEBUG_GL;

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
		m_explode_render->shaderFaces()->setDepthPeeling(true);
		glDisable(GL_BLEND); DEBUG_GL;
		glEnable(GL_DEPTH_TEST); DEBUG_GL;
		glActiveTexture(GL_TEXTURE0 + 0); DEBUG_GL;
		glBindTexture(GL_TEXTURE_2D, *(m_fbo2->GetDepthTexId())); DEBUG_GL;
		m_explode_render->drawFaces(); DEBUG_GL;
		glBindTexture(GL_TEXTURE_2D, 0); DEBUG_GL;
		glFlush(); DEBUG_GL;
		m_fbo1->Unbind(); DEBUG_GL;
		glDrawBuffer(GL_BACK); DEBUG_GL;

		Utils::TextureSticker::StickTextureOnWholeScreen(m_fbo1->GetColorTexId(0));

		CGoGN::Utils::FBO * previousFBO = m_fbo1;
		CGoGN::Utils::FBO * currentFBO = m_fbo2;

		// TODO count number of layers
		for (int i = 0; i < 10; ++i)
		{
			peelDepthLayerAndBlendToDefaultBuffer(previousFBO, currentFBO, m_explode_render);
			std::swap(previousFBO, currentFBO);
		}

		m_explode_render->shaderFaces()->setDepthPeeling(false);
	}
	else
	{
		glFlush(); DEBUG_GL;
		m_fbo1->Unbind(); DEBUG_GL;
		glDrawBuffer(GL_BACK); DEBUG_GL;
		Utils::TextureSticker::StickTextureOnWholeScreen(m_fbo1->GetColorTexId(0));
	}

	glReadBuffer(GL_BACK);

	Viewport const viewport;
	int const width = viewport.width();
	int const height = viewport.height();
	QImage image(width, height, QImage::Format_ARGB32);
	image.fill(0);

	std::vector<GLubyte> pixels(width * height * 4);
	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			QColor const color(
					pixels[(y * width + x) * 4 + 0],
					pixels[(y * width + x) * 4 + 1],
					pixels[(y * width + x) * 4 + 2]);
			image.setPixel(x, height - 1 - y, color.rgba());
		}
	}

	m_imageComponent.setPixmap(QPixmap::fromImage(image));
	m_imageViewer.setWidget(&m_imageComponent);
	m_imageViewer.show();

	DEBUG_OUT << "Depth peeling done in " << timer.elapsed() << " ms" << std::endl;
}

int main(int argc, char **argv)
{
	testRasterizeTriangle();

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
			{
				::position = ::myMap.getAttribute<PFP::VEC3,VERTEX>(attrNames[0]) ;
			}
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
			{
				maxV=v;
			}
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
		int nb = 3;
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

	sqt.setCallBack( dock.button_depth_peeling, SIGNAL(released()), SLOT(button_depth_peeling()) );


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
