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
* Web site: https://iggservis.u-strasbg.fr/CGoGN/                              *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#include "stage_shader_number_two.h"

Stage_shader_number_two::Stage_shader_number_two() :
	m_renderStyle(FLAT),
	m_drawVertices(false),
	m_drawEdges(false),
	m_drawFaces(true),
	m_drawNormals(false),
	m_render(NULL),
	m_phongShader(NULL),
	m_flatShader(NULL),
	m_vectorShader(NULL),
	m_simpleColorShader(NULL),
	m_pointSprite(NULL)
{
	normalScaleFactor = 1.0f ;
	vertexScaleFactor = 1.0f ;
	faceShrinkage = 1.0f ;

	colClear = Geom::Vec4f(0.2f, 0.2f, 0.2f, 0.1f) ;
	colDif = Geom::Vec4f(0.8f, 0.9f, 0.7f, 1.0f) ;
	colSpec = Geom::Vec4f(0.9f, 0.9f, 0.9f, 1.0f) ;
	colNormal = Geom::Vec4f(1.0f, 0.0f, 0.0f, 1.0f) ;
	shininess = 80.0f ;
}

void Stage_shader_number_two::initGUI()
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
	setCallBack( dock.check_drawNormals, SIGNAL(toggled(bool)), SLOT(slot_drawNormals(bool)) ) ;
	setCallBack( dock.slider_normalsSize, SIGNAL(valueChanged(int)), SLOT(slot_normalsSize(int)) ) ;

	setCallBack(dock.doubleSpinBox_plane_a, SIGNAL(valueChanged(double)), SLOT(slot_doubleSpinBox_Plane(double)));
	setCallBack(dock.doubleSpinBox_plane_b, SIGNAL(valueChanged(double)), SLOT(slot_doubleSpinBox_Plane(double)));
	setCallBack(dock.doubleSpinBox_plane_c, SIGNAL(valueChanged(double)), SLOT(slot_doubleSpinBox_Plane(double)));
	setCallBack(dock.doubleSpinBox_plane_d, SIGNAL(valueChanged(double)), SLOT(slot_doubleSpinBox_Plane(double)));

	setCallBack(dock.doubleSpinBox_color_attenuation, SIGNAL(valueChanged(double)), SLOT(slot_doubleSpinBox_ColorAttenuationFactor(double)));
}

void Stage_shader_number_two::cb_initGL()
{
	Utils::GLSLShader::setCurrentOGLVersion(2) ;

	setFocal(5.0f) ;

	m_render = new Algo::Render::GL2::MapRender() ;

	m_positionVBO = new Utils::VBO() ;
	m_normalVBO = new Utils::VBO() ;

	m_phongShader = new Utils::ShaderPhong(true) ; // true sets on double sided lighting
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

	m_pointSprite = new Utils::PointSprite() ;
	m_pointSprite->setAttributePosition(m_positionVBO) ;

	registerShader(m_phongShader) ;
	registerShader(m_flatShader) ;
	registerShader(m_vectorShader) ;
	registerShader(m_simpleColorShader) ;
	registerShader(m_pointSprite) ;

	m_phongShader->setPlaneClipping(1);
}

void Stage_shader_number_two::cb_redraw()
{
	if(m_drawVertices)
	{
		float size = vertexBaseSize * vertexScaleFactor ;
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

	if(m_drawNormals)
	{
		float size = normalBaseSize * normalScaleFactor ;
		m_vectorShader->setScale(size) ;
		glLineWidth(1.0f) ;
		m_render->draw(m_vectorShader, Algo::Render::GL2::POINTS) ;
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
				break ;
			case PHONG :
				m_render->draw(m_phongShader, Algo::Render::GL2::TRIANGLES) ;
				break ;
		}
		glDisable(GL_POLYGON_OFFSET_FILL) ;
	}

	m_phongShader->displayClippingPlane();
}

void Stage_shader_number_two::cb_Open()
{
	std::string filters("all (*.*);; trian (*.trian);; ctm (*.ctm);; off (*.off);; ply (*.ply)") ;
	std::string filename = selectFile("Open Mesh", "", filters) ;

	importMesh(filename) ;
	updateGL() ;
}

void Stage_shader_number_two::cb_keyPress(int code)
{

}

