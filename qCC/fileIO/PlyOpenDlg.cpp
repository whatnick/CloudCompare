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
//$Rev:: 1992                                                              $
//$LastChangedDate:: 2012-01-18 12:17:49 +0100 (mer., 18 janv. 2012)       $
//**************************************************************************
//
#include "PlyOpenDlg.h"

#include <QMessageBox>

#include <assert.h>

PlyOpenDlg::PlyOpenDlg(QWidget* parent) : QDialog(parent), Ui::PlyOpenDlg()
{
    setupUi(this);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(testBeforeAccept()));
    connect(this, SIGNAL(fullyAccepted()), this, SLOT(accept()));
}

void PlyOpenDlg::testBeforeAccept()
{
    //we need at least two coordinates for a points (i.e. 2D)
    int zeroCoord=0;
    if (xComboBox->currentIndex()==0) ++zeroCoord;
    if (yComboBox->currentIndex()==0) ++zeroCoord;
    if (zComboBox->currentIndex()==0) ++zeroCoord;

    if (zeroCoord>1)
    {
        QMessageBox::warning(0, "Error",
            "At least two vertex coordinates (X,Y,Z) must be defined!");
        return;
    }

    //we must assure that no property is assigned to more than one field
    int i,n = xComboBox->count();
    assert(n>0);
    int* assignedIndexCount = new int[n];
    memset(assignedIndexCount,0,n*sizeof(int));

    ++assignedIndexCount[xComboBox->currentIndex()];
    ++assignedIndexCount[yComboBox->currentIndex()];
    ++assignedIndexCount[zComboBox->currentIndex()];
    ++assignedIndexCount[rComboBox->currentIndex()];
    ++assignedIndexCount[gComboBox->currentIndex()];
    ++assignedIndexCount[bComboBox->currentIndex()];
    ++assignedIndexCount[iComboBox->currentIndex()];
    ++assignedIndexCount[sfComboBox->currentIndex()];
    ++assignedIndexCount[nxComboBox->currentIndex()];
    ++assignedIndexCount[nyComboBox->currentIndex()];
    ++assignedIndexCount[nzComboBox->currentIndex()];
    //++assignedIndexCount[facesComboBox->currentIndex()];

    for (i=1;i<n;++i)
        if (assignedIndexCount[i]>1)
        {
            delete[] assignedIndexCount;
            QMessageBox::warning(0, "Error",
                QString("Can't assign same property to multiple fields! (%1)").arg(xComboBox->itemText(i)));
            return;
        }

    delete[] assignedIndexCount;
    assignedIndexCount=0;

    emit fullyAccepted();
}
