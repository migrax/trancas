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
    
    /* Sometime the route contains the destination node twice. This happens
     * when the forward ant followed a longer way than the current route, and the
     * backward ant records the optimal route, that reaches sooner to the destination.
     * Prune the superflous part.
     */
    newRoute.erase(find(newRoute.begin() + 1, newRoute.end() - 1, newRoute.back()) + 1, newRoute.end());
    /*
     * We now it is not the first element (orig != dst) (Micro optimization)
     * And we don't care if we do not find it, 'cause the erase from ele+1 onwards (Correcteness)
     */
    
    if(route.getDst() != newRoute.back()) {
        /* This happens when the last jump changes but the ant came
         * from the old route.
         * Just return the old route... it will eventually change when the ant
         * goes thru the new "good" route.
         */
        newRoute = route;
    }
}

Route BackwardAnt::getRoute() throw (AntException) {
    prepareRoute();
    
    return newRoute;
}

Node BackwardAnt::advance() throw(TrancasException) {    
    linkInfo& lc = linkCosts.top();
    Node current = lc.first;        
        
    // Update probabilities and get current nexto hop for route /route/
    if (!reversedNodes.empty()) {
        const Node& prevNode = reversedNodes.back().first;
        
        current.updateStats(route, routeCost);        
        newRoute.push_back(current.calcNextHop(route, prevNode));
    }
    // Add cost from previous node to the current one to the total route cost
    routeCost += lc.second;
    
    // Store the info about the current node for the next one
    reversedNodes.push_back(move(lc));   
    
    linkCosts.pop();
    
    // Return current node    
    return current;
}
