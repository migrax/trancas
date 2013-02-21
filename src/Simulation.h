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

#include <random>
#include <limits>

#ifndef SIMULATION_H
#define	SIMULATION_H

class Simulation {
public:    
    static void setRandomSeed(std::mt19937_64::result_type seed) noexcept;
    static double getRandom(double a = 0.0, double b = 1.0) noexcept {
        std::uniform_real_distribution<double> dstr(a, b);
        
        return dstr(randomGenerator);
    }
    template<class IntType = int>
    static IntType getRandom(IntType a = 0, IntType b = std::numeric_limits<IntType>::max()) noexcept {
        std::uniform_int_distribution<IntType> dstr(a, b);
        
        return dstr(randomGenerator);
    }
    
    static constexpr double explorationProb = 0.002;
    static constexpr double goodnessProb = 1 - explorationProb;
private:
    static std::mt19937_64 randomGenerator;
};

#endif	/* SIMULATION_H */

