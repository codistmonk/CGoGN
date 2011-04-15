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

#include <iostream>

#include "extrusionView.h"

#include "Topology/generic/parameters.h"
#include "Topology/map/map2.h"
#include "Topology/generic/embeddedMap2.h"

#include "Geometry/vector_gen.h"
#include "Geometry/matrix.h"
#include "Geometry/transfo.h"

#include "Algo/Geometry/boundingbox.h"
#include "Algo/Modelisation/extrusion.h"
#include "Topology/generic/parameters.h"

#include "Algo/Render/GL2/mapRender.h"
#include "Utils/shaderFlat.h"
#include "Utils/shaderSimpleColor.h"

#include <vector>

using namespace CGoGN;



struct PFP: public PFP_STANDARD
{
	// definition de la carte
	typedef EmbeddedMap2<Map2> MAP;
};


PFP::MAP myMap;


void MyQT::cb_initGL()
{
	// choose to use GL version 2
	Utils::GLSLShader::setCurrentOGLVersion(2);

	// create the render
	m_render = new Algo::Render::GL2::MapRender();

	// create VBO for position
	m_positionVBO = new Utils::VBO();


	m_shader = new Utils::ShaderFlat();
	m_shader->setAttributePosition(m_positionVBO);
	m_shader->setDiffuse(Geom::Vec4f(0.,1.,0.,0.));

	// using simple shader with color
	m_shader2 = new Utils::ShaderSimpleColor();
	m_shader2->setAttributePosition(m_positionVBO);
	m_shader2->setColor(Geom::Vec4f(1.,1.,0.,0.));


	registerRunning(m_shader);
	registerRunning(m_shader2);
}


void MyQT::cb_redraw()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_LIGHTING);

	m_render->draw(m_shader2, Algo::Render::GL2::LINES);

	glEnable( GL_POLYGON_OFFSET_FILL );
	glPolygonOffset( 1.0f, 1.0f );

	m_render->draw(m_shader, Algo::Render::GL2::TRIANGLES);

	glDisable(GL_POLYGON_OFFSET_FILL);
}




int main(int argc, char **argv)
{
	// interface:
	QApplication app(argc, argv);
	MyQT sqt;

	PFP::TVEC3 position = myMap.addAttribute<PFP::VEC3>(VERTEX_ORBIT, "position") ;

	// define the face extruded (here a cross)
	std::vector<PFP::VEC3> objV;
	objV.push_back(PFP::VEC3(-1, 0,-1));
	objV.push_back(PFP::VEC3(-1, 0,-2));
	objV.push_back(PFP::VEC3(+1, 0,-2));
	objV.push_back(PFP::VEC3(+1, 0,-1));
	objV.push_back(PFP::VEC3(+2, 0,-1));
	objV.push_back(PFP::VEC3(+2, 0,+1));
	objV.push_back(PFP::VEC3(+1, 0,+1));
	objV.push_back(PFP::VEC3(+1, 0,+2));
	objV.push_back(PFP::VEC3(-1, 0,+2));
	objV.push_back(PFP::VEC3(-1, 0,+1));
	objV.push_back(PFP::VEC3(-2, 0,+1));
	objV.push_back(PFP::VEC3(-2, 0,-1));

	// define a path (here an helicoid)
	std::vector<PFP::VEC3> pathV;
	// and a varying radius
	std::vector<float> pathRadius;
	for (int i=0; i < 200; ++ i)
	{
		PFP::REAL alpha = 3.141592/20.0 * i;
		PFP::REAL rad = 20.0 - 0.2*i;
		pathV.push_back(PFP::VEC3(rad*cos(alpha),rad*sin(alpha), 0.5*i));
		pathRadius.push_back(1.5f+0.7*cos(alpha));
	}

	// extrusion
	Dart d = Algo::Modelisation::extrusion_scale<PFP>(myMap, position, objV, PFP::VEC3(0.0,0.0,0.0), PFP::VEC3(0.0,1.0,0.0),true, pathV, false, pathRadius);

    //  bounding box
    Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position);
    float lWidthObj = std::max<PFP::REAL>(std::max<PFP::REAL>(bb.size(0), bb.size(1)), bb.size(2));
    Geom::Vec3f lPosObj = (bb.min() +  bb.max()) / PFP::REAL(2);

    // envoit info BB a l'interface
	sqt.setParamObject(lWidthObj,lPosObj.data());

	// show 1 pour GL context
	sqt.show();

	// update du VBO position (context GL necessaire)
	sqt.m_positionVBO->updateData(position);

	// update des primitives du renderer
	SelectorTrue allDarts;
	sqt.m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::GL2::TRIANGLES);
	sqt.m_render->initPrimitives<PFP>(myMap, allDarts, Algo::Render::GL2::LINES);

	// show final pour premier redraw
	sqt.show();

	// et on attend la fin.
	return app.exec();

}
