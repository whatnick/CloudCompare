//##########################################################################
//#                                                                        #
//#                            CLOUDCOMPARE                                #
//#                                                                        #
//#  This program is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU General Public License as published by  #
//#  the Free Software Foundation; version 2 of the License.               #
//#                                                                        #
//#  This program is distributed in the hope that it will be useful,       #
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
//#  GNU General Public License for more details.                          #
//#                                                                        #
//#          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
//#                                                                        #
//##########################################################################
//
//*********************** Last revision of this file ***********************
//$Author:: dgm                                                            $
//$Rev:: 2101                                                              $
//$LastChangedDate:: 2012-05-03 18:19:18 +0200 (jeu., 03 mai 2012)         $
//**************************************************************************
//

#include "ccGraphicalTransformationTool.h"

#include <Matrix.h>

#include <ccHObject.h>
#include "ccGLWindow.h"
#include "ccConsole.h"
#include "mainwindow.h"

ccGraphicalTransformationTool::ccGraphicalTransformationTool(QWidget* parent)
	: QDialog(parent)
	, Ui::GraphicalTransformationDlg()
	, m_toTransform(0)
	, m_associatedWin(0)
	, m_started(false)
{
	setupUi(this);
	setWindowFlags(Qt::FramelessWindowHint |Qt::Tool);

	connect(okButton,       SIGNAL(clicked()), this, SLOT(apply()));
	connect(razButton,      SIGNAL(clicked()), this, SLOT(reset()));
	connect(cancelButton,   SIGNAL(clicked()), this, SLOT(cancel()));

	m_toTransform = new ccHObject("ToTransform");
}

ccGraphicalTransformationTool::~ccGraphicalTransformationTool()
{
	assert(!m_started);

	clear();

	delete m_toTransform;
	m_toTransform=0;
}

void ccGraphicalTransformationTool::clear()
{
	m_toTransform->removeAllChildren();

	m_rotation.toIdentity();
	m_translation = CCVector3(0,0,0);
	m_rotationCenter = CCVector3(0,0,0);
}

bool ccGraphicalTransformationTool::addEntity(ccHObject* entity)
{
	//we don't handle entities associated to another context
	if (entity->getDisplay()!=m_associatedWin)
	{
		ccConsole::Warning("[Graphical Transformation Tool] Can't use entity '%s' cause it's not displayed in the active 3D view!",entity->getName());
		return false;
	}

	//we can't tranform locked entities
	if (entity->isLocked())
	{
		ccConsole::Warning("[Graphical Transformation Tool] Can't transform entity '%s' cause it's locked!",entity->getName());
		return false;
	}

	//we can't tranform child meshes
	if (entity->isA(CC_MESH) && entity->getParent() && entity->getParent()->isKindOf(CC_MESH))
	{
		ccConsole::Warning("[Graphical Transformation Tool] Entity '%s' can't be modified as it is part of a mesh group. You should 'clone' it first.",entity->getName());
		return false;
	}

	//eventually, we must check that there is no "parent + sibling" in the selection!
	//otherwise, the sibling will be rotated twice!
	assert(m_toTransform);
	unsigned i,n=m_toTransform->getChildrenNumber();
	for (i=0; i<n; ++i)
	{
		ccHObject* previous = m_toTransform->getChild(i);
		if (previous->isAncestorOf(entity))
		{
			//we have found a parent, we won't add this entity
			return false;
		}
		//if the inverse is true, then we get rid of the current element!
		else if (entity->isAncestorOf(previous))
		{
			m_toTransform->removeChild(previous);
		}
	}

	m_toTransform->addChild(entity,false);

	return true;
}

unsigned ccGraphicalTransformationTool::getNumberOfValidEntities()
{
	assert(m_toTransform);
	return m_toTransform->getChildrenNumber();
}

void ccGraphicalTransformationTool::linkWith(ccGLWindow* win)
{
	if (m_associatedWin == win)
		return;

	m_associatedWin = win;
	
	assert(m_toTransform);
	assert(m_toTransform->getChildrenNumber()==0);
	m_toTransform->setDisplay(win);
}

