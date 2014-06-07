/*#############################################################################################}
  {    This code is distributed as part of the FREE!ship project. FREE!ship is an               }
  {    open source surface-modelling program based on subdivision surfaces and intended for     }
  {    designing ships.                                                                         }
  {                                                                                             }
  {    Copyright © 2005, by Martijn van Engeland                                                }
  {    e-mail                  : Info@FREEship.org                                              }
  {    FREE!ship project page  : https://sourceforge.net/projects/freeship                      }
  {    FREE!ship homepage      : www.FREEship.org                                               }
  {                                                                                             }
  {    This program is free software; you can redistribute it and/or modify it under            }
  {    the terms of the GNU General Public License as published by the                          }
  {    Free Software Foundation; either version 2 of the License, or (at your option)           }
  {    any later version.                                                                       }
  {                                                                                             }
  {    This program is distributed in the hope that it will be useful, but WITHOUT ANY          }
  {    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A          }
  {    PARTICULAR PURPOSE. See the GNU General Public License for more details.                 }
  {                                                                                             }
  {    You should have received a copy of the GNU General Public License along with             }
  {    this program; if not, write to the Free Software Foundation, Inc.,                       }
  {    59 Temple Place, Suite 330, Boston, MA 02111-1307 USA                                    }
  {                                                                                             }
  {#############################################################################################*/

#ifndef SUBDIVCONTROLCURVE_H_
#define SUBDIVCONTROLCURVE_H_

#include <iosfwd>
#include <QObject>
#include "subdivbase.h"

namespace ShipCADGeometry {

//////////////////////////////////////////////////////////////////////////////////////

class SubdivisionPoint;
class SubdivisionControlEdge;

class SubdivisionControlCurve : public SubdivisionBase
{
    Q_OBJECT
    //Q_PROPERTY(SubdivisionSurface* Owner READ getOwner)

public:

    explicit SubdivisionControlCurve(SubdivisionSurface* owner);
    virtual ~SubdivisionControlCurve();

    // modifications
    void replaceVertexPoint(SubdivisionPoint* oldpt, SubdivisionPoint* newpt);
    void insertEdgePoint(SubdivisionPoint* p1, SubdivisionPoint* p2, SubdivisionPoint* newpt);
    void deleteEdge(SubdivisionControlEdge* edge);
    void insertControlPoint(SubdivisionPoint* p1, SubdivisionPoint* p2, SubdivisionPoint* newpt);

    // getters/setters
    bool isSelected();

    // output
    void dump(std::ostream& os) const;

protected:
};

//////////////////////////////////////////////////////////////////////////////////////

};				/* end namespace */

std::ostream& operator << (std::ostream& os, const ShipCADGeometry::SubdivisionControlCurve& curve);

#endif

