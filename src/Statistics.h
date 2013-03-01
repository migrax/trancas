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
#include <algorithm>
#include <functional>

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

    double Deviation() const noexcept {
        return sqrt(Variance());
    }

    double addSample(double sample) noexcept {
        lastSample = sample;

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

    double getRPrime() const noexcept {
        return std::min(1.0, lastSample / (c * Mean()));
    }

    double getCorrectedRPrime() const noexcept {
        double rp = getRPrime();
        const double dOm = Deviation() / Mean();
        std::function<double (void) > f;
        double rmin = 0.0, rmax = 1.0;

        if (dOm < epsilon) {
            f = std::bind(&Statistics::S, this);
        } else {
            f = std::bind(&Statistics::U, this);
        }

        if (rp < threshold) {
            rp += f();
            rmax = 0.5;
        } else {
            rp -= f();
            rmin = 0.5;
        }
                
        rp = bound(rp, rmin, rmax);
        
        return pow(rp, h);
    }

private:
    double mean = std::numeric_limits<double>::signaling_NaN();
    double var = std::numeric_limits<double>::infinity();
    bool init = false;
    double lastSample = std::numeric_limits<double>::signaling_NaN();

    double S() const noexcept {
        return -exp(-a * Deviation() / Mean());
    }

    double U() const noexcept {
        return 1 - exp(-aPrime * Deviation() / Mean());
    }

    template <typename T>
    static T bound(T val, T min, T max) {
        return std::min(std::max(min, val), max);
    }

    static constexpr double gain = 0.1;
    static constexpr double c = 2.0;
    static constexpr double threshold = 0.5;
    static const double a;
    static const double aPrime;
    static constexpr double epsilon = 0.25;
    static constexpr double h = 0.04;
};

#endif	/* STATISTICS_H */

