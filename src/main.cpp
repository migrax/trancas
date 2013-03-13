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

#include <cstring>

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <exception>

using namespace std;

namespace {
    string conf_prefix;

    int usage(int argc, char** argv) {
        cerr << "Usage: " << basename(argv[0]) << " prefix" << endl;

        return -1;
    }

    void parse_args(int argc, char *argv[]) {
        conf_prefix = argv[1];

        return;
    }

    void parse_config(Network& net, string prefix) {
        ifstream topofile(prefix + ".topo");
        string line;
        map<string, Node> nodes;

        for (int linenumber = 1; getline(topofile, line); linenumber++) {
            string src_name, dst_name;
            vector<double> coefs;
            double capacity = 0, coef;
            istringstream lis(line);

            if (line.length() > 0 && line[0] == '#')
                continue;

            lis >> src_name >> dst_name;

            if (src_name.length() == 0 || dst_name.length() == 0) {
                cerr << "Falformed input on line " << linenumber << ": " << line << endl;
            }

            lis >> capacity;

            while (lis >> coef) {
                coefs.push_back(coef);
            }

            if (nodes.find(src_name) == nodes.end()) {
                nodes.insert(make_pair(src_name, Node(src_name)));
            }
            if (nodes.find(dst_name) == nodes.end()) {
                nodes.insert(make_pair(dst_name, Node(dst_name)));
            }

            net.add(nodes.find(src_name)->second);
            net.add(nodes.find(dst_name)->second);
            net.add(Link(nodes.find(src_name)->second, nodes.find(dst_name)->second, coefs, capacity));
        }
    }

    string extract_nodename_from_spec(std::string& node_spec) {
        istringstream is(node_spec);
        string name;

        is >> name;

        getline(is, node_spec);

        return name;
    }

    void vary_traffic(Network& topo, char order, string& spec) throw (exception) {
        string origName = extract_nodename_from_spec(spec);
        string dstName = extract_nodename_from_spec(spec);

        const Node& src = topo.getNode(origName);
        const Node& dst = topo.getNode(dstName);

        double lambda = atof(spec.c_str());

        if (order == 'A')
            topo.addTrafficToRoute(src, dst, lambda);
        else if (order == 'R')
            topo.removeTrafficFromRoute(src, dst, lambda);
        else
            cerr << "Incorrect line: " << order << ' ' << spec << endl;
    }

    void dump_info(Network& net, string& spec) {
        string origName = extract_nodename_from_spec(spec);
        string dstName = extract_nodename_from_spec(spec);

        double cost;
        Route route;

        if (origName != "" && dstName != "") {
            const Node& src = net.getNode(origName);
            const Node& dst = net.getNode(dstName);
            const Network::RouteInfo& r = net.getRoute(src, dst);

            route = r.first;
            cost = r.second;
        } else {
            cost = net.getTotalCost();
        }

        if (route.empty()) {
            cout << "C: " << cost << " @" << net.getTotalTraffic()
                    << " (" << cost << ')' << endl;
        } else {
            cout << "C: " << route
                    << " (" << cost << ')' << endl;
        }
    }

    void send_army(Network& net, const string& spec) {
        int militia;
        int verbosity;
        istringstream is(spec);
        set<pair<Node, Node> > routes = net.copyRoutes();

        is >> militia >> verbosity;

        for (int i = 0; i < militia; i++) {
            for (const pair<Node, Node>& np : routes) {
                bool alert = false;
                Network::RouteInfo rinfo = net.sendAnt(np.first, np.second, &alert);
                if (verbosity && alert) {
                    cout << "T (it=" << i << ") " << rinfo.first << " (" << rinfo.second << ')' << endl;
                }
            }
        }
    }

    void run_simulation(Network& net, const string& prefix) {
        ifstream runfile(prefix + ".traffic");
        string line;
        int line_number = 0;

        try {
            while (getline(runfile, line)) {
                string working_line;                
                char order = line[0];
                line_number += 1;

                if (line.size() > 1) {
                    working_line = line.substr(1);
                }
                
                switch (line[0]) {
                    case '#': continue;
                    case 'A':
                    case 'R':
                        vary_traffic(net, order, working_line);
                        break;
                    case 'C':
                        dump_info(net, working_line);
                        break;
                    case 'T':
                        send_army(net, working_line);                        
                        break;
                    case 'S':
                        Simulation::setRandomSeed(atoi(working_line.c_str()));
                        break;
                    default:
                        cerr << "Malformed line: " << line << endl;
                        return;
                }
            }
        } catch (const exception& e) {
            cerr << "Error while simulation at line " << line_number << ": " << line << endl
                    << e.what() << endl;
        }
    }
}

/*
 * 
 */
int main(int argc, char** argv) {
    if (argc < 2) {
        return usage(argc, argv);
    }

    parse_args(argc, argv);

    Network net;

    parse_config(net, conf_prefix);
    run_simulation(net, conf_prefix);

    return 0;
}
