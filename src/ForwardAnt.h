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

#ifndef FORWARDANT_H
#define	FORWARDANT_H

#include "Ant.h"
#include "Route.h"

#include <set>

class ForwardAnt : public Ant {
public:

    ForwardAnt(const Network& graph, const Route& route, double traffic) noexcept :
    Ant(graph, route, traffic),    
    currentNode(route.front()) {
        linkCosts.push(std::make_pair(currentNode, 0));
    };

    virtual Node advance();
    void dump() noexcept;

private:
    // Internal state    
    std::set<Node> visitedNodes;
    Node currentNode;

    const Node& chooseNextNode() const noexcept;
    const Node& chooseRandomNeighbour() const noexcept;
    const Node& chooseGoodNeighbour() const noexcept;
    void pruneStack(const Node& rNode) noexcept;
    // Forward ants move from left to right
    double getCost(const Node& left, const Node& right) const noexcept;       
};

#endif	/* FORWARDANT_H */

