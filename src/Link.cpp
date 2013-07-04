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

#include "Link.h"
#include "Node.h"

#include <cmath>
#include <algorithm>

using namespace std;

namespace {

    template<typename A, typename R>
    class Power : unary_function<A, R> {
    public:

        Power(A lambda) noexcept : cost(0), lambda(lambda), current(log(lambda)), first(false) {
        }

        R getCost() const noexcept {
            return max(static_cast<R> (0), cost);
        }

        void operator() (R coef) noexcept {
            current = max(static_cast<R> (0), current); // Cost is always possitive

            cost += coef * current;

            if (!first) {
                first = true;
                current = 1;
            } else {
                current *= lambda;
            }
        }
    private:
        R cost;
        const A lambda;
        R current;
        bool first;
    };

    template<typename D, class C>
    static D getCostForTraffic(D lambda, const C& coefs) noexcept {
        typedef typename C::value_type vtype;

        Power<vtype, vtype> p = for_each(coefs.begin(), coefs.end(), Power<vtype, vtype>(lambda));

        return p.getCost();
    }
}

Link Link::getNewReversedLink(const Link &orig) noexcept {
    return Link(orig.nodes.second, orig.nodes.first, orig.shared->coefs, orig.max_traffic);
}

double Link::getCost(double lambda) const noexcept {
    double new_cost, current_cost;

    if (lambda + shared->current_traffic > max_traffic) {
        return numeric_limits<double>::infinity();
    }
    
    new_cost = getCostForTraffic(lambda + shared->current_traffic, shared->coefs);
    current_cost = getCurrentCost();

    return new_cost - current_cost;
}

double Link::updateCurrentPower() const noexcept {
    double lambda = shared->current_traffic;

    shared->current_power = getCostForTraffic(lambda, shared->coefs);

    return shared->current_power;
}

double Link::getCurrentCost(double flow_traffic) const noexcept {

    struct restoreTraffic {

        restoreTraffic(const Link *l, double t) : link(const_cast<Link *> (l)), traffic(t) {
            if (t != 0.0)
                link->addTraffic(-traffic);
        }

        ~restoreTraffic() {
            link->addTraffic(traffic);
        }

    private:
        Link *link;
        const double traffic;
    } restore(this, flow_traffic);

    if (isCurrentPowerValid()) {
        return shared->current_power;
    }
    
    double currentPower = updateCurrentPower();
    
    if (flow_traffic > 0.0)
        return getCost(flow_traffic);
    else
        return currentPower;    
}

ostream& operator<<(ostream& output, const Link& l) {
    output << l.getOrig() << " → " << l.getDst();

    return output;
}
