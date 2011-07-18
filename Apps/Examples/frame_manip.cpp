/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009-2011, IGG Team, LSIIT, University of Strasbourg           *
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
* Web site: http://cgogn.u-strasbg.fr/                                         *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#include <iostream>

#include "frame_manip.h"

#include "Topology/generic/parameters.h"
#include "Topology/map/map2.h"
#include "Topology/generic/embeddedMap2.h"

#include "Geometry/vector_gen.h"
#include "Geometry/matrix.h"
#include "Geometry/transfo.h"
#include "Algo/Geometry/boundingbox.h"

#include "Algo/Modelisation/polyhedron.h"
#include "Topology/generic/parameters.h"

#include "Algo/Render/GL2/mapRender.h"
#include "Utils/Shaders/shaderFlat.h"
#include "Utils/Shaders/shaderSimpleColor.h"


#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_precision.hpp"
#include "glm/gtc/type_ptr.hpp"


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

	m_frame = new Utils::FrameManipulator();

	// frame is rendered twice the size of normal
	m_frame->setSize(2.f);

	// none axis is picked
	m_pickedAxis=0;

	// subdivision of 7 for each drawable
	m_precDraw=9;

	// create the Drawables (one of each)
	Utils::LineDrawable* ldp = new Utils::Grid(m_precDraw);
	ldp->setColor(Geom::Vec4f(0.0f,0.0f,0.8f,1.0f));
	m_ld.push_back(ldp);

	ldp = new Utils::Sphere(m_precDraw,m_precDraw);
	ldp->setColor(Geom::Vec4f(0.0f,0.8f,0.0f,1.0f));
	m_ld.push_back(ldp);

	ldp = new Utils::Cube(m_precDraw);
	ldp->setColor(Geom::Vec4f(0.8f,0.0f,0.0f,1.0f));
	m_ld.push_back(ldp);

	ldp = new Utils::Cone(m_precDraw,m_precDraw);
	ldp->setColor(Geom::Vec4f(0.8f,0.0f,0.8f,1.0f));
	m_ld.push_back(ldp);


	ldp= new Utils::Cylinder(m_precDraw,m_precDraw);
	ldp->setColor(Geom::Vec4f(0.0f,0.8f,0.8f,1.0f));
	m_ld.push_back(ldp);

	ldp= new Utils::IcoSphere(m_precDraw);
	ldp->setColor(Geom::Vec4f(0.8f,0.3f,0.1f,1.0f));
	m_ld.push_back(ldp);


	// create a grid of pickables
	Geom::Vec3f center(float(NBP-1)*2.0f, float(NBP-1)*2.0f, float(NBP-1)*2.0f);
	for (unsigned int i=0; i< NBP; ++i)
	{
		for (unsigned int j=0; j< NBP; ++j)
		{
			for (unsigned int k=0; k< NBP; ++k)
			{
				unsigned int idx = i*NBP*NBP + j*NBP + k;

				Utils::Pickable* pick = new Utils::Pickable(m_ld[idx%m_ld.size()],idx);
				m_pickables.push_back(pick);

				// attention transfo appliques en ordre inverse !
				Geom::Vec3f position( 4.0f * Geom::Vec3f(i,j,k) - center);
				pick->translate(position);
				pick->randomOrientation();
				pick->randomUniformScale(0.8f,1.6f);
			}
		}
	}


	m_lastPickedObject = m_pickables.front();
	m_frame->setTransformation(m_lastPickedObject->transfo());

}


void MyQT::cb_redraw()
{
	// affichage du frame
	if (m_lastPickedObject)
		m_frame->draw();

	// affichage des objets
	for (std::vector<Utils::Pickable*>::iterator it=m_pickables.begin(); it != m_pickables.end(); ++it)
	{
		if (*it == m_lastPickedObject)
			glLineWidth(2.0);
		else
			glLineWidth(1.0);
		(*it)->draw();
	}

}

void  MyQT::cb_mousePress(int button, int x, int y)
{
	if (!Shift())
		return;

	m_begX = x;
	m_begY = y;

	// get ray of selection
	Geom::Vec3f rayA,rayB;
	float dist = getOrthoScreenRay(x,y,rayA,rayB);
	Geom::Vec3f AB = rayB-rayA;


	unsigned int fr_picked =0;
	if (m_lastPickedObject)	// an object is already picked ? => frame is drawn
	{
		// picking the frame -> axis
		fr_picked = m_frame->pick(rayA,AB,dist);
		// same already picked -> unpick
		if (fr_picked == m_pickedAxis)
			m_pickedAxis = 0;
		else
			m_pickedAxis=fr_picked;
	}

	// project center & axis on screen for easy manipulation
	if (m_pickedAxis != Utils::FrameManipulator::NONE)
		m_frame->storeProjection(m_pickedAxis);

	// highlighting
	m_frame->highlight(m_pickedAxis);

	if (fr_picked == 0)	// frame not picked -> pick the pickable objects
	{
		m_lastPickedObject = Utils::Pickable::pick(m_pickables,rayA,AB);
		// set FrameManipulator on picked object
		if (m_lastPickedObject)
		{
			m_frame->setTransformation(m_lastPickedObject->transfo());
			std::cout << "Picked "<<m_lastPickedObject->shape() << " "<< m_lastPickedObject->id()<< std::endl;
		}
	}

	updateGL();

}

