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
//$Rev:: 2184                                                              $
//$LastChangedDate:: 2012-07-02 18:47:25 +0200 (lun., 02 juil. 2012)       $
//**************************************************************************
//

#ifndef CC_POINT_PROPERTIES_DIALOG_HEADER
#define CC_POINT_PROPERTIES_DIALOG_HEADER

#include "ccPointPickingGenericInterface.h"

//Local
#include <ui_pointPropertiesDlg.h>

class cc2DLabel;
class cc2DViewportLabel;

//! Dialog for simple point picking (information, distance, etc.)
class ccPointPropertiesDlg : public ccPointPickingGenericInterface, public Ui::PointPropertiesDlg
{
    Q_OBJECT

public:

	ccPointPropertiesDlg(QWidget* parent);
	virtual ~ccPointPropertiesDlg();

    //inherited from ccPointPickingGenericInterface
	virtual bool start();
	virtual void stop(bool state);
    virtual void linkWith(ccGLWindow* win);

protected slots:

    void cancel();
    void activatePointPropertiesDisplay();
    void activateDistanceDisplay();
    void activateAngleDisplay();
	void activate2DZonePicking();
    void initializeState();
	void exportCurrentLabel();
	void update2DZone(int x, int y, Qt::MouseButtons buttons);
    void processClickedPoint(int x, int y);
	void close2DZone();

signals:

	//! Signal emitted when a new label is created
	void newLabel(ccHObject*);

protected:

    //! Picking mode
    enum Mode
    {
        POINT_INFO,
        POINT_POINT_DISTANCE,
        POINTS_ANGLE,
		RECT_ZONE
    };

    //inherited from ccPointPickingGenericInterface
    void processPickedPoint(ccPointCloud* cloud, unsigned pointIndex, int x, int y);

	//! Current picking mode
    Mode m_pickingMode;

	//! Associated 3D label
	cc2DLabel* m_label;

	//! Associated 2D label
	cc2DViewportLabel* m_rect2DLabel;

};

#endif
