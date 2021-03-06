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

#include "Algo/Render/GL2/topoRender.h"
#include "Utils/Shaders/shaderSimpleColor.h"
#include "Utils/Shaders/shaderColorPerVertex.h"

namespace CGoGN
{

namespace Algo
{

namespace Render
{

namespace GL2
{

TopoRender::TopoRender():
	m_nbDarts(0),
	m_nbRel2(0),
	m_topo_dart_width(2.0f),
	m_topo_relation_width(3.0f),
	m_dartsColor(1.0f,1.0f,1.0f),
	m_bufferDartPosition(NULL)
{
	m_vbo0 = new Utils::VBO();
	m_vbo1 = new Utils::VBO();
	m_vbo2 = new Utils::VBO();
	m_vbo3 = new Utils::VBO();

	m_vbo0->setDataSize(3);
	m_vbo1->setDataSize(3);
	m_vbo2->setDataSize(3);
	m_vbo3->setDataSize(3);

	m_shader1 = new Utils::ShaderSimpleColor();
	m_shader2 = new Utils::ShaderColorPerVertex();

	// binding VBO - VA
	m_vaId = m_shader1->setAttributePosition(m_vbo1);

	m_shader2->setAttributePosition(m_vbo0);
	m_shader2->setAttributeColor(m_vbo3);

	// registering for auto matrices update
	Utils::GLSLShader::registerShader(NULL, m_shader1);
	Utils::GLSLShader::registerShader(NULL, m_shader2);
}

TopoRender::~TopoRender()
{
	Utils::GLSLShader::unregisterShader(NULL, m_shader2);
	Utils::GLSLShader::unregisterShader(NULL, m_shader1);

	delete m_shader2;
	delete m_shader1;
	delete m_vbo3;
	delete m_vbo2;
	delete m_vbo1;
	delete m_vbo0;

	if (m_attIndex.map() != NULL)
		static_cast<AttribMap*>(m_attIndex.map())->removeAttribute(m_attIndex);

	if (m_bufferDartPosition!=NULL)
		delete[] m_bufferDartPosition;
}

void TopoRender::setDartWidth(float dw)
{
	m_topo_dart_width = dw;
}

void TopoRender::setRelationWidth(float pw)
{
	m_topo_relation_width = pw;
}

void TopoRender::setDartColor(Dart d, float r, float g, float b)
{
	float RGB[6];
	RGB[0]=r; RGB[1]=g; RGB[2]=b;
	RGB[3]=r; RGB[4]=g; RGB[5]=b;
	m_vbo3->bind();
	glBufferSubData(GL_ARRAY_BUFFER, m_attIndex[d]*3*sizeof(float), 6*sizeof(float),RGB);
}

void TopoRender::setAllDartsColor(float r, float g, float b)
{
	m_vbo3->bind();
	GLvoid* ColorDartsBuffer = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
	float* colorDartBuf = reinterpret_cast<float*>(ColorDartsBuffer);
	for (unsigned int i=0; i < 2*m_nbDarts; ++i)
	{
		*colorDartBuf++ = r;
		*colorDartBuf++ = g;
		*colorDartBuf++ = b;
	}

	m_vbo3->bind();
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void TopoRender::setInitialDartsColor(float r, float g, float b)
{
	m_dartsColor = Geom::Vec3f(r,g,b);
}

void TopoRender::drawDarts()
{
	if (m_nbDarts==0)
		return;

	m_shader2->enableVertexAttribs();

	glLineWidth(m_topo_dart_width);
	glDrawArrays(GL_LINES, 0, m_nbDarts*2);

	// change the stride to take 1/2 vertices
	m_shader2->enableVertexAttribs(6*sizeof(GL_FLOAT));

	glPointSize(2.0f*m_topo_dart_width);
 	glDrawArrays(GL_POINTS, 0, m_nbDarts);

 	m_shader2->disableVertexAttribs();
}

void TopoRender::drawRelation1()
{
	if (m_nbDarts==0)
		return;

	glLineWidth(m_topo_relation_width);

	m_shader1->changeVA_VBO(m_vaId, m_vbo1);
	m_shader1->setColor(Geom::Vec4f(0.0f,1.0f,1.0f,0.0f));
	m_shader1->enableVertexAttribs();

	glDrawArrays(GL_LINES, 0, m_nbDarts*2);

	m_shader1->disableVertexAttribs();
}

void TopoRender::drawRelation2()
{
	if (m_nbRel2==0)
		return;

	glLineWidth(m_topo_relation_width);

	m_shader1->changeVA_VBO(m_vaId, m_vbo2);
	m_shader1->setColor(Geom::Vec4f(1.0f,0.0f,0.0f,0.0f));
	m_shader1->enableVertexAttribs();

	glDrawArrays(GL_LINES, 0, m_nbRel2*2);

	m_shader1->disableVertexAttribs();
}

void TopoRender::drawTopo()
{
	drawDarts();
	drawRelation1();
	drawRelation2();
}

void TopoRender::overdrawDart(Dart d, float width, float r, float g, float b)
{
	unsigned int indexDart = m_attIndex[d];

	m_shader1->changeVA_VBO(m_vaId, m_vbo0);
	m_shader1->setColor(Geom::Vec4f(r,g,b,0.0f));
	m_shader1->enableVertexAttribs();

	glLineWidth(width);
	glDrawArrays(GL_LINES, indexDart, 2);

	glPointSize(2.0f*width);
 	glDrawArrays(GL_POINTS, indexDart, 1);

 	m_shader2->disableVertexAttribs();
}

Dart TopoRender::colToDart(float* color)
{
	unsigned int r = (unsigned int)(color[0]*255.0f);
	unsigned int g = (unsigned int)(color[1]*255.0f);
	unsigned int b = (unsigned int)(color[2]*255.0f);

	unsigned int id = r + 255*g +255*255*b;

	if (id == 0)
		return NIL;
	return Dart(id-1);

}

void TopoRender::dartToCol(Dart d, float& r, float& g, float& b)
{
	// here use d.index beacause it is what we want (and not map.dartIndex(d) !!)
	unsigned int lab = d.index + 1; // add one to avoid picking the black of screen

	r = float(lab%255) / 255.0f; lab = lab/255;
	g = float(lab%255) / 255.0f; lab = lab/255;
	b = float(lab%255) / 255.0f; lab = lab/255;
	if (lab!=0)
		CGoGNerr << "Error picking color, too many darts"<< CGoGNendl;
}



Dart TopoRender::pickColor(unsigned int x, unsigned int y)
{
	//more easy picking for
	unsigned int dw = m_topo_dart_width;
	m_topo_dart_width+=2;

	// save clear color and set to zero
	float cc[4];
	glGetFloatv(GL_COLOR_CLEAR_VALUE,cc);

	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_LIGHTING);
	// draw in back buffer (not shown on screen)
	drawDarts();

	// restore dart width
	m_topo_dart_width = dw;

	// read the pixel under the mouse in back buffer
	glReadBuffer(GL_BACK);
	float color[3];
	glReadPixels(x,y,1,1,GL_RGB,GL_FLOAT,color);

	glClearColor(cc[0], cc[1], cc[2], cc[3]);

	return colToDart(color);
}

void TopoRender::pushColors()
{
	m_color_save = new float[6*m_nbDarts];
	m_vbo3->bind();
	void* colorBuffer = glMapBufferARB(GL_ARRAY_BUFFER, GL_READ_WRITE);

	memcpy(m_color_save, colorBuffer, 6*m_nbDarts*sizeof(float));
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void TopoRender::popColors()
{
	m_vbo3->bind();
	void* colorBuffer = glMapBufferARB(GL_ARRAY_BUFFER, GL_READ_WRITE);

	memcpy(colorBuffer, m_color_save, 6*m_nbDarts*sizeof(float));
	glUnmapBuffer(GL_ARRAY_BUFFER);

	delete[] m_color_save;
	m_color_save=NULL;
}

void TopoRender::svgout2D(const std::string& filename, const glm::mat4& model, const glm::mat4& proj)
{
	Utils::SVG::SVGOut svg(filename,model,proj);
	toSVG(svg);
}

void TopoRender::toSVG(Utils::SVG::SVGOut& svg)
{
	svg.setWidth(m_topo_relation_width);

	// PHI2 / beta2

	const Geom::Vec3f* ptr = reinterpret_cast<Geom::Vec3f*>(m_vbo2->lockPtr());

	svg.beginLines();
	for (unsigned int i=0; i<m_nbRel2; ++i)
		svg.addLine(ptr[2*i], ptr[2*i+1],Geom::Vec3f(0.8f,0.0f,0.0f));
	svg.endLines();

	m_vbo2->releasePtr();

	//PHI1 /beta1
	ptr = reinterpret_cast<Geom::Vec3f*>(m_vbo1->lockPtr());

	svg.beginLines();
	for (unsigned int i=0; i<m_nbRel1; ++i)
		svg.addLine(ptr[2*i], ptr[2*i+1],Geom::Vec3f(0.0f,0.7f,0.7f));
	svg.endLines();

	m_vbo1->releasePtr();


	const Geom::Vec3f* colorsPtr = reinterpret_cast<const Geom::Vec3f*>(m_vbo3->lockPtr());
	ptr= reinterpret_cast<Geom::Vec3f*>(m_vbo0->lockPtr());

	svg.setWidth(m_topo_dart_width);

	svg.beginLines();
	for (unsigned int i=0; i<m_nbDarts; ++i)
		svg.addLine(ptr[2*i], ptr[2*i+1], colorsPtr[2*i]);
	svg.endLines();

	svg.beginPoints();
	for (unsigned int i=0; i<m_nbDarts; ++i)
			svg.addPoint(ptr[2*i], colorsPtr[2*i]);
	svg.endPoints();

	m_vbo0->releasePtr();
	m_vbo3->releasePtr();
}



}//end namespace GL2

}//end namespace Render

}//end namespace Algo

}//end namespace CGoGN
