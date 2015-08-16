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

#include <iostream>
#include <cmath>
#include <stdexcept>
#include <algorithm>

#include "intersection.h"
#include "filebuffer.h"
#include "spline.h"
#include "shipcadmodel.h"
#include "viewport.h"
#include "shader.h"
#include "projsettings.h"
#include "preferences.h"
#include "subdivsurface.h"
#include "utility.h"

using namespace std;
using namespace ShipCAD;

Intersection::Intersection(ShipCADModel* owner)
    : _owner(owner), _items(true), _intersection_type(fiStation),
      _show_curvature(false), _use_hydrostatic_surfaces_only(false)
{
    // does nothing
}

Intersection::Intersection(ShipCADModel *owner, intersection_type_t ty, const Plane& pln, bool use_hydrostatics_only)
    : _owner(owner), _intersection_type(ty), _plane(pln),
      _show_curvature(false), _use_hydrostatic_surfaces_only(use_hydrostatics_only)
{
    // does nothing
}

Intersection::~Intersection()
{
    clear();
}

void Intersection::clear()
{
    // clear method deletes all memory for splines
    _items.clear();
    _show_curvature = false;
    _use_hydrostatic_surfaces_only = false;
    Entity::clear();
}

void Intersection::setBuild(bool val)
{
    if (val)
        clear();
	Entity::setBuild(val);
}

QColor Intersection::getColor()
{
    switch(_intersection_type) {
    case fiStation:
        return _owner->getPreferences().getStationColor();
    case fiButtock:
        return _owner->getPreferences().getButtockColor();
    case fiWaterline:
        return _owner->getPreferences().getWaterlineColor();
    case fiDiagonal:
        return _owner->getPreferences().getDiagonalColor();
    }
    return Qt::white;
}

struct SplineExtents
{
    QVector3D& _min;
    QVector3D& _max;
    SplineExtents(QVector3D& min, QVector3D& max)
        : _min(min), _max(max) {}
    void operator ()(Spline *s) {
        s->extents(_min, _max);
    }
};

void Intersection::extents(QVector3D& min, QVector3D& max)
{
    if (!_build)
        rebuild();
    SplineExtents se(min, max);
    for_each(_items.begin(), _items.end(), se);
}

QString Intersection::getDescription()
{
    return "";
}

void Intersection::draw(Viewport& vp, LineShader* lineshader)
{
    // TODO
}

static void Simplify(Spline* s)
{
    s->simplify(2.0);
}

void Intersection::rebuild()
{
    setBuild(false);
    _owner->getSurface()->intersectPlane(_plane, _use_hydrostatic_surfaces_only, _items);
    // use a low simplification factor to remove only points that are (nearly) on a line
    if (_owner->getProjectSettings().getSimplifyIntersections()) {
        _items.apply(Simplify);
    }
    _build = true;
}

struct CalculateSplineArea
{
    float* _area;
    QVector3D* _cog;
    QVector2D* _moi;
    intersection_type_t _intersection_type;
    Plane _plane;
    CalculateSplineArea(float* area, QVector3D* cog, QVector2D* mom_inertia, intersection_type_t ty, const Plane& pln)
        : _area(area), _cog(cog), _moi(mom_inertia), _intersection_type(ty), _plane(pln)
    {}
    QVector2D ProjectTo2D(QVector3D& p)
    {
        QVector2D result;
        switch (_intersection_type) {
        case fiStation:
            result = QVector2D(p.y(), p.z());
            break;
        case fiButtock:
            result = QVector2D(p.x(), p.z());
            break;
        case fiWaterline:
            result = QVector2D(p.x(), p.y());
            break;
        default:
            result = ZERO2;
        }
        return result;
    }

