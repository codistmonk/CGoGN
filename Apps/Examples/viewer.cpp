/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009, IGG Team, LSIIT, University of Strasbourg                *
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

#include "viewer.h"

Viewer::Viewer() :
	m_renderStyle(FLAT),
	m_drawVertices(false),
	m_drawEdges(false),
	m_drawFaces(true),
	m_drawNormals(false),
	m_drawTopo(false),
	m_render(NULL),
	m_phongShader(NULL),
	m_flatShader(NULL),
	m_vectorShader(NULL),
	m_simpleColorShader(NULL),
	m_simpleNormalShader(NULL),
	m_pointSprite(NULL),
	m_fbo(NULL)
{
	normalScaleFactor = 1.0f ;
	vertexScaleFactor = 0.1f ;
	faceShrinkage = 1.0f ;

	colClear = Geom::Vec4f(0.2f, 0.2f, 0.2f, 0.1f) ;
	colDif = Geom::Vec4f(0.8f, 0.9f, 0.7f, 1.0f) ;
	colSpec = Geom::Vec4f(0.9f, 0.9f, 0.9f, 1.0f) ;
	colNormal = Geom::Vec4f(1.0f, 0.0f, 0.0f, 1.0f) ;
	shininess = 80.0f ;
}

void Viewer::initGUI()
{
    setDock(&dock) ;

    dock.check_drawVertices->setChecked(false) ;
    dock.check_drawEdges->setChecked(false) ;
    dock.check_drawFaces->setChecked(true) ;
    dock.check_drawNormals->setChecked(false) ;

    dock.slider_verticesSize->setVisible(false) ;
    dock.slider_normalsSize->setVisible(false) ;

    dock.slider_verticesSize->setSliderPosition(50) ;
    dock.slider_normalsSize->setSliderPosition(50) ;

	setCallBack( dock.check_drawVertices, SIGNAL(toggled(bool)), SLOT(slot_drawVertices(bool)) ) ;
	setCallBack( dock.slider_verticesSize, SIGNAL(valueChanged(int)), SLOT(slot_verticesSize(int)) ) ;
	setCallBack( dock.check_drawEdges, SIGNAL(toggled(bool)), SLOT(slot_drawEdges(bool)) ) ;
	setCallBack( dock.check_drawFaces, SIGNAL(toggled(bool)), SLOT(slot_drawFaces(bool)) ) ;
	setCallBack( dock.combo_faceLighting, SIGNAL(currentIndexChanged(int)), SLOT(slot_faceLighting(int)) ) ;
	setCallBack( dock.check_drawTopo, SIGNAL(toggled(bool)), SLOT(slot_drawTopo(bool)) ) ;
	setCallBack( dock.check_drawNormals, SIGNAL(toggled(bool)), SLOT(slot_drawNormals(bool)) ) ;
	setCallBack( dock.slider_normalsSize, SIGNAL(valueChanged(int)), SLOT(slot_normalsSize(int)) ) ;
}

void Viewer::cb_initGL()
{
	Utils::GLSLShader::setCurrentOGLVersion(2) ;

	setFocal(5.0f) ;

	m_render = new Algo::Render::GL2::MapRender() ;
	m_topoRender = new Algo::Render::GL2::TopoRender() ;

	m_topoRender->setInitialDartsColor(0.25f, 0.25f, 0.25f) ;

	m_positionVBO = new Utils::VBO() ;
	m_normalVBO = new Utils::VBO() ;

	m_phongShader = new Utils::ShaderPhong() ;
	m_phongShader->setAttributePosition(m_positionVBO) ;
	m_phongShader->setAttributeNormal(m_normalVBO) ;
	m_phongShader->setAmbiant(colClear) ;
	m_phongShader->setDiffuse(colDif) ;
	m_phongShader->setSpecular(colSpec) ;
	m_phongShader->setShininess(shininess) ;

	m_flatShader = new Utils::ShaderFlat() ;
	m_flatShader->setAttributePosition(m_positionVBO) ;
	m_flatShader->setAmbiant(colClear) ;
	m_flatShader->setDiffuse(colDif) ;
	m_flatShader->setExplode(faceShrinkage) ;

	m_vectorShader = new Utils::ShaderVectorPerVertex() ;
	m_vectorShader->setAttributePosition(m_positionVBO) ;
	m_vectorShader->setAttributeVector(m_normalVBO) ;
	m_vectorShader->setColor(colNormal) ;

	m_simpleColorShader = new Utils::ShaderSimpleColor() ;
	m_simpleColorShader->setAttributePosition(m_positionVBO) ;
	Geom::Vec4f c(0.1f, 0.1f, 0.1f, 1.0f) ;
	m_simpleColorShader->setColor(c) ;
	
	m_simpleNormalShader = new Utils::ShaderSimpleNormal() ;
	m_simpleNormalShader->setAttributePosition(m_positionVBO);
	m_simpleNormalShader->setAttributeNormal(m_normalVBO);

	m_pointSprite = new Utils::PointSprite() ;
	m_pointSprite->setAttributePosition(m_positionVBO) ;

	registerShader(m_phongShader) ;
	registerShader(m_flatShader) ;
	registerShader(m_vectorShader) ;
	registerShader(m_simpleColorShader) ;
	registerShader(m_simpleNormalShader) ;
	registerShader(m_pointSprite) ;
	
	m_fbo = new Utils::FBO(1024, 1024);
	m_fbo->AttachRenderbuffer(GL_DEPTH_COMPONENT);
	m_fbo->AttachColorTexture(GL_RGBA);
	m_fbo->AttachColorTexture(GL_RGBA);
	m_fbo->AttachDepthTexture();
}

