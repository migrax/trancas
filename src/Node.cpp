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

#include "Node.h"
#include "Ant.h"

using namespace std;

Node::Node(const std::string& name) noexcept :
id(genId()), name(name), status(new InternalStatus) {
}

int Node::genId() noexcept {
    static int id_generator = 0;

    return ++id_generator;
}

Node::Neighbour::Neighbour() noexcept : currentTraffic(std::make_shared<double> (0)) {
}

double Node::Neighbour::getProb(const NodePair& routeEnds) const noexcept {
    auto ci = routeProbs.find(routeEnds);
    
    if (ci == routeProbs.end())
        return 0;
    
    return ci->second;
}

double Node::Neighbour::setProb(const NodePair& routeEnds, double prob) throw (NodeException) {
    if (prob < 0 || prob > 1)
        throw NodeException("Probability cannot be outside [0, 1] range.");
    
    routeProbs[routeEnds] = prob;
    
    return prob;
}

ostream& operator<<(ostream& output, const Node& l) {    
    output << l;

    return output;
}