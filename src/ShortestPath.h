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

#ifndef SHORTESTPATH_H
#define	SHORTESTPATH_H

#include "Route.h"
#include "TrancasException.h"

class Network;
class Node;

class SPFException : public TrancasException {
public:
    SPFException(const std::string& reason) : TrancasException(reason) {
    }
};

class ShortestPath {
public:
    double getCost() const noexcept;

    Route getRoute() const noexcept {
        return nodes;
    }

protected:

    ShortestPath(const Node& src, const Node& dst,
            const Network& graph, double lambda) noexcept : src(src), dst(dst),
    graph(graph), lambda(lambda) {
    }

    const Node& src;
    const Node& dst;
    const Network& graph;
    const double lambda;

    Route nodes;
};

#endif	/* SHORTESTPATH_H */