void Viewer::cb_redraw()
{
	bool useFbo = true;

	if (useFbo)
	{
		// Enable Fbo before rendering
		m_fbo->Bind();
		m_fbo->EnableColorAttachments();
	
		// Clear old Fbo buffers content
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	if(m_drawVertices)
	{
		float size = vertexScaleFactor ;
		m_pointSprite->setSize(size) ;
		m_pointSprite->predraw(Geom::Vec3f(0.0f, 0.0f, 1.0f)) ;
		m_render->draw(m_pointSprite, Algo::Render::GL2::POINTS) ;
		m_pointSprite->postdraw() ;
	}

	if(m_drawEdges)
	{
		glLineWidth(1.0f) ;
		m_render->draw(m_simpleColorShader, Algo::Render::GL2::LINES) ;
	}

	if(m_drawFaces)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL) ;
		glEnable(GL_LIGHTING) ;
		glEnable(GL_POLYGON_OFFSET_FILL) ;
		glPolygonOffset(1.0f, 1.0f) ;
		switch(m_renderStyle)
		{
			case FLAT :
				m_flatShader->setExplode(faceShrinkage) ;
				m_render->draw(m_flatShader, Algo::Render::GL2::TRIANGLES) ;
				break;
				
			case PHONG :
				m_render->draw(m_phongShader, Algo::Render::GL2::TRIANGLES) ;
				break;
				
			case NORMALS :
				m_render->draw(m_simpleNormalShader, Algo::Render::GL2::TRIANGLES) ;
				break;
				
			default :
				break;
		}
		glDisable(GL_POLYGON_OFFSET_FILL) ;
	}

	if(m_drawTopo)
	{
		m_topoRender->drawTopo() ;
	}

	if(m_drawNormals)
	{
		float size = normalBaseSize * normalScaleFactor ;
		m_vectorShader->setScale(size) ;
		glLineWidth(1.0f) ;
		m_render->draw(m_vectorShader, Algo::Render::GL2::POINTS) ;
	}
	
	if (useFbo)
	{
		// Disable Fbo after rendering
		m_fbo->Unbind();
	
		// Get and draw color texture from Fbo
		Utils::TextureSticker::StickTextureOnWholeScreen(m_fbo->GetColorTexId(0));
	}
	
	GLenum glError = glGetError();
	if (glError != GL_NO_ERROR)
		std::cout << "GL error : " << gluErrorString(glError) << std::endl;
}

void Viewer::cb_Open()
{
	std::string filters("all (*.*);; trian (*.trian);; ctm (*.ctm);; off (*.off);; ply (*.ply)") ;
	std::string filename = selectFile("Open Mesh", "", filters) ;
	if (filename.empty())
		return ;

	importMesh(filename) ;
	updateGL() ;
}

void Viewer::cb_Save()
{
	std::string filters("all (*.*);; map (*.map);; off (*.off);; ply (*.ply)") ;
	std::string filename = selectFileSave("Save Mesh", "", filters) ;

	exportMesh(filename) ;
}

void Viewer::cb_keyPress(int keycode)
{
    switch(keycode)
    {
    	case 'c' :
    		myMap.check();
    		break;
    	default:
    		break;
    }
}

