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
#include "Route.h"

using namespace std;

void Route::populateEdges() const noexcept {
    if (edgesReady)
        return;

    for (auto ci = begin(); ci < end(); ci++) {
        if (ci + 1 < end())
            edges.insert(Node::NodePair(*ci, *(ci + 1)));
    }

    edgesReady = true;
}

ostream& operator<<(ostream& os, const Route& r) {
    for (auto ci = r.begin(); ci < r.end(); ci++) {
        os << *ci;
        if (ci + 1 < r.end())
            os << " → ";
    }

    return os;
}

