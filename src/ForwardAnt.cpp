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

#include "ForwardAnt.h"
#include "Simulation.h"
#include "Network.h"
#include "Link.h"

#include <iostream>

using namespace std;

const Node& ForwardAnt::chooseRandomNeighbour() const noexcept {
    if (currentNode != route.getSrc())
        return currentNode.getRandomNeighbour(linkCosts.top().first);
    else
        return currentNode.getRandomNeighbour();
}

const Node& ForwardAnt::chooseGoodNeighbour() const noexcept {
    Node::NodePair routeEnds = make_pair(route.front(), route.back());

    if (currentNode != route.getSrc())
        return currentNode.getGoodNeighbour(routeEnds, linkCosts.top().first);
    else
        return currentNode.getGoodNeighbour(routeEnds);
}

const Node& ForwardAnt::chooseNextNode() const noexcept {
    if (Simulation::getRandom() < Simulation::explorationProb)
        return chooseRandomNeighbour();
    else
        return chooseGoodNeighbour();
}

void ForwardAnt::pruneStack(const Node& rNode) noexcept {
    // cerr << "Pruning until node " << rNode << " size: " << linkCosts.size() << ": ";

    do {
        visitedNodes.erase(linkCosts.top().first);
        // cerr << " Pop!(" << linkCosts.top().first << ") ";
        linkCosts.pop();
    } while (linkCosts.top().first != rNode);    
}

double ForwardAnt::getCost(const Node& left, const Node& right) const noexcept {
    const Node::NodePair np(left, right);
    const Link& l = graph.getLink(np);
    double cost;

    if (route.isEdge(np)) {
        cost = l.getCurrentCost(traffic);
    } else {
        cost = l.getCost(traffic);
    }

    cerr << " cost: " << cost << ' ';
    return cost;
}

Node ForwardAnt::advance() throw (AntException) {
    cerr << "At " << currentNode << ": ";

    // If we are in an already visited node. Pop information about it from the stack
    if (visitedNodes.find(currentNode) != visitedNodes.end()) {
        pruneStack(currentNode);
    }

    visitedNodes.insert(currentNode);

    Node next = chooseNextNode();
    cerr << " ¿" << next << "? ";
    assert(currentNode != next);

    // Force random if the chosen node has already been visited
    if (visitedNodes.find(currentNode) != visitedNodes.end())
        next = chooseRandomNeighbour();
    assert(currentNode != next);

    linkCosts.push(make_pair(next, getCost(currentNode, next)));

    cerr << " going to " << next << endl;

    return currentNode = next;
}

void ForwardAnt::dump() noexcept {
    stack<pair<Node, double> > rcost;

    while (!linkCosts.empty()) {
        rcost.push(linkCosts.top());
        linkCosts.pop();
    }

    while (!rcost.empty()) {
        cerr << rcost.top().second << " → " << rcost.top().first << ' ';
        linkCosts.push(rcost.top());
        rcost.pop();
    }

    cerr << endl;
}