void Viewer::importMesh(std::string& filename)
{
	myMap.clear(true) ;

	size_t pos = filename.rfind(".");    // position of "." in filename
	std::string extension = filename.substr(pos);

	if (extension == std::string(".map"))
	{
		myMap.loadMapBin(filename);
		position = myMap.getAttribute<VEC3, VERTEX>("position") ;
	}
	else
	{
		std::vector<std::string> attrNames ;
		if(!Algo::Import::importMesh<PFP>(myMap, filename.c_str(), attrNames))
		{
			CGoGNerr << "could not import " << filename << CGoGNendl ;
			return;
		}
		position = myMap.getAttribute<PFP::VEC3, VERTEX>(attrNames[0]) ;
	}

	myMap.enableQuickTraversal<VERTEX>() ;

	m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::GL2::POINTS) ;
	m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::GL2::LINES) ;
	m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::GL2::TRIANGLES) ;

	m_topoRender->updateData<PFP>(myMap, position, 0.85f, 0.85f) ;

	bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position) ;
	normalBaseSize = bb.diagSize() / 100.0f ;
//	vertexBaseSize = normalBaseSize / 5.0f ;

	normal = myMap.getAttribute<VEC3, VERTEX>("normal") ;
	if(!normal.isValid())
		normal = myMap.addAttribute<VEC3, VERTEX>("normal") ;

	Algo::Geometry::computeNormalVertices<PFP>(myMap, position, normal) ;

	m_positionVBO->updateData(position) ;
	m_normalVBO->updateData(normal) ;

	setParamObject(bb.maxSize(), bb.center().data()) ;
	updateGLMatrices() ;
}

void Viewer::exportMesh(std::string& filename, bool askExportMode)
{
	size_t pos = filename.rfind(".") ;    // position of "." in filename
	std::string extension = filename.substr(pos) ;

	if (extension == std::string(".off"))
		Algo::Export::exportOFF<PFP>(myMap, position, filename.c_str(), allDarts) ;
	else if (extension.compare(0, 4, std::string(".ply")) == 0)
	{
		int ascii = 0 ;
		if (askExportMode)
			Utils::QT::inputValues(Utils::QT::VarCombo("binary mode;ascii mode",ascii,"Save in")) ;

		std::vector<VertexAttribute<VEC3>*> attributes ;
		attributes.push_back(&position) ;
		Algo::Export::exportPLYnew<PFP>(myMap, attributes, filename.c_str(), !ascii, allDarts) ;
	}
	else if (extension == std::string(".map"))
		myMap.saveMapBin(filename) ;
	else
		std::cerr << "Cannot save file " << filename << " : unknown or unhandled extension" << std::endl ;
}

void Viewer::slot_drawVertices(bool b)
{
	m_drawVertices = b ;
	updateGL() ;
}

void Viewer::slot_verticesSize(int i)
{
	vertexScaleFactor = i / 500.0f ;
	updateGL() ;
}

void Viewer::slot_drawEdges(bool b)
{
	m_drawEdges = b ;
	updateGL() ;
}

void Viewer::slot_drawFaces(bool b)
{
	m_drawFaces = b ;
	updateGL() ;
}

void Viewer::slot_faceLighting(int i)
{
	m_renderStyle = i ;
	updateGL() ;
}

void Viewer::slot_drawTopo(bool b)
{
	m_drawTopo = b ;
	updateGL() ;
}

void Viewer::slot_drawNormals(bool b)
{
	m_drawNormals = b ;
	updateGL() ;
}

void Viewer::slot_normalsSize(int i)
{
	normalScaleFactor = i / 50.0f ;
	m_topoRender->updateData<PFP>(myMap, position, i / 100.0f, i / 100.0f) ;
	updateGL() ;
}

/**********************************************************************************************
 *                                      MAIN FUNCTION                                         *
 **********************************************************************************************/

int main(int argc, char **argv)
{
	QApplication app(argc, argv) ;

	Viewer sqt ;
	sqt.setGeometry(0, 0, 1000, 800) ;
 	sqt.show() ;

	if(argc >= 2)
	{
		std::string filename(argv[1]) ;
		sqt.importMesh(filename) ;
		if(argc >= 3)
		{
			std::string filenameExp(argv[2]) ;
			std::cout << "Exporting " << filename << " as " << filenameExp << " ... "<< std::flush ;
			sqt.exportMesh(filenameExp, false) ;
			std::cout << "done!" << std::endl ;

			return (0) ;
		}
	}

	sqt.initGUI() ;

	return app.exec() ;
}
