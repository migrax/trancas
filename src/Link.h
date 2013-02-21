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

#ifndef LINK_H
#define	LINK_H

#include <utility>
#include <vector>
#include <ostream>
#include <memory>

#include "Node.h"

class Link {
public:

    Link(const Node::NodePair& np) noexcept : nodes(np),
    shared(nullptr) {
    }

    Link(const Node& orig, const Node& dst) noexcept : Link(Node::NodePair(orig, dst)) {
    }

    Link(const Node& orig, const Node& dst,
            const std::vector<double>& coeficients,
            double max_traffic = 0) noexcept : nodes(Node::NodePair(orig, dst)),
    max_traffic(max_traffic),
    shared(std::make_shared<_shared>(coeficients)) {
    }

    Link(const Node& orig, const Node& dst,
            std::vector<double>&& coeficients,
            double max_traffic = 0) noexcept : nodes(Node::NodePair(orig, dst)),
    max_traffic(max_traffic),
    shared(std::make_shared<_shared>(std::move(coeficients))) {
    }

    static Link getNewReversedLink(const Link& orig) noexcept;
    
    Node getOrig() const noexcept {
        return nodes.first;
    }

    Node getDst() const noexcept {
        return nodes.second;
    }

    Node::NodePair getNodes() const noexcept {
        return nodes;
    }

    void setPower(const std::vector<double>& coeficients) noexcept {
        if (shared == nullptr)
            shared = std::make_shared<_shared>();

        shared->coefs = coeficients;
    }

    void setPower(std::vector<double>&& coeficients) noexcept {
        if (shared == nullptr)
            shared = std::make_shared<_shared>();

        shared->coefs = std::move(coeficients);
    }

    double getCost(double lambda) const noexcept;

    double addTraffic(double lambda) {
        invalidateCurrentPower();

        return shared->current_traffic += lambda;
    }

    double removeTraffic(double lambda) noexcept {
        invalidateCurrentPower();

        return shared->current_traffic -= lambda;
    }

    double getCurrentTraffic() const noexcept {
        return shared->current_traffic;
    }

    double getCurrentPower() const noexcept {
        if (isCurrentPowerValid()) {
            return shared->current_power;
        }

        return updateCurrentPower();
    }

    bool operator<(const Link& b) const noexcept {
        return nodes < b.nodes;
    }

private:
    Node::NodePair nodes;

    Node::NodePair normalizedNodePair(const Node::NodePair& nodes) const noexcept {
        if (nodes.first < nodes.second)
            return nodes;
        else
            return Node::NodePair(nodes.second, nodes.first);
    }

    double max_traffic;

    struct _shared {

        _shared() : coefs() {
        }

        _shared(const std::vector<double>& coefs) : coefs(coefs) {
        }

        _shared(std::vector<double>&& coefs) : coefs(std::move(coefs)) {
        }

        std::vector<double> coefs;

        // Status
        double current_traffic = 0;
        mutable double current_power = 0;
    };

    std::shared_ptr<_shared> shared;

    double updateCurrentPower() const noexcept;

    void invalidateCurrentPower() noexcept {
        shared->current_power = -1;
    }

    bool isCurrentPowerValid() const noexcept {
        return shared->current_power >= 0;
    }
};

std::ostream& operator<<(std::ostream& output, const Link& l);

#endif	/* LINK_H */

