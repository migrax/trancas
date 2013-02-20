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
    Network() {};

    void add(const Node& n) noexcept;
    
    Node getNode(const std::string& nodeName) const throw(NetworkException);
    
    void add(const Link& l) noexcept;
    
    Link getLink(const Node::NodePair& np) const throw(NetworkException);
    
    const std::set<Node>& getNeighbours(const Node& orig) const throw(NetworkException) {
        auto ci = nodeEdges.find(orig);
        
        if (ci == nodeEdges.end()) {
            throw NetworkException("Node " + orig.getName() + " has no links");
        }
        
        return ci->second;
    }    
    
    Route addTraffic(Node orig, const Node& dst, double traffic) const throw(TrancasException);
    Route sendAnt(Node orig, const Node& dst) const throw(TrancasException);
    // addtraffic
    // removetraffic
    // sendant
private:
    std::map<std::string, Node> nodes;
    std::set<Link> links;
    std::map<std::string, std::set<Node> > nodeEdges;
};

#endif	/* NETWORK_H */

