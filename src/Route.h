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

#ifndef ROUTE_H
#define	ROUTE_H

#include "Node.h"
#include "TrancasException.h"

#include <vector>
#include <set>
#include <ostream>

class RouteException : public TrancasException {
public:

    RouteException(const std::string& reason) : TrancasException(reason) {
    }
};

class Route : public std::vector<Node> {
public:

    Route() noexcept {
    }

    Route(const std::vector<Node>& nodes) noexcept : vector(nodes) {
    }

    Route(std::vector<Node>&& nodes) noexcept : vector(std::move(nodes)) {
    }

    const Node& getSrc() const noexcept {
        return front();
    }

    const Node& getDst() const noexcept {
        return back();
    }

    const Node& getCurrent() const noexcept {
        return (*this)[index];
    }

    bool isFirst() const noexcept {
        return index == 0;
    }

    bool isLast() const noexcept {
        return index == size() - 1;
    }

    bool isEdge(const Node::NodePair& edge) const noexcept {
        if (!edgesReady)
            populateEdges();

        return edges.find(edge) != edges.end();
    }

    const Node& getNext() const {
        if (isLast())
            throw RouteException("Cannot go past node " + std::string(getCurrent()));

        return (*this)[index + 1];
    }

    const Node& goNext() {
        const Node& next = getNext();

        index += 1;

        return next;
    }

    const Node& getPrevious() const {
        if (isFirst())
            throw RouteException("Cannot go before node" + std::string(getCurrent()));

        return (*this)[index - 1];
    }

    const Node& goPrevious() {
        const Node& prev = getPrevious();

        index -= 1;

        return prev;
    }

    const Node& rewind() {
        index = 0;

        return getCurrent();
    }
    
private:
    size_type index = 0;
    mutable std::set<Node::NodePair> edges;
    mutable bool edgesReady = false;
    
    void populateEdges() const noexcept;
};

std::ostream& operator<<(std::ostream& os, const Route& r);

#endif	/* ROUTE_H */

