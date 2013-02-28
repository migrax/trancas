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

void Network::addNewRoute(const Route& r, double traffic) throw (TrancasException) {
    const Node& orig = r.front();
    const Node& dst = r.back();
    pair<string, string> np(orig, dst);
    auto ci = routes.find(np);

    if (ci != routes.end()) {
        throw RouteException("There is already a route from " + orig.getName() + " to " + dst.getName());
    }

    routes[np] = make_pair(r, traffic);

    // Tell nodes to adjust neighbour probabilities
    for (Node n : r) {
        if (n != r.getDst())
            n.cleanRoute(make_pair(getNode(orig), getNode(dst)));
    }
}

Link Network::getLink(const Node::NodePair& np) const throw (NetworkException) {
    auto clink = links.find(np);

    if (clink == links.end())
        throw NetworkException("Cannot find node between " + std::string(np.first) + " and " + std::string(np.second) + '.');

    return *clink;
}

Network::RouteInfo Network::addTraffic(Node orig, const Node& dst, double traffic) throw (TrancasException) {
    /* Algorithm:
     * a) Calculate route (link collection)
     * b) Update info at orig node
     * c) Update link traffic
     * d) Update Nodes neighbour traffic
     */
    Dijkstra spf(orig, dst, *this, traffic);

    Route r = spf.getRoute();
    double cost = 0.0;
    for (auto i = r.begin(); i < r.end() - 1; i++) {
        Link l = getLink(make_pair(*i, *(i + 1)));
        cost += l.getCost(traffic);
        l.addTraffic(traffic);

        i->addTraffic(*(i + 1), traffic);
    }
    addNewRoute(r, traffic);

    return RouteInfo(move(r), cost);
}

Network::RouteInfo Network::sendAnt(Node orig, const Node& dst) throw (TrancasException) {
    auto ci = routes.find(make_pair(orig, dst));
    if (ci == routes.end())
        throw RouteException("No such route from " + string(orig) + " to " + string(dst) + '.');

    RouteInfo rInfo(ci->second); // Route, traffic
    ForwardAnt trancas(*this, rInfo.first, rInfo.second);

    while (dst != trancas.advance());
    trancas.dump();
    BackwardAnt barrancas(move(trancas));
    while (orig != barrancas.advance());

    Route newRoute = barrancas.getRoute();
    if (newRoute != rInfo.first) { // Route has changed        
        rInfo = updateRoute(newRoute); // Route, cost
    }

    return rInfo;
}

Network::RouteInfo Network::updateRoute(Route& newRoute) noexcept {
    std::pair<const Node&, const Node&> np(newRoute.front(), newRoute.back());
    auto ori = routes.find(np);
    assert(ori != routes.end());
    Route& oldRoute = ori->second.first;
    const double traffic = ori->second.second;

    // Remove the traffic from the old route
    for (auto i = oldRoute.begin(); i < oldRoute.end() - 1; i++) {
        Link l = getLink(make_pair(*i, *(i + 1)));
        l.removeTraffic(traffic);

        i->removeTraffic(*(i + 1), traffic);
    }

    // Add it to the new route
    double cost = 0.0;
    for (auto i = newRoute.begin(); i < newRoute.end() - 1; i++) {
        Link l = getLink(make_pair(*i, *(i + 1)));
        l.addTraffic(traffic);

        i->addTraffic(*(i + 1), traffic);

        cost += l.getCurrentCost(traffic);
    }


    ori->second = RouteInfo(newRoute, traffic);

    return RouteInfo(newRoute, cost);
}

Network::RouteInfo Network::getRoute(const Node& orig, const Node& dst) const throw (NetworkException) {
    auto ci = routes.find(make_pair(orig, dst));
    if (ci == routes.end())
        throw RouteException("No such route from " + string(orig) + " to " + string(dst) + '.');

    const Route& route = ci->second.first;
    const double traffic = ci->second.second;
    double cost = 0.0;

    for (auto i = route.begin(); i < route.end() - 1; i++) {
        Link l = getLink(make_pair(*i, *(i + 1)));

        cost += l.getCurrentCost(traffic);
    }

    return RouteInfo(route, cost);
}

Network::RouteInfo Network::getRoute(const Node::NodePair& np) const throw (NetworkException) {
    return getRoute(np.first, np.second);
}

Network::RouteInfo Network::addTrafficToRoute(const Node& orig, const Node& dst, double traffic) throw (NetworkException) {
    if (traffic < 0.0) {
        throw NetworkException("Can only add positive values of traffic to a route");
    }

    return changeTrafficInRoute(orig, dst, traffic);
}

Network::RouteInfo Network::removeTrafficFromRoute(const Node& orig, const Node& dst, double traffic) throw (NetworkException) {
    if (traffic < 0.0) {
        throw NetworkException("Can only remove positive values of traffic from a route");
    }

    return changeTrafficInRoute(orig, dst, -traffic);
}

Network::RouteInfo Network::changeTrafficInRoute(const Node& orig, const Node& dst, double traffic) throw (NetworkException) {
    auto ci = routes.find(make_pair(orig, dst));
    if (ci == routes.end())
        return addTraffic(orig, dst, traffic);

    Route& route = ci->second.first;
    double& curTraffic = ci->second.second;

    if (curTraffic + traffic < 0.0) {
        throw NetworkException("Routes must have positive values from total traffic");
    }

    for (auto i = route.begin(); i < route.end() - 1; i++) {
        Link l = getLink(make_pair(*i, *(i + 1)));
        l.addTraffic(traffic);
        i->addTraffic(*(i + 1), traffic);
    }

    curTraffic += traffic;

    return ci->second;
}

double Network::getTotalCost() const noexcept {
    double cost = 0.0;

    for (const Link& l : links) {
        cost += l.getCurrentCost();
    }

    return cost;
}

double Network::getTotalTraffic() const noexcept {
    double traffic = 0.0;
    
    for (const pair<pair<string, string>, RouteInfo> ri : routes) {
        traffic += ri.second.second;
    }
    
    return traffic;
}

std::ostream& operator<<(std::ostream& os, const Network::RouteInfo& i) {
    return os << i.first << " (" << i.second << ')';
}