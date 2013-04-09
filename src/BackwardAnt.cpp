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

BackwardAnt::BackwardAnt(ForwardAnt&& ant) throw (AntException) : Ant(ant.graph, move(ant.route), ant.traffic) {
    linkCosts = move(ant.linkCosts);

    if (linkCosts.top().first != route.back()) {
        throw AntException("Backward ants must start at the end of the route");
    }

    extraCost = 0.0; 
    for (auto ci = route.begin(); ci < route.end() - 1; ci++) {
        Node::NodePair np = make_pair(*ci, *(ci + 1));
        double extra = calcExtraCost(np); // This is the cost the other traffic in the link has to assume if this flow leaves it
        unvisitedLinks[np] = extra;      
        extraCost += extra;
    }    
}

/* Get the cost the other flows suffer if we abandon this link */
double BackwardAnt::calcExtraCost(const Node::NodePair& np) const throw (NetworkException) {
    Link l = graph.getLink(np);
    
    double curTraffic = l.getCurrentTraffic();
    
    l.removeTraffic(curTraffic -  traffic); // Leave only our flow
    double curCostRest = l.getCost(curTraffic - traffic); // Marginal cost for the rest while our flow is the the link
    l.removeTraffic(traffic);
    double newCostRest = l.getCost(curTraffic - traffic); // New cost for the rest if they are alone in the link
    
    l.addTraffic(curTraffic); // Restore traffic;
    
    
    return max(0.0, newCostRest - curCostRest);
}

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
     * We know it is not the first element (orig != dst) (Micro optimization)
     * And we don't care if we do not find it, 'cause the erase from ele+1 onwards (Correcteness)
     */

    if (route.getDst() != newRoute.back()) {
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

Node BackwardAnt::advance() throw (TrancasException) {
    linkInfo& lc = linkCosts.top();
    Node current = lc.first;

    // Update probabilities and get current next hop for route /route/
    if (!reversedNodes.empty()) {
        const Node& prevNode = reversedNodes.back().first;
        map<Node::NodePair, double>::iterator ui;

        if ((ui = unvisitedLinks.find(make_pair(prevNode, current))) != unvisitedLinks.end()) {
            extraCost -= ui->second; // FIXME
            unvisitedLinks.erase(ui);            
        }
        
        current.updateStats(route, routeCost + extraCost);
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