void  MyQT::cb_mouseMove(int button, int x, int y)
{
	if (!Shift())
		return;

	if (Utils::FrameManipulator::rotationAxis(m_pickedAxis))
	{
		float angle = m_frame->angleFromMouse(x,y,x-m_begX, y-m_begY);
		m_frame->rotate(m_pickedAxis, angle);
		m_lastPickedObject->transfo() = m_frame->transfo();
	}
	else if (Utils::FrameManipulator::translationAxis(m_pickedAxis))
	{
		float dist =  m_frame->distanceFromMouse(x-m_begX, y-m_begY);
		m_frame->translate(m_pickedAxis, dist);
		m_lastPickedObject->transfo() = m_frame->transfo();
	}
	else if (Utils::FrameManipulator::scaleAxis(m_pickedAxis) )
	{
		float scale = m_frame->scaleFromMouse(x-m_begX, y-m_begY);
		m_frame->scale(m_pickedAxis, scale );
		m_lastPickedObject->transfo() = m_frame->transfo();
	}

	m_begX = x;
	m_begY = y;
	updateGL();
	return;

}


void  MyQT::cb_wheelEvent(int delta, int x, int y)
{
	if (!Shift())
		return;

	if (!m_lastPickedObject)
		return;

	if (Utils::FrameManipulator::rotationAxis(m_pickedAxis))
		m_frame->rotate(m_pickedAxis, float(delta)/60.0f);
	else if (Utils::FrameManipulator::translationAxis(m_pickedAxis))
		m_frame->translate(m_pickedAxis, float(-delta)/1200.0f);
	else if (Utils::FrameManipulator::scaleAxis(m_pickedAxis) )
		m_frame->scale(m_pickedAxis, 1.0f+ float(-delta)/1200.0f);
	else
		m_frame->addSize(float(-delta)/480.0f);

	m_lastPickedObject->transfo() = m_frame->transfo();
	updateGL();
}

void  MyQT::cb_keyPress(int code)
{
	switch(code)
	{
	case '+':
		if (m_precDraw < 128)
			m_precDraw++;
		for (std::vector<Utils::LineDrawable*>::iterator it=m_ld.begin(); it != m_ld.end(); ++it)
			(*it)->updatePrecisionDrawing(m_precDraw);
		break;

	case '-':
		if (m_precDraw > 3)
			m_precDraw--;
		for (std::vector<Utils::LineDrawable*>::iterator it=m_ld.begin(); it != m_ld.end(); ++it)
			(*it)->updatePrecisionDrawing(m_precDraw);
		break;

	case 'r':
		for (unsigned int i=0; i< NBP*NBP*NBP; ++i)
			m_pickables[i]->randomOrientation();
		if (m_lastPickedObject)
			m_frame->setTransformation(m_lastPickedObject->transfo());
		break;
	case 'x':
		m_frame->lock(Utils::FrameManipulator::Xt);
		break;
	case 'y':
		m_frame->lock(Utils::FrameManipulator::Yt);
		break;
	case 'z':
		m_frame->lock(Utils::FrameManipulator::Zt);
		break;
	case 'X':
		m_frame->unlock(Utils::FrameManipulator::Xt);
		break;
	case 'Y':
		m_frame->unlock(Utils::FrameManipulator::Yt);
		break;
	case 'Z':
		m_frame->unlock(Utils::FrameManipulator::Zt);
		break;

	case 'a':
		m_frame->lock(Utils::FrameManipulator::Xr);
		break;
	case 'b':
		m_frame->lock(Utils::FrameManipulator::Yr);
		break;
	case 'c':
		m_frame->lock(Utils::FrameManipulator::Zr);
		break;
	case 'A':
		m_frame->unlock(Utils::FrameManipulator::Xr);
		break;
	case 'B':
		m_frame->unlock(Utils::FrameManipulator::Yr);
		break;
	case 'C':
		m_frame->unlock(Utils::FrameManipulator::Zr);
		break;
	}
	updateGL();
}


int main(int argc, char **argv)
{


	// interface:
	QApplication app(argc, argv);
	MyQT sqt;

	if (argc>1)
		sqt.NBP = atoi(argv[1]);

    //  bounding box
    Geom::Vec3f lPosObj = Geom::Vec3f(0.0f,0.0f,0.0f);
    float lWidthObj = sqt.NBP*4.0f;

    // envoit info BB a l'interface
	sqt.setParamObject(lWidthObj,lPosObj.data());

	// show final pour premier redraw
	sqt.show();

	// et on attend la fin.
	return app.exec();

	return 0;
}
