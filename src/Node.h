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

#ifndef NODE_H
#define	NODE_H

#include <string>
#include <map>
#include <memory>
#include <utility>
#include <cassert>

#include <boost/operators.hpp>

#include "TrancasException.h"

class Route;

class NodeException : public TrancasException {
public:
    NodeException(const std::string& reason) noexcept : TrancasException(reason) {}
};

class Node : public boost::less_than_comparable<Node>,
        public boost::equality_comparable<Node>{
public:
    Node(const std::string& name) noexcept;
    
    operator const std::string& () const noexcept {
        return getName();
    }
    const std::string& getName() const noexcept {
        return name;
    }
    
    bool operator==(const Node& b) const noexcept {
        return id == b.id;
    }
    
    bool operator<(const Node& b) const noexcept {
        return id < b.id;
    }
private:
    int id;
    std::string name;
    
    /*
     * Routes to every destination
     * Traffic for every neightbour. How? (Idea al nodo fuente "refresca" las rutas cuando llegan las hormigas de vuelta... en la práctica se haría con estimaciones de tráfico)
     * Links do not belong here. Another DB. 
     * 
     */
    class Neighbour {
    public:
        typedef std::pair<Node, Node> NodePair;
        
        Neighbour() noexcept;
        
        double addTraffic(double traffic) { return *currentTraffic += traffic; }
        double rmTraffic(double traffic) { assert((*currentTraffic -= traffic) >= 0); return *currentTraffic;}
        double getProb(const NodePair& routeEnds) const noexcept;
        double setProb(const NodePair& routeEnds, double prob) throw(NodeException);
    private:
        std::shared_ptr<double> currentTraffic;
        std::map<NodePair, double> routeProbs;
    };
    
    struct InternalStatus {
        std::map<std::string, Route> routes;
        std::map<std::string, Neighbour> neighbours;        
    };
    std::shared_ptr<InternalStatus> status;
    
    static int genId() noexcept;
};

#endif	/* NODE_H */

