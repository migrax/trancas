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

class NetworkException : public TrancasException {
public:
    NetworkException(const std::string& reason) :  TrancasException(reason) {}
};

class Network {
public:
    Network();

    void add(const Node& n) noexcept {
        nodes.insert(std::make_pair(n.getName(), n));
    }

    void add(Node&& n) noexcept {
        nodes.insert(std::move(std::make_pair(n.getName(), n)));
    }
    
    Node findNode(const std::string& nodeName) const {
        auto cnode = nodes.find(nodeName);
        
        if (cnode == nodes.end())
            throw NetworkException("Cannot find node " + nodeName + " in network.");
        
        return cnode->second;
    }
    
    void add(const Link& l) noexcept {
        links.insert(l);
    }
    
    void add(Link&& l) noexcept {
        links.insert(l);
    }
    
    Link findLink(const Node::NodePair& np) const {        
        auto clink = links.find(np);
        
        if (clink == links.end())
            throw NetworkException("Cannot find node between " + std::string(np.first) + " and " + std::string(np.second) + '.');
                
        return *clink;
    }

private:
    std::map<std::string, Node> nodes;
    std::set<Link> links;
};

#endif	/* NETWORK_H */

