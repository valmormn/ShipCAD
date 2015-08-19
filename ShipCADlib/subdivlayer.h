/*###############################################################################################
 *    ShipCAD																					*
 *    Copyright 2015, by Greg Green <ggreen@bit-builder.com>									*
 *    Original Copyright header below															*
 *																								*
 *    This code is distributed as part of the FREE!ship project. FREE!ship is an                *
 *    open source surface-modelling program based on subdivision surfaces and intended for      *
 *    designing ships.                                                                          *
 *                                                                                              *
 *    Copyright © 2005, by Martijn van Engeland                                                 *
 *    e-mail                  : Info@FREEship.org                                               *
 *    FREE!ship project page  : https://sourceforge.net/projects/freeship                       *
 *    FREE!ship homepage      : www.FREEship.org                                                *
 *                                                                                              *
 *    This program is free software; you can redistribute it and/or modify it under             *
 *    the terms of the GNU General Public License as published by the                           *
 *    Free Software Foundation; either version 2 of the License, or (at your option)            *
 *    any later version.                                                                        *
 *                                                                                              *
 *    This program is distributed in the hope that it will be useful, but WITHOUT ANY           *
 *    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A           *
 *    PARTICULAR PURPOSE. See the GNU General Public License for more details.                  *
 *                                                                                              *
 *    You should have received a copy of the GNU General Public License along with              *
 *    this program; if not, write to the Free Software Foundation, Inc.,                        *
 *    59 Temple Place, Suite 330, Boston, MA 02111-1307 USA                                     *
 *                                                                                              *
 *##############################################################################################*/

#ifndef SUBDIVLAYER_H_
#define SUBDIVLAYER_H_

#include <iosfwd>
#include <vector>
#include <QObject>
#include <QColor>
#include <QString>
#include <QVector3D>

#include "subdivbase.h"

namespace ShipCAD {

//////////////////////////////////////////////////////////////////////////////////////

class SubdivisionControlFace;
class Viewport;
class FileBuffer;

//////////////////////////////////////////////////////////////////////////////////////

struct LayerProperties
{
    float surface_area;
    float weight;
    QVector3D surface_center_of_gravity;
};

//////////////////////////////////////////////////////////////////////////////////////

// SubdivisionLayer is a layer-type class
//
// All individual controlfaces can be assigned to a layer. Properties such as color,
// visibility etc. are common for all controlfaces belonging the the same layer
class SubdivisionLayer : public SubdivisionBase
{
    Q_OBJECT

public:

    explicit SubdivisionLayer(SubdivisionSurface* owner);
    virtual ~SubdivisionLayer();

    // modifications
    void deleteControlFace(SubdivisionControlFace* face);
    void addControlFace(SubdivisionControlFace* newface);
    size_t numberOfFaces() { return _patches.size(); }
    SubdivisionControlFace* getFace(size_t index);
    bool calculateIntersectionPoints(SubdivisionLayer* layer);
    virtual void clear();
    void assignProperties(SubdivisionLayer* source);
    void moveDown();
    void moveUp();

    void extents(QVector3D& min, QVector3D& max);
    //void unroll();
    LayerProperties getSurfaceProperties();

    // getters/setters
    bool isVisible() {return _visible;}
    bool isSymmetric() {return _symmetric;}
    bool isDevelopable() {return _developable;}
    bool useForIntersections() {return _use_for_intersections;}
    bool useInHydrostatics() {return _use_in_hydrostatics;}
    bool showInLinesplan() {return _show_in_linesplan;}
    size_t getLayerID() {return _layerid;}
    void setLayerID(size_t newid) { _layerid = newid; }
    size_t getLayerIndex();
    float getMaterialDensity() {return _material_density;}
    float getThickness() {return _thickness;}
    QString getName();
    QString getDescription();
    QString getDXFLayername() {return getName();}
    QColor getColor() {return _color;}
    float getAlphaBlend() {return _alphablend/255.0;}
    void setDevelopable(bool val);
    void setDescription(const QString& val) {_desc = val;}
    void setName(const QString& val);
    void setSymmetric(bool val);
    void setColor(QColor col);
    void setMaterialDensity(float val) {_material_density = val;}
    void setThickness(float val) {_thickness = val;}
    void setShowInLinesplan(bool val);
    void setUseInHydrostatics(bool val);
    void setUseForIntersections(bool val);
    void setVisible(bool val);

    // persistence
    void loadBinary(FileBuffer& source);
    void saveBinary(FileBuffer& destination);
    void saveToDXF(QStringList& strings);
    void loadFromStream(size_t& lineno, QStringList& strings);
    void saveToStream(QStringList& strings);

    // draw
    static void drawLayers(Viewport &vp, SubdivisionSurface* surface);
    virtual void draw(Viewport &vp);

    // output
    virtual void dump(std::ostream& os, const char* prefix = "") const;

    // makers
    static SubdivisionLayer* construct(SubdivisionSurface* owner);

signals:

    void changedLayerData(size_t layerid);

protected:

    void priv_dump(std::ostream& os, const char* prefix) const;

    // used in calculate intersection points
    void processTriangle(const QVector3D& p1,
                         const QVector3D& p2,
                         const QVector3D& p3,
                         LayerProperties& props);

protected:

    size_t _layerid;
    QColor _color;
    bool _visible;
    QString _desc;
    bool _symmetric;
    bool _developable;
    bool _use_for_intersections;
    bool _use_in_hydrostatics;
    bool _show_in_linesplan;
    float _material_density;
    float _thickness;
    unsigned char _alphablend;
    std::vector<SubdivisionControlFace*> _patches;
};

//////////////////////////////////////////////////////////////////////////////////////

};				/* end namespace */

std::ostream& operator << (std::ostream& os, const ShipCAD::SubdivisionLayer& layer);

#endif

