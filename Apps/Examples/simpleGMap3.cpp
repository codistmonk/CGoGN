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

#include "simpleGMap3.h"
#include "Utils/GLSLShader.h"
#include "Algo/Geometry/boundingbox.h"
#include "Algo/Modelisation/primitives3d.h"


SimpleGMap3::SimpleGMap3()
{
	position = myMap.addAttribute<PFP::VEC3>(VERTEX, "position");

//	Algo::Modelisation::Primitive3D<PFP> prim(myMap,position);
//	Dart d = prim.hexaGrid_topo(2,2,1);
//	prim.embedHexaGrid(1,1,1);
//
//	Dart d1 = myMap.phi1(myMap.phi1(myMap.phi2(myMap.phi1(myMap.phi1(d)))));
//	VEC3 mid0 = (position[d1]+position[myMap.phi1(d1)])/2.0f;
//	myMap.cutEdge(d1);
//	position[myMap.phi1(d1)] = mid0;
//
//	VEC3 mid1 = (position[d]+position[myMap.phi1(d)])/2.0f;
//	myMap.cutEdge(d);
//	position[myMap.phi1(d)] = mid1;
//
////	d = myMap.phi1(myMap.phi1(d));
//	d = myMap.phi1(myMap.phi1(myMap.phi2(myMap.phi1(myMap.phi1(d)))));
//	VEC3 mid = (position[d]+position[myMap.phi1(d)])/2.0f;
//	myMap.cutEdge(d);
//	position[myMap.phi1(d)] = mid;

	Algo::Modelisation::Primitive3D<PFP> prim2(myMap,position);
	Dart d = prim2.hexaGrid_topo(2,1,1);
	prim2.embedHexaGrid(1,1,1);

	d = myMap.phi2(myMap.phi1(myMap.phi1(myMap.phi2(d))));
	myMap.unsewVolumes(d);
}

void SimpleGMap3::initGUI()
{

}

void SimpleGMap3::cb_initGL()
{
	Utils::GLSLShader::setCurrentOGLVersion(1) ;

	Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position) ;
	VEC3 gPosObj = bb.center() ;
	float tailleX = bb.size(0) ;
	float tailleY = bb.size(1) ;
	float tailleZ = bb.size(2) ;
	float gWidthObj = std::max<float>(std::max<float>(tailleX, tailleY), tailleZ) ;
	setParamObject(gWidthObj, gPosObj.data());
}

void SimpleGMap3::cb_redraw()
{
	glDisable(GL_LIGHTING);
	glLineWidth(1.0f);
	Algo::Render::GL1::renderTopoGMD3<PFP>(myMap, position, true, true, true, true, 0.9f, 0.9f, 0.9f, 0.9f);
}


/**********************************************************************************************
 *                                      MAIN FUNCTION                                         *
 **********************************************************************************************/

int main(int argc, char **argv)
{
	QApplication app(argc, argv) ;

	SimpleGMap3 sqt ;
	sqt.setGeometry(0, 0, 1000, 800) ;
 	sqt.show() ;

	sqt.initGUI() ;

	return app.exec() ;
}

