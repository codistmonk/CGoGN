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
#include "Utils/svg.h"
#include "Utils/cgognStream.h"
#include <algorithm>
#include <typeinfo>
#include <GL/glew.h>

namespace CGoGN
{

namespace Utils
{

namespace SVG
{


const std::vector<Geom::Vec3f>& SvgObj::vertices() const
{
	return  m_vertices;
}

void SvgObj::addVertex(const Geom::Vec3f& v)
{
	m_vertices.push_back(v);
	m_colors.push_back(m_color);
}

void SvgObj::addVertex3D(const Geom::Vec3f& v)
{
	m_vertices3D.push_back(v);
	m_colors.push_back(m_color);
}

void SvgObj::addVertex(const Geom::Vec3f& v, const Geom::Vec3f& c)
{
	m_vertices.push_back(v);
	if (m_colors.size() < m_vertices.size())
		m_colors.push_back(c);
}

void SvgObj::addVertex3D(const Geom::Vec3f& v, const Geom::Vec3f& c)
{
	m_vertices3D.push_back(v);
	if (m_colors.size() < m_vertices.size())
		m_colors.push_back(c);
}


void SvgObj::setWidth(float w)
{
	m_width=w;
}


void SvgObj::setColor(const Geom::Vec3f& c)
{
	m_color=c;
}


void SvgObj::close()
{
	m_vertices.push_back(m_vertices.front());
}

unsigned int SvgObj::nbv() const
{
	return m_vertices3D.size();
}

const Geom::Vec3f& SvgObj::P(unsigned int i) const
{
	return m_vertices3D[i];
}


Geom::Vec3f SvgObj::normal()
{
	if (m_vertices.size()<3)
	{
		CGoGNerr << "Error SVG normal computing (not enough points)"<<CGoGNendl;
		return Geom::Vec3f(0.0f,0.0f,0.0f);
	}

	Geom::Vec3f U = m_vertices3D[2] - m_vertices3D[1];
	Geom::Vec3f V = m_vertices3D[0] - m_vertices3D[1];

	Geom::Vec3f N = U^V;

	N.normalize(); // TO DO verify that is necessary

	return N;
}


void SvgPoints::save(std::ofstream& out)
{
	std::stringstream ss;

	unsigned int nb = m_vertices.size();
	for (unsigned int i=0; i<nb; ++i)
	{
		out << "<circle cx=\""<< m_vertices[i][0];
		out << "\" cy=\""<< m_vertices[i][1];
		out << "\" r=\""<< m_width;
		out << "\" style=\"stroke: none; fill: #";

		out << std::hex;
		unsigned int wp = out.width(2);
		char prev = out.fill('0');
		out << int(m_colors[i][0]*255);
		out.width(2); out.fill('0');
		out<< int(m_colors[i][1]*255);
		out.width(2); out.fill('0');
		out << int(m_colors[i][2]*255)<<std::dec;
		out.fill(prev);
		out.width(wp);

		out <<"\"/>"<< std::endl;
	}

}


void SvgPolyline::save(std::ofstream& out)
{
	std::stringstream ss;

	out << "<polyline fill=\"none\" stroke=\"#";
	out << std::hex;
	unsigned int wp = out.width(2);
	char prev = out.fill('0');
	out << int(m_color[0]*255);
	out.width(2); out.fill('0');
	out<< int(m_color[1]*255);
	out.width(2); out.fill('0');
	out << int(m_color[2]*255)<<std::dec;
	out <<"\" stroke-width=\""<<m_width<<"\" points=\"";
	out.fill(prev);
	out.width(wp);
	for (std::vector<Geom::Vec3f>::iterator it =m_vertices.begin(); it != m_vertices.end(); ++it)
	{
		out << (*it)[0] << ","<< (*it)[1]<< " ";
	}
	out <<"\"/>"<< std::endl;
}

void SvgLines::save(std::ofstream& out)
{
	std::stringstream ss;

	unsigned int nb = m_vertices.size();
	for (unsigned int i=0; i<nb; ++i)
	{

		out << "<polyline fill=\"none\" stroke=\"#";
		out << std::hex;
		unsigned int wp = out.width(2);
		char prev = out.fill('0');
		out << int(m_colors[i][0]*255);
		out.width(2); out.fill('0');
		out<< int(m_colors[i][1]*255);
		out.width(2); out.fill('0');
		out << int(m_colors[i][2]*255)<<std::dec;
		out <<"\" stroke-width=\""<<m_width<<"\" points=\"";
		out.fill(prev);
		out.width(wp);
		out << m_vertices[i][0] << ","<< m_vertices[i][1]<< " ";
		i++;
		out << m_vertices[i][0] << ","<< m_vertices[i][1];
		out <<"\"/>"<< std::endl;
	}
}


void SvgPolygon::setColorFill(const Geom::Vec3f& c)
{
	m_colorFill=c;
}


void SvgPolygon::save(std::ofstream& out)
{
	std::stringstream ss;

	out << "<polyline fill=\"";
	out << std::hex;
	unsigned int wp = out.width(2);
	char prev = out.fill('0');
	out << int(m_colorFill[0]*255);
	out.width(2); out.fill('0');
	out<< int(m_colorFill[1]*255);
	out.width(2); out.fill('0');
	out << int(m_colorFill[2]*255);

	out << "none\" stroke=\"#";
	wp = out.width(2);
	prev = out.fill('0');
	out << int(m_color[0]*255);
	out.width(2); out.fill('0');
	out<< int(m_color[1]*255);
	out.width(2); out.fill('0');
	out << int(m_color[2]*255)<<std::dec;

	out <<"\" stroke-width=\""<<m_width<<"\" points=\"";
	out.fill(prev);
	out.width(wp);
	for (std::vector<Geom::Vec3f>::iterator it =m_vertices.begin(); it != m_vertices.end(); ++it)
	{
		out << (*it)[0] << ","<< (*it)[1]<< " ";
	}
	out <<"\"/>"<< std::endl;

}





SVGOut::SVGOut(const std::string& filename, const glm::mat4& model, const glm::mat4& proj):
		m_model(model),m_proj(proj),global_color(Geom::Vec3f(0.0f,0.0f,0.0f)), global_width(2.0f)
{
	m_objs.reserve(1000);

	m_out = new std::ofstream(filename.c_str()) ;
	if (!m_out->good())
	{
		CGoGNerr << "Unable to open file " << CGoGNendl ;
		// ????
	}

	glGetIntegerv(GL_VIEWPORT, &(m_viewport[0]));
//
//	*m_out << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"<< std::endl;
//	*m_out << "<svg xmlns=\"http://www.w3.org/2000/svg\""<< std::endl;
//	*m_out << " xmlns:xlink=\"http://www.w3.org/1999/xlink\""<< std::endl;
//	*m_out << " width=\""<<m_viewport[2]<<"px\" height=\""<<m_viewport[3]<<"px\" viewBox=\"0 0 "<<m_viewport[2]<<" "<<m_viewport[3]<<"\">"<< std::endl;
//	*m_out << "<title>test</title>"<< std::endl;
//	*m_out << "<desc>"<< std::endl;
//	*m_out << "Rendered from CGoGN"<< std::endl;
//
//	*m_out << "</desc>"<< std::endl;
//	*m_out << "<defs>"<< std::endl;
//	*m_out << "</defs>"<< std::endl;
//	*m_out << "<g shape-rendering=\"crispEdges\">" << std::endl;
}

SVGOut::~SVGOut()
{
	if (m_out->good())
	{
		closeFile();
	}
	delete m_out;

	for (std::vector<SvgObj*>::iterator it = m_objs.begin(); it != m_objs.end(); ++it)
		delete (*it);
}

void SVGOut::setColor(const Geom::Vec3f& col)
{
	global_color = col;
}

void SVGOut::setWidth(float w)
{
	global_width = w;
}

void SVGOut::closeFile()
{
	unsigned int a = 0;
	unsigned int b = 0;
	unsigned int c = 1024;
	unsigned int d = 1024;
	computeBB(a,b,c,d);

	*m_out << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"<< std::endl;
	*m_out << "<svg xmlns=\"http://www.w3.org/2000/svg\""<< std::endl;
	*m_out << " xmlns:xlink=\"http://www.w3.org/1999/xlink\""<< std::endl;
//	*m_out << " width=\""<<m_viewport[2]<<"px\" height=\""<<m_viewport[3]<<"px\" viewBox=\"0 0 "<<m_viewport[2]<<" "<<m_viewport[3]<<"\">"<< std::endl;
	*m_out << "viewBox=\""<< a <<" "<< b <<" "<< c-a << " " << d-b <<"\">"<< std::endl;
	*m_out << "<title>test</title>"<< std::endl;
	*m_out << "<desc>"<< std::endl;
	*m_out << "Rendered from CGoGN"<< std::endl;

	*m_out << "</desc>"<< std::endl;
	*m_out << "<defs>"<< std::endl;
	*m_out << "</defs>"<< std::endl;
	*m_out << "<g shape-rendering=\"crispEdges\">" << std::endl;

	// here do the sort in necessary

	for (std::vector<SvgObj*>::iterator it = m_objs.begin(); it != m_objs.end(); ++it)
	{
		(*it)->save(*m_out);
	}

	*m_out << "</g>" << std::endl;
	*m_out << "</svg>" << std::endl;
	m_out->close();
}


void SVGOut::computeBB(unsigned int& a, unsigned int& b, unsigned int& c, unsigned& d)
{
	// here do the sort in necessary
	a = 100000;
	b = 100000;
	c = 0;
	d = 0;

	for (std::vector<SvgObj*>::iterator it = m_objs.begin(); it != m_objs.end(); ++it)
	{
		const std::vector<Geom::Vec3f>& vert = (*it)->vertices();
		for (std::vector<Geom::Vec3f>::const_iterator j = vert.begin(); j != vert.end(); ++j)
		{
			const Geom::Vec3f& P = *j;
			if (P[0]<a)
				a = (unsigned int)(P[0]);
			if (P[1]<b)
				b = (unsigned int)(P[1]);
			if (P[0]>c)
				c = (unsigned int)(P[0]);
			if (P[1]>d)
				d = (unsigned int)(P[1]);
		}

		if (a>10)
			a-=10;
		if (b>10)
			b-=10;
		c+=10;
		d+=10;
	}
}



void SVGOut::beginPoints()
{
	glm::i32vec4 viewport;
	glGetIntegerv(GL_VIEWPORT, &(viewport[0]));

	m_current = new SvgPoints();
	m_current->setColor(global_color);
	m_current->setWidth(global_width);
}

void SVGOut::endPoints()
{
	m_objs.push_back(m_current);
	m_current = NULL;
}

void SVGOut::addPoint(const Geom::Vec3f& P)
{
	glm::vec3 Q = glm::project(glm::vec3(P[0],P[1],P[2]),m_model,m_proj,m_viewport);
	glm::vec3 R = glm::project(glm::vec3(P[0],P[1],P[2]),m_model,glm::mat4(1.0),m_viewport);
	m_current->addVertex(Geom::Vec3f(Q[0],float(m_viewport[3])-Q[1],Q[2]));
	m_current->addVertex3D(Geom::Vec3f(Q[0],float(m_viewport[3])-Q[1],Q[2]));
}

void SVGOut::addPoint(const Geom::Vec3f& P, const Geom::Vec3f& C)
{
	glm::vec3 Q = glm::project(glm::vec3(P[0],P[1],P[2]),m_model,m_proj,m_viewport);
	glm::vec3 R = glm::project(glm::vec3(P[0],P[1],P[2]),m_model,glm::mat4(1.0),m_viewport);
	m_current->addVertex(Geom::Vec3f(Q[0],float(m_viewport[3])-Q[1],Q[2]),C);
	m_current->addVertex3D(Geom::Vec3f(Q[0],float(m_viewport[3])-Q[1],Q[2]),C);
}


void SVGOut::beginLines()
{
	glm::i32vec4 viewport;
	glGetIntegerv(GL_VIEWPORT, &(viewport[0]));

	m_current = new SvgLines();
	m_current->setColor(global_color);
	m_current->setWidth(global_width);
}


void SVGOut::endLines()
{
	m_objs.push_back(m_current);
	m_current = NULL;
}

void SVGOut::addLine(const Geom::Vec3f& P, const Geom::Vec3f& P2)
{
	glm::vec3 Q = glm::project(glm::vec3(P[0],P[1],P[2]),m_model,m_proj,m_viewport);
	glm::vec3 R = glm::project(glm::vec3(P[0],P[1],P[2]),m_model,glm::mat4(1.0),m_viewport);

	glm::vec3 Q2 = glm::project(glm::vec3(P2[0],P2[1],P2[2]),m_model,m_proj,m_viewport);
	glm::vec3 R2 = glm::project(glm::vec3(P2[0],P2[1],P2[2]),m_model,glm::mat4(1.0),m_viewport);

	m_current->addVertex(Geom::Vec3f(Q[0],float(m_viewport[3])-Q[1],Q[2]));
	m_current->addVertex(Geom::Vec3f(Q2[0],float(m_viewport[3])-Q2[1],Q2[2]));

	m_current->addVertex3D(Geom::Vec3f(R[0],float(m_viewport[3])-R[1],R[2]));
	m_current->addVertex3D(Geom::Vec3f(R2[0],float(m_viewport[3])-R2[1],R2[2]));
}



void SVGOut::addLine(const Geom::Vec3f& P, const Geom::Vec3f& P2, const Geom::Vec3f& C)
{
	glm::vec3 Q = glm::project(glm::vec3(P[0],P[1],P[2]),m_model,m_proj,m_viewport);
	glm::vec3 R = glm::project(glm::vec3(P[0],P[1],P[2]),m_model,glm::mat4(1.0),m_viewport);

	glm::vec3 Q2 = glm::project(glm::vec3(P2[0],P2[1],P2[2]),m_model,m_proj,m_viewport);
	glm::vec3 R2 = glm::project(glm::vec3(P2[0],P2[1],P2[2]),m_model,glm::mat4(1.0),m_viewport);

	m_current->addVertex(Geom::Vec3f(Q[0],float(m_viewport[3])-Q[1],Q[2]),C);
	m_current->addVertex(Geom::Vec3f(Q2[0],float(m_viewport[3])-Q2[1],Q2[2]),C);

	m_current->addVertex3D(Geom::Vec3f(R[0],float(m_viewport[3])-R[1],R[2]),C);
	m_current->addVertex3D(Geom::Vec3f(R2[0],float(m_viewport[3])-R2[1],R2[2]),C);
}




} // namespace SVG
} // namespace Utils
} // namespace CGoGN


