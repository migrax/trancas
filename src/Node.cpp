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
#include "Simulation.h"

using namespace std;

Node::Node(const std::string& name) noexcept :
id(genId()), name(name), status(make_shared<InternalStatus>()) {
}

int Node::genId() noexcept {
    static int id_generator = 0;

    return ++id_generator;
}

void Node::add(const Node& neighbour) noexcept {
    auto cindex = status->neighboursIndex.find(neighbour);

    if (cindex == status->neighboursIndex.end()) {
        status->neighbours.push_back(Neighbour(neighbour));
        status->neighboursIndex[neighbour] = status->neighbours.size() - 1;
    } else {
        // Replace it!
        status->neighbours[cindex->second] = Neighbour(neighbour);
    }
}

double Node::addTraffic(const Node& neighbour, double traffic) throw (NodeException) {
    auto i = status->neighboursIndex.find(neighbour);

    if (i == status->neighboursIndex.end()) {
        throw NodeException("Node " + string(neighbour) + " is not a neighbour of " + getName());
    }

    return status->neighbours[i->second].addTraffic(traffic);
}

double Node::removeTraffic(const Node& neighbour, double traffic) throw (NodeException) {
    auto i = status->neighboursIndex.find(neighbour);

    if (i == status->neighboursIndex.end()) {
        throw NodeException("Node " + string(neighbour) + " is not a neighbour of " + getName());
    }

    return status->neighbours[i->second].removeTraffic(traffic);
}

void Node::cleanRoute(const NodePair& np) noexcept {
    // (FIXME: uniform or 1 for chosen?. 1 for chosen looks bad for convergency speed)
    double prob = 1.0 / (status->neighbours.size());

    for (Neighbour& neigh : status->neighbours) {
        neigh.setProb(np, prob);
    }
}

const Node& Node::getRandomNeighbour(const std::string& avoid) const noexcept {
    vector<Neighbour>::size_type index;

    do {
        index = Simulation::getRandom<vector<Neighbour>::size_type>(0, status->neighbours.size() - 1);
    } while (avoid == status->neighbours[index].getName());

    return status->neighbours[index].getNode();
}

const Node& Node::getGoodNeighbour(const NodePair& routeEnds, const std::string& avoid) const noexcept {
    vector<Neighbour>::size_type index;

    do {
        // FIXME: brute force search
        double acumul = 0.0;
        double prob = Simulation::getRandom();

        for (index = 0; index < status->neighbours.size(); index++) {
            acumul += getProb(routeEnds, index);
            if (prob <= acumul)
                break;
        }
        assert(index < status->neighbours.size());
    } while (avoid == status->neighbours[index].getName());

    return status->neighbours[index].getNode();
}

double Node::getProb(const NodePair& routeEnds, vector<Neighbour>::size_type index) const noexcept {
    double prob = status->neighbours[index].getProb(routeEnds);

    if (prob < 0)
        const_cast<Node *> (this)->cleanRoute(routeEnds); // Just uninitialize stuff

    return status->neighbours[index].getProb(routeEnds);
}

void Node::updateStats(const Route& r, double cost) noexcept {
    const pair<const Node&, const Node&> np(r.front(), r.back());

    status->trips[np].addSample(cost);
}

Node Node::calcNextHop(const Route& r, const Node& prev) noexcept {
    static const Node dummy("Dummy node in calcNextHop");
    const pair<const Node&, const Node&> np(r.front(), r.back());
    const double rp = status->trips[np].getCorrectedRPrime();
    Node nextHop = dummy;
    double higherProb = 0.0;

    for (Neighbour& n : status->neighbours) {
        double prob = n.getProb(np);

        if (n.getNode() == prev) {
            prob += (1 - rp) * (1 - prob);
        } else {
            prob += -(1 - rp) * prob;
        }

        n.setProb(np, prob);
        if (prob > higherProb) {
            higherProb = prob;
            nextHop = n.getNode();
        }

        assert(0 <= prob && prob <= 1);
    }

    return nextHop;
}

Node::Neighbour::Neighbour(const Node& node) noexcept : node(make_shared<Node>(node)), status(std::make_shared<_status> ()) {
}

double Node::Neighbour::getProb(const NodePair& routeEnds) const noexcept {
    auto ci = status->routeProbs.find(routeEnds);

    if (ci == status->routeProbs.end())
        return -1; // Signal that we don't know about that route

    return ci->second;
}

double Node::Neighbour::setProb(const NodePair& routeEnds, double prob) throw (NodeException) {
    if (prob < 0.0 || prob > 1.0)
        throw NodeException("Probability cannot be outside [0, 1] range.");

    status->routeProbs[routeEnds] = prob;

    return prob;
}

ostream& operator<<(ostream& output, const Node& n) {
    output << n.getName();

    return output;
}