    void operator()(Spline* spline)
    {
        bool closed_spline;
        IntersectionData intersection_data;
        vector<float> parameters;
        *_area = 0;
        *_cog = ZERO;
        *_moi = ZERO2;
        QVector2D c = ZERO2;
        QVector2D momi = c;
        closed_spline = DistPP3D(spline->getPoint(0), spline->getPoint(spline->numberOfPoints()-1)) < 1e-4;
        if (!closed_spline) {
            // make it closed
            spline->add(spline->getPoint(0));
            spline->setKnuckle(spline->numberOfPoints()-2, true);
        }
        spline->setFragments(500);
        parameters.push_back(0);
        parameters.push_back(1.0);
        if (_intersection_type != fiWaterline) {
            if (spline->intersect_plane(_plane, intersection_data)) {
                for (size_t i=0; i<intersection_data.number_of_intersections; i++)
                    parameters.push_back(intersection_data.parameters[i]);
            }
        }
        sort(parameters.begin(), parameters.end());
        if (parameters.size()) {
            float t1 = 0;
            for (size_t i=2; i<parameters.size(); i++) {
                float t2 = parameters[i-1];
                float t = 0.5 * (t1 + t2);
                QVector3D p = spline->value(t);
                // check on which side of the plane this point is
                float side = _plane.distance(p);
                if (side < 0 || _intersection_type == fiWaterline) {
                    // the point lies at the back of the plane, include this area
                    QVector2D p1 = ZERO2;
                    for (size_t j=0; j<500; j++) {
                        t = t1 + (j/500.0) * (t2-t1);
                        p = spline->value(t);
                        QVector2D p2 = ProjectTo2D(p);
                        if (j > 0) {
                            float delta = 0.5 * (p2.x() + p1.x()) * (p2.y() - p1.y());
                            *_area += delta;
                            c.setX(c.x() + delta * .25 * (p2.x() + p1.x()));
                            c.setY(c.y() + delta * .5 * (p2.y() + p1.y()));
                            momi.setX(momi.x() + (1/12.) * (p1.y()+p2.y())*(p1.y()*p1.y()+p2.y()*p2.y())*(p2.x()-p1.x()));
                            momi.setY(momi.y() + (1/12.) * (p2.x()*p2.x()*(3*p2.y()+p1.y())+2*p1.x()*p2.x()*(p1.y()+p2.y())+p1.x()*p1.x()*(3*p1.y()+p2.y()))*(p2.x()-p1.x()));
                        }
                        p1 = p2;
                    }
                }
                t1 = t2;
            }
            if (_area != 0) {
                c /= *_area;
                momi.setX(fabs(momi.x()));
                momi.setY(fabs(momi.y()));
                switch (_intersection_type) {
                case fiStation:
                    _cog->setX(-_plane.d());
                    _cog->setY(c.x());
                    _cog->setZ(c.y());
                    _moi->setX(0);
                    _moi->setY(momi.x());
                    break;
                case fiButtock:
                    _cog->setX(c.x());
                    _cog->setY(-_plane.d());
                    _cog->setZ(c.y());
                    _moi->setX(momi.x());
                    _moi->setY(0);
                    break;
                case fiWaterline:
                    _cog->setX(c.x());
                    _cog->setY(c.y());
                    _cog->setZ(-_plane.d());
                    _moi->setX(momi.x());
                    _moi->setY(momi.y());
                    break;
                }
            }
        }
    }
};

void Intersection::calculateArea(const Plane& plane, float* area, QVector3D* cog, QVector2D* moment_of_inertia)
{
    *area = 0;
    *cog = ZERO;
    *moment_of_inertia = ZERO2;
    QVector2D momi;
    if (!isBuild())
        rebuild();
    if (_items.size() > 0) {
        createStarboardPart();  // this ensures correct winding order
        CalculateSplineArea calc(area, cog, moment_of_inertia, _intersection_type, plane);
        for_each(_items.begin(), _items.end(), calc);
        if (*area != 0) {
            *cog = *cog / *area;
            if (_intersection_type == fiWaterline) {
                moment_of_inertia->setX(moment_of_inertia->x() - cog->y() * cog->y() * *area);
                moment_of_inertia->setY(moment_of_inertia->y() - cog->x() * cog->x() * *area);
            }
        }
    }
    if (_items.size() == 0 || *area == 0) {
        *cog = ZERO;
        switch (_intersection_type) {
        case fiStation:
            cog->setX(-_plane.d());
            break;
        case fiButtock:
            cog->setY(-_plane.d());
            break;
        case fiWaterline:
            cog->setZ(-_plane.d());
            break;
        }
    }
}

