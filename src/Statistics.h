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

#ifndef STATISTICS_H
#define	STATISTICS_H

#include <cmath>
#include <limits>

/* We employ a exponentially moving average to adapt
 * to changes in the traffic matrix
 */
class Statistics {
public:

    Statistics() {
    }

    double Mean() const noexcept {
        return mean;
    }

    double Variance() const noexcept {
        return var;
    }

    double addSample(double sample) noexcept {
        if (init) {
            const double err = sample - mean;

            mean += gain*err;
            var += gain * (fabs(err) - var);
        } else {
            init = true;
            
            mean = sample;
            var = mean * mean; // Bad enough (but not too bad) initial value?
        }

        return sample;
    }

    double operator()(double sample) noexcept {
        return addSample(sample);
    }

private:
    double mean = std::numeric_limits<double>::quiet_NaN();
    double var = std::numeric_limits<double>::infinity();
    bool init = false;

    static constexpr double gain = 0.1;
};

#endif	/* STATISTICS_H */