bool ccGraphicalTransformationTool::start()
{
	assert(!m_started);
	assert(m_associatedWin);
	assert(m_toTransform);

	unsigned childNum=m_toTransform->getChildrenNumber();
	if (childNum==0)
		return false;

	m_rotation.toIdentity();
	m_translation = CCVector3(0,0,0);
	m_rotationCenter = m_toTransform->getCenter(); //m_rotation center == selected entities center

	//activate "moving mode" in associated GL window
	m_associatedWin->setInteractionMode(ccGLWindow::TRANSFORM_ENTITY);
	m_associatedWin->setPickingMode(ccGLWindow::NO_PICKING);
	//the user must not close this window!
	m_associatedWin->setUnclosable(true);
	connect(m_associatedWin, SIGNAL(rotation(const ccGLMatrix&)),	this, SLOT(glRotate(const ccGLMatrix&)));
	connect(m_associatedWin, SIGNAL(translation(const CCVector3&)),	this, SLOT(glTranslate(const CCVector3&)));
	m_associatedWin->displayNewMessage("[Rotation/Translation mode]",3600);
	m_associatedWin->updateGL();
	m_started = true;

	show();

	return true;
}

void ccGraphicalTransformationTool::stop()
{
	if (!m_associatedWin)
		return;

	//deactivate "moving mode" in associated GL window
	m_associatedWin->setInteractionMode(ccGLWindow::TRANSFORM_CAMERA);
	m_associatedWin->setPickingMode(ccGLWindow::DEFAULT_PICKING);
	m_associatedWin->setUnclosable(false);
	disconnect(m_associatedWin, 0, this, 0);
	m_associatedWin->displayNewMessage("[Rotation/Translation mode OFF]");
	m_associatedWin->updateGL();
	m_started = false;
}

void ccGraphicalTransformationTool::glTranslate(const CCVector3& t)
{
	m_translation += t;

	updateAllGLTransformations();
}

void ccGraphicalTransformationTool::glRotate(const ccGLMatrix& rotMat)
{
	m_rotation = rotMat * m_rotation;

	updateAllGLTransformations();
}

void ccGraphicalTransformationTool::reset()
{
	m_rotation.toIdentity();
	m_translation = CCVector3(0,0,0);

	updateAllGLTransformations();
}

void ccGraphicalTransformationTool::updateAllGLTransformations()
{
	assert(m_toTransform);

	//we recompute global GL transformation matrix
	ccGLMatrix newTrans = m_rotation;
	newTrans += (m_rotationCenter+m_translation-m_rotation*m_rotationCenter);

	for (unsigned i=0; i<m_toTransform->getChildrenNumber();++i)
	{
		ccHObject* child = m_toTransform->getChild(i);
		child->setGLTransformation(newTrans);
		child->prepareDisplayForRefresh_recursive();
	}

	MainWindow::RefreshAllGLWindow();
}

void ccGraphicalTransformationTool::apply()
{
	assert(m_toTransform);

	//we recompute global GL transformation matrix
	ccGLMatrix finalTrans = m_rotation;
	finalTrans += (m_rotationCenter+m_translation-m_rotation*m_rotationCenter);
	//output resulting transformation matrix
	ccConsole::Print("[GraphicalTransformationTool] Applied transformation:");
	const float* mat = finalTrans.data();
	ccConsole::Print("%6.12f\t%6.12f\t%6.12f\t%6.12f\n%6.12f\t%6.12f\t%6.12f\t%6.12f\n%6.12f\t%6.12f\t%6.12f\t%6.12f\n%6.12f\t%6.12f\t%6.12f\t%6.12f",mat[0],mat[4],mat[8],mat[12],mat[1],mat[5],mat[9],mat[13],mat[2],mat[6],mat[10],mat[14],mat[3],mat[7],mat[11],mat[15]);

	for (unsigned i=0; i<m_toTransform->getChildrenNumber();++i)
	{
		ccHObject* child = m_toTransform->getChild(i);
		child->applyGLTransformation_recursive();
		child->prepareDisplayForRefresh_recursive();
	}

	emit transformationFinished(true);

	clear();

	//MainWindow::RefreshAllGLWindow();

}

void ccGraphicalTransformationTool::cancel()
{
	assert(m_toTransform);

	for (unsigned i=0; i<m_toTransform->getChildrenNumber();++i)
	{
		ccHObject* child = m_toTransform->getChild(i);
		child->razGLTransformation();
		child->prepareDisplayForRefresh_recursive();
	}

	emit transformationFinished(false);

	clear();

	//MainWindow::RefreshAllGLWindow();
}
