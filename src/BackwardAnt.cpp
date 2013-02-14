/* Copyright (C) 2013 Miguel Rodríguez Pérez <miguel@det.uvigo.es>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 * Authors: Miguel Rodríguez Pérez <miguel@det.uvigo.es>
 *
 */

#include "BackwardAnt.h"

#include <algorithm>

using namespace std;

void BackwardAnt::prepareRoute() throw (AntException) {
    if (route.isFirst() == false) {
        throw AntException("Cannot give complete route until I reach " + string(route.getSrc()));
    }

    if (!reversed) {
        reverse(newRoute.begin(), newRoute.end());
        reversed = true;
    }
}

const Route& BackwardAnt::getRoute() throw (AntException) {
    prepareRoute();
    
    return route;
}
