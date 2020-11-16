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

#ifndef NETWORK_H
#define	NETWORK_H

#include <set>
#include <map>
#include <ostream>

#include "Node.h"
#include "Link.h"
#include "TrancasException.h"

class Route;

class NetworkException : public TrancasException {
public:
    NetworkException(const std::string& reason) :  TrancasException(reason) {}
};

class Network {
public:
    typedef std::pair<Route, double> RouteInfo;
    enum RouteMode {Optimal, SPF};
    
    Network() {};

    void add(const Node& n) noexcept;
    
    Node getNode(const std::string& nodeName) const;
    
    void add(const Link& l) noexcept;
    
    Link getLink(const Node::NodePair& np) const;
    
    const std::set<Node>& getNeighbours(const Node& orig) const {
        auto ci = nodeEdges.find(orig);
        
        if (ci == nodeEdges.end()) {
            throw NetworkException("Node " + orig.getName() + " has no links");
        }
        
        return ci->second;
    }    
    
    void addNewRoute(const Route& r, double traffic);    
    
    RouteInfo addTraffic(Node orig, const Node& dst, double traffic, RouteMode mode = Optimal);
    RouteInfo sendAnt(Node orig, const Node& dst, bool *changed = nullptr);

    RouteInfo getRoute(const Node &orig, const Node &dst) const;
    RouteInfo getRoute(const Node::NodePair &np) const;

    RouteInfo addTrafficToRoute(const Node &orig, const Node &dst, double traffic);
    RouteInfo removeTrafficFromRoute(const Node &orig, const Node &dst, double traffic);

    std::set<std::pair<Node, Node> > copyRoutes() const noexcept;
    
    double getTotalCost() const noexcept;
    double getTotalTraffic() const noexcept;
    double getAvLength() const noexcept;
private:
    std::map<std::string, Node> nodes;
    std::set<Link> links;
    std::map<std::string, std::set<Node> > nodeEdges;
    std::map<std::pair<std::string, std::string>, RouteInfo> routes;

    RouteInfo updateRoute(Route &newRoute);
    RouteInfo changeTrafficInRoute(const Node &orig, const Node &dst, double traffic);
    bool testRoute(const Route &r) const noexcept;
};

std::ostream& operator<<(std::ostream& os, const Network::RouteInfo& i);

#endif	/* NETWORK_H */

