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
#include <utility>

using namespace std;

void BackwardAnt::prepareRoute() throw (AntException) {
    if (!linkCosts.empty()) {
        throw AntException("Cannot give complete route until I reach " + string(route.getSrc()));
    }    
    
    // Origin route has yet to be added to the route
    newRoute.push_back(route.getSrc());
    
    if (!prepared) {        
        reverse(newRoute.begin(), newRoute.end());
        prepared = true;
    }
    
    assert(route.getDst() == newRoute.back());
}

Route BackwardAnt::getRoute() throw (AntException) {
    prepareRoute();
    
    return newRoute;
}

Node BackwardAnt::advance() throw(AntException) {    
    linkInfo& lc = linkCosts.top();
    Node& current = lc.first;    
    routeCost += lc.second;
        
    // Update probabilities and get current nexto hop for route /route/
    if (!reversedNodes.empty()) {
        const Node& prevNode = reversedNodes.back().first;
        
        current.updateStats(route, routeCost);
        newRoute.push_back(current.calcNextHop(route, prevNode));
    }
    
    // Store the info about the current node for the next one
    reversedNodes.push_back(move(lc));
    /* Algorithm:
     * a) Drop prev. node from stack and add it to a vector of visited nodes
     * b) Update trip cost from current node to route destination
     * c) Update probabilities
     * d) Modify routes?. Save if to tell the source
     * e) At the src, update the route
     */   
    
    linkCosts.pop();
    
    // Go to parent node
    return lc.first;
}
