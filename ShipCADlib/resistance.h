/*##############################################################################################
 *    ShipCAD																				   *
 *    Copyright 2015, by Greg Green <ggreen@bit-builder.com>								   *
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

#ifndef RESISTANCE_H
#define RESISTANCE_H

namespace ShipCAD {

//////////////////////////////////////////////////////////////////////////////////////

    
struct DelftSeriesResistance
{
    float start_speed;
    float end_speed;
    float step_speed;
    float bwl;
    float cp;
    float displacement;
    float draft;
    float draft_total;
    float keel_chord_length;
    float keel_area;
    float lcb;
    float lwl;
    float rudder_chord_length;
    float rudder_area;
    float viscosity;
    float wetted_surface;
    float wl_area;
    bool estimate_wet_surf;
    bool extract;
    // The original Pascal code apparently
    // wrote 4 byte aligned structures
    // put in some padding here to match
    // so that loadBinary/saveBinary works
    quint8 padding[2];
}
#ifdef __GNUG__
__attribute__ ((aligned (4)));
#endif
;

struct KAPERResistance
{
    float draft;
    float lwl;
    float bwl;
    float cp;
    float displacement;
    float lcb;
    float wetted_surface;
    float at_ax;
    float entrance_angle;
    bool extract;
    // The original Pascal code apparently
    // wrote 4 byte aligned structures
    // put in some padding here to match
    // so that loadBinary/saveBinary works
    quint8 padding[3];
}
#ifdef __GNUG__
__attribute__ ((aligned (4)));
#endif
;

//////////////////////////////////////////////////////////////////////////////////////

};				/* end namespace */
#endif // RESISTANCE_H

