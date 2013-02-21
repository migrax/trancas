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

#include "config.h"

#include "Network.h"
#include "Route.h"
#include "Dijkstra.h"
#include "ForwardAnt.h"
#include "BackwardAnt.h"

using namespace std;

void Network::add(const Node& n) noexcept {
    nodes.insert(std::make_pair(n.getName(), n));
}

Node Network::getNode(const std::string& nodeName) const throw (NetworkException) {
    auto cnode = nodes.find(nodeName);

    if (cnode == nodes.end())
        throw NetworkException("Cannot find node " + nodeName + " in network.");

    return cnode->second;
}

void Network::add(const Link& l) noexcept {
    links.insert(l);
    links.insert(Link::getNewReversedLink(l));

    // Make both nodes known to each other
    l.getOrig().add(l.getDst());
    l.getDst().add(l.getOrig());
    
    nodeEdges[l.getOrig()].insert(l.getDst());
    nodeEdges[l.getDst()].insert(l.getOrig());
}

void Network::addRoute(const Route& r, double traffic) throw (TrancasException) {
    const Node& orig = r.front();
    const Node& dst = r.back();
    pair<string, string> np = make_pair(orig, dst);
    auto ci = routes.find(np);
    
    if (ci != routes.end()) {
        throw RouteException("There is already a route from " + orig.getName() + " to " + dst.getName());
    }    
    
    routes[np] = make_pair(r, traffic);
}

Link Network::getLink(const Node::NodePair& np) const throw (NetworkException) {
    auto clink = links.find(np);

    if (clink == links.end())
        throw NetworkException("Cannot find node between " + std::string(np.first) + " and " + std::string(np.second) + '.');

    return *clink;
}

Route Network::addTraffic(Node orig, const Node& dst, double traffic) throw(TrancasException) {
    /* Algorithm:
     * a) Calculate route (link collection)
     * b) Update info at orig node
     * c) Update link traffic
     * d) Update Nodes neighbour traffic
     */
    Dijkstra spf(orig, dst, *this, traffic);
    
    Route r = spf.getRoute();
    for (auto i = r.begin(); i < r.end() - 1; i++) {
        Link l = getLink(make_pair(*i, *(i+1)));
        l.addTraffic(traffic);
        
        i->addTraffic(*(i+1), traffic);
    }
    addRoute(r, traffic);
    
    return r;
}

Route Network::sendAnt(Node orig, const Node& dst) const throw(TrancasException) {
    auto ci = routes.find(make_pair(orig, dst));
    if (ci == routes.end())
        throw RouteException("No such route from " + string(orig) + " to " + string(dst) + '.');
    
    ForwardAnt trancas(ci->second.first, ci->second.second);
    
    while(dst != trancas.advance());
    BackwardAnt barrancas(move(trancas));
    while(orig != barrancas.advance());
    
    return barrancas.getRoute().first;
}