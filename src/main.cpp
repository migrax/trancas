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

#include "Simulation.h"
#include "Network.h"
#include "Route.h"

#include <cmath>

#include <iostream>
#include <exception>

using namespace std;

int main(int argc, char **argv) {
    constexpr double log_e_10 = log(10);

    Simulation::setRandomSeed(1); // Always the same value for debugging

    try {
        Network net;
        Node a("A"), b("B"), c("C"), d("D"), e("E"), f("F");
        net.add(a);
        net.add(b);
        net.add(c);
        net.add(d);
        net.add(e);
        net.add(f);

        net.add(Link(a, b,{11. / log_e_10})); // 11/log_e(10)
        net.add(Link(a, c,{0, 0, 1}));
        net.add(Link(b, d,{0, 0, 1}));
        net.add(Link(c, d,{0, 0, 1}));
        net.add(Link(d, e,{0, 0, 1}));
        net.add(Link(d, f,{0, 0, 1}));

        cout << net.addTraffic(a, e, 10) << endl;
        cout << net.addTraffic(a, f, 10) << endl;

        for (int i = 1; i <= 500; i++) {
            cout << i << ": " << net.sendAnt(a, e) << endl;
            cout << i << ": " << net.sendAnt(a, f) << endl;
        }        

        for (int i = 501; i <= 1000; i++) {
            cout << i << ": " << net.sendAnt(a, e) << endl;
            cout << i << ": " << net.sendAnt(a, f) << endl;
        }
        
        cout << endl <<  "Summary:" << endl;
        cout << net.getRoute(a, e) << endl;
        cout << net.getRoute(a, f) << endl;
        cout << "Total cost: " << net.getTotalCost() << endl;
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }

    return 0;
}

