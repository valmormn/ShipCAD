/*##############################################################################################
 *    ShipCAD																				   *
 *    Copyright 2017, by Greg Green <ggreen@bit-builder.com>								   *
 *    Original Copyright header below														   *
 *																							   *
 *    This code is distributed as part of the FREE!ship project. FREE!ship is an               *
 *    open source surface-modelling program based on subdivision surfaces and intended for     *
 *    designing ships.                                                                         *
 *                                                                                             *
 *    Copyright © 2005, by Martijn van Engeland                                                *
 *    e-mail                  : Info@FREEship.org                                              *
 *    FREE!ship project page  : https://sourceforge.net/projects/freeship                      *
 *    FREE!ship homepage      : www.FREEship.org                                               *
 *                                                                                             *
 *    This program is free software; you can redistribute it and/or modify it under            *
 *    the terms of the GNU General Public License as published by the                          *
 *    Free Software Foundation; either version 2 of the License, or (at your option)           *
 *    any later version.                                                                       *
 *                                                                                             *
 *    This program is distributed in the hope that it will be useful, but WITHOUT ANY          *
 *    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A          *
 *    PARTICULAR PURPOSE. See the GNU General Public License for more details.                 *
 *                                                                                             *
 *    You should have received a copy of the GNU General Public License along with             *
 *    this program; if not, write to the Free Software Foundation, Inc.,                       *
 *    59 Temple Place, Suite 330, Boston, MA 02111-1307 USA                                    *
 *                                                                                             *
 *#############################################################################################*/

#ifndef DIALOGDATA_H_
#define DIALOGDATA_H_

#include <QtCore>
#include <QColorDialog>
#include <vector>
#include "shipcadlib.h"

namespace ShipCAD {

    class SubdivisionLayer;
    
//////////////////////////////////////////////////////////////////////////////////////
/*! \brief data structure for insert plane dialog exchange
 */
struct InsertPlaneDialogData
{
    bool accepted;
    bool addControlCurveSelected;
    plane_selected_t planeSelected;
    float distance;
    QVector3D min;
    QVector3D max;
    explicit InsertPlaneDialogData();
};

//////////////////////////////////////////////////////////////////////////////////////
/*! \brief data structure for intersect layer dialog exchange
 */
struct IntersectLayersDialogData
{
    bool accepted;
    std::vector<SubdivisionLayer*> layers;
    size_t layer1;
    size_t layer2;
    explicit IntersectLayersDialogData();
};

//////////////////////////////////////////////////////////////////////////////////////
/*! \brief data structure for extrude edge dialog exchange
 */
struct ExtrudeEdgeDialogData
{
    bool accepted;
    QVector3D vector;
    explicit ExtrudeEdgeDialogData();
};

//////////////////////////////////////////////////////////////////////////////////////
/*! \brief data structure for color chooser dialog exchange
 */
struct ChooseColorDialogData
{
    bool accepted;
    QString title;
    const QColor& initial;
    QColorDialog::ColorDialogOptions options;
    QColor chosen;
    explicit ChooseColorDialogData(const QString& title, const QColor& initial);
};

//////////////////////////////////////////////////////////////////////////////////////

};				/* end namespace */

#endif