void Stage_shader_number_two::cb_mousePress(int button, int x, int y)
{
	m_mouseLastX = x;
	m_mouseLastY = y;
}

void Stage_shader_number_two::cb_mouseMove(int button, int x, int y)
{
	if (Shift())
	{
		if (button == Qt::LeftButton)
		{
			Geom::Vec4f clipPlane = m_phongShader->getClippingPlaneEquation();
			clipPlane[0] += (m_mouseLastX - x)/40.0;
			clipPlane[1] += (m_mouseLastY - y)/40.0;

			m_phongShader->setClippingPlaneEquation(clipPlane);

			updateGL();
		}
	}

	m_mouseLastX = x;
	m_mouseLastY = y;
}

void Stage_shader_number_two::importMesh(std::string& filename)
{
	std::vector<std::string> attrNames ;
	if(!Algo::Import::importMesh<PFP>(myMap, filename.c_str(), attrNames))
	{
		CGoGNerr << "could not import " << filename << CGoGNendl ;
		return;
	}
	position = myMap.getAttribute<PFP::VEC3>(VERTEX, attrNames[0]) ;

	m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::GL2::POINTS) ;
	m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::GL2::LINES) ;
	m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::GL2::TRIANGLES) ;

	bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position) ;
	normalBaseSize = bb.diagSize() / 100.0f ;
	vertexBaseSize = normalBaseSize * 2.0f ;

	if(!normal.isValid())
		normal = myMap.addAttribute<PFP::VEC3>(VERTEX, "normal") ;

	Algo::Geometry::computeNormalVertices<PFP>(myMap, position, normal) ;

	m_positionVBO->updateData(position) ;
	m_normalVBO->updateData(normal) ;

	setParamObject(bb.maxSize(), bb.center().data()) ;
	updateGLMatrices() ;
}

void Stage_shader_number_two::slot_drawVertices(bool b)
{
	m_drawVertices = b ;
	updateGL() ;
}

void Stage_shader_number_two::slot_verticesSize(int i)
{
	vertexScaleFactor = i / 50.0f ;
	updateGL() ;
}

void Stage_shader_number_two::slot_drawEdges(bool b)
{
	m_drawEdges = b ;
	updateGL() ;
}

void Stage_shader_number_two::slot_drawFaces(bool b)
{
	m_drawFaces = b ;
	updateGL() ;
}

void Stage_shader_number_two::slot_faceLighting(int i)
{
	m_renderStyle = i ;
	updateGL() ;
}

void Stage_shader_number_two::slot_drawNormals(bool b)
{
	m_drawNormals = b ;
	updateGL() ;
}

void Stage_shader_number_two::slot_normalsSize(int i)
{
	normalScaleFactor = i / 50.0f ;
	updateGL() ;
}

void Stage_shader_number_two::slot_doubleSpinBox_Plane(double c)
{
	float aPlane = dynamic_cast<Utils::QT::uiDockInterface*>(dockWidget())->doubleSpinBox_plane_a->value();
	float bPlane = dynamic_cast<Utils::QT::uiDockInterface*>(dockWidget())->doubleSpinBox_plane_b->value();
	float cPlane = dynamic_cast<Utils::QT::uiDockInterface*>(dockWidget())->doubleSpinBox_plane_c->value();
	float dPlane = dynamic_cast<Utils::QT::uiDockInterface*>(dockWidget())->doubleSpinBox_plane_d->value();
	m_phongShader->setClippingPlaneEquation(Geom::Vec4f(aPlane, bPlane, cPlane, dPlane));
	updateGL();
}

void Stage_shader_number_two::slot_doubleSpinBox_ColorAttenuationFactor(double c)
{
	m_phongShader->setClippingColorAttenuationFactor((float)c);
	updateGL();
}

/**********************************************************************************************
 *                                      MAIN FUNCTION                                         *
 **********************************************************************************************/

int main(int argc, char **argv)
{
	// comment
	QApplication app(argc, argv) ;

	Stage_shader_number_two sqt ;
	sqt.setGeometry(0, 0, 1000, 800) ;
 	sqt.show() ;

	if(argc == 2)
	{
		std::string filename(argv[1]) ;
		sqt.importMesh(filename) ;
	}

	sqt.initGUI() ;

	return app.exec() ;
}

