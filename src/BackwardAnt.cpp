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
#include <cassert>

using namespace std;

void BackwardAnt::prepareRoute() throw (AntException) {
    if (!linkCosts.empty()) {
        throw AntException("Cannot give complete route until I reach " + string(route.getSrc()));
    }    
    
    if (!reversed) {
        reverse(newRoute.begin(), newRoute.end());
        reversed = true;
    }
    
    assert(route.getSrc() == newRoute.front());
}

Node::RouteInfo BackwardAnt::getRoute() throw (AntException) {
    prepareRoute();
    
    return make_pair(route, routeCost);
}

Node BackwardAnt::advance() throw(AntException) {
    // FIXME: Dummy implementation
    pair<Node, double> lc = linkCosts.top();
    routeCost += lc.second;
    newRoute.push_back(lc.first);
    
    linkCosts.pop();
    
    return lc.first;
}