void Intersection::createStarboardPart()
{
    if (_intersection_type != fiButtock) {
        // copy all present splines and mirror the y coordinate
        for (size_t i=_items.size(); i>=1; i--) {
            Spline* spline = new Spline(*_items.get(i-1));
            for (size_t j=0; j<spline->numberOfPoints(); j++) {
                QVector3D p1 = spline->getPoint(j);
                p1.setY(-p1.y());
                spline->setPoint(j, p1);
            }
            _items.add(spline);
        }
        // try to connect the splines
        JoinSplineSegments(0.05f, false, _items);
    }
    // check if the orientation is counterclockwise
    for (size_t i=_items.size(); i>=1; i--) {
        Spline* spline = _items.get(i-1);
        float area = 0;
        QVector3D p1 = spline->getPoint(spline->numberOfPoints()-1);
        for (size_t j=0; j<500; j++) {
            QVector3D p2 = spline->value(j/500.);
            float delta;
            switch (_intersection_type) {
            case fiStation:
                delta = 0.5 * (p2.y() + p1.y()) * (p2.z() - p1.z());
                break;
            case fiButtock:
                delta = 0.5 * (p2.x() + p1.x()) * (p2.z() - p1.z());
                break;
            case fiWaterline:
                delta = 0.5 * (p2.x() + p1.x()) * (p2.y() - p1.y());
                break;
            default:
                throw runtime_error("66");
            }
            area += delta;
            p1 = p2;
        }
        if (fabs(area) < 1e-4 && _items.size() > 0) {
            // either this spline has a very small area (.01m x .01m) or it is an
            // unconnected straight line. In both cases it may be deleted as long
            // as at least one valid spline segment remains
            _items.del(spline);
        } else if (area < 0) {
            // spline is defined clockwise, so invert the controlpoints
            spline->invert_direction();
        }
    }
}

void Intersection::deleteItem(Spline* item)
{
    _items.del(item);
}

void Intersection::loadBinary(FileBuffer& source)
{
    size_t i;
    QVector3D p;
    source.load(i);
    _intersection_type = static_cast<intersection_type_t>(i);
    if (_owner->getFileVersion() >= fv191)
        source.load(_show_curvature);
    else
        _show_curvature = false;
    source.load(_plane);
    source.load(_build);
    size_t n;
    source.load(n);
    for (i=0; i<n; i++) {
        Spline* sp = new Spline();
        _items.add(sp);
        // read number of points for this spline
        size_t m;
        source.load(m);
        for (size_t j=0; j<m; j++) {
            float x, y, z;
            if (_owner->getFileVersion() >= fv160) {
                if (_intersection_type == fiStation) {
                    p.setX(-_plane.d());
                    source.load(y);
                    p.setY(y);
                    source.load(z);
                    p.setZ(z);
                } else if (_intersection_type == fiButtock) {
                    source.load(x);
                    p.setX(x);
                    p.setY(-_plane.d());
                    source.load(z);
                    p.setZ(z);
                } else if (_intersection_type == fiWaterline) {
                    source.load(x);
                    p.setX(x);
                    source.load(y);
                    p.setY(y);
                    p.setZ(-_plane.d());
                } else {
                    source.load(p);
                }
            } else {
                source.load(p);
            }
            sp->setPoint(j, p);
            bool b;
            source.load(b);
            sp->setKnuckle(j, b);
        }
    }
}

void Intersection::saveBinary(FileBuffer& dest)
{
    dest.add(static_cast<int>(_intersection_type));
    if (_owner->getFileVersion() >= fv191)
        dest.add(_show_curvature);
    dest.add(_plane);
    dest.add(_build);
    dest.add(_items.size());
    for (size_t i=0; i<_items.size(); i++) {
        Spline* sp = _items.get(i);
        dest.add(sp->numberOfPoints());
        for (size_t j=0; j<sp->numberOfPoints(); j++) {
            QVector3D p = sp->getPoint(j);
            if (_owner->getFileVersion() >= fv160) {
                switch (_intersection_type) {
                case fiStation:
                    dest.add(p.y());
                    dest.add(p.z());
                    dest.add(sp->isKnuckle(j));
                    break;
                case fiButtock:
                    dest.add(p.x());
                    dest.add(p.z());
                    dest.add(sp->isKnuckle(j));
                    break;
                case fiWaterline:
                    dest.add(p.x());
                    dest.add(p.y());
                    dest.add(sp->isKnuckle(j));
                    break;
                case fiDiagonal:
                    dest.add(p);
                    dest.add(sp->isKnuckle(j));
                    break;
                }
            } else {
                dest.add(p);
                dest.add(sp->isKnuckle(j));
            }
        }
    }
}

void Intersection::saveToDXF(QStringList& strings)
{
    // TODO
}

