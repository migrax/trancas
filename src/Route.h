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

#ifndef ROUTE_H
#define	ROUTE_H

#include "Node.h"
#include "TrancasException.h"

#include <vector>
#include <ostream>

class RouteException : public TrancasException {
public:
    RouteException(const std::string& reason) : TrancasException(reason) {
    }
};

class Route : public std::vector<Node> {
public:

    Route() noexcept : index(0) {
    }

    Route(const std::vector<Node>& nodes) noexcept : vector(nodes), index(0) {
    }

    Route(std::vector<Node>&& nodes) noexcept : vector(std::move(nodes)), index(0) {
    }
    
    Node getSrc() const noexcept { return front(); }
    Node getDst() const noexcept { return back(); }    

    Node getCurrent() const noexcept {
        return at(index);
    }        

    bool isFirst() const noexcept {
        return index == 0;
    }

    bool isLast() const noexcept {
        return index == size() - 1;
    }

    Node goNext() throw (RouteException) {
        if (isLast())
            throw RouteException("Cannot go past node " + std::string(getCurrent()));

        index += 1;
        return getCurrent();
    }

    Node goPrevious() throw (RouteException) {
        if (isFirst())
            throw RouteException("Cannot go before node" + std::string(getCurrent()));

        index -= 1;
        return getCurrent();
    }
    
private:
    size_type index;    
};

std::ostream& operator<<(std::ostream& os, const Route& r);

#endif	/* ROUTE_H */

