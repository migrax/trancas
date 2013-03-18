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

#ifndef ANT_H
#define	ANT_H

#include "config.h"
#include "Node.h"
#include "Route.h"
#include "TrancasException.h"

#include <stack>
#include <utility>

class Network;

class AntException : public TrancasException {
public:
    AntException(const std::string& reason) noexcept : TrancasException(reason) {}
};

class BackwardAnt;

class Ant {
public:

    Ant(const Network& graph, const Route& route, double traffic) noexcept : graph(graph), route(route), traffic(traffic) {
    }
    Ant(const Network& graph, const Route&& route, double traffic) noexcept : graph(graph), route(std::move(route)), traffic(traffic) {
    }

    virtual Node advance() throw(TrancasException) = 0;
    
    friend class BackwardAnt;
protected:
    const Network& graph;
    typedef std::pair<Node, double> linkInfo;
    
    Route route;    
    double traffic;
    
    // Internal state
    std::stack<linkInfo> linkCosts;
};

#endif	/* ANT_H */

