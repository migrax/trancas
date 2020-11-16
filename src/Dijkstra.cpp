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

#include "Dijkstra.h"
#include "Network.h"
#include "Node.h"

#include <set>
#include <algorithm>

using namespace std;

Dijkstra::Dijkstra(const Node& src, const Node& dst,
        const Network& graph, double lambda, CostFunction costfunc) : ShortestPath(src, dst, graph, lambda), costFunc(costfunc) {
    set<Node> reachable;

    Node last_added = src;

    distances_map distances;

    distances.insert(make_pair(src, make_pair(0, make_pair(src, src))));

    while (reachable.find(dst) == reachable.end()) {                
        reachable.insert(last_added);
        updateDistances(&distances, last_added);

        last_added = getNearestNode(distances, reachable);
        if (reachable.find(last_added) != reachable.end()) {
            throw SPFException("There is no route from " + src.getName() + " to " + dst.getName());
        }
    }

    // And now, just populate the parent info
    Node current = dst;
    nodes.push_back(current);
    while (current != src) {
        current = distances.find(current)->second.second.first;
        nodes.push_back(current);        
    };
    
    reverse(nodes.begin(), nodes.end());
}

void Dijkstra::updateDistances(Dijkstra::distances_map *distances,
        Node newest) const {
    const Distance<double> distance2Newest = distances->find(newest)->second.first;
    const set<Node> neighbours = graph.getNeighbours(newest);
    
    for (Node n : neighbours) {
        const Node::NodePair np(newest, n);
        const Link l = graph.getLink(np);        

        double cost;
        switch(costFunc) {
            case Real:
                cost = l.getCost(lambda);
                break;
            case Constant:
                if (l.getMaxTraffic() <= 0 || l.getCurrentTraffic() + lambda < l.getMaxTraffic()) {
                    cost = 1.0;
                } else {
                    cost = numeric_limits<double>::infinity();
                }
                break;
            default:
                throw SPFException("Unhandled cost function");
        }
        
        const Distance<double> distance = cost + distance2Newest;        

        if (distances->find(n) == distances->end()) {                
            distances->insert(make_pair(n, make_pair(distance, np)));
        } else {
            Distance<double> current_distance = distances->find(n)->second.first;

            if (distance < current_distance) {                              
                distances->erase(n);                
                distances->insert(make_pair(n, make_pair(distance, np)));                
            }
        }
    }
}

Node Dijkstra::getNearestNode(const Dijkstra::distances_map& distances, const set<Node>& reachable) const noexcept {
    static const Node dummy("dummy at Dijkstra::getNearestNode");
    
    Node closest(dummy);
    Distance<double> distance = numeric_limits<double>::infinity();    

    for (auto d : distances) {           
        if (reachable.find(graph.getNode(d.first)) == reachable.end()) {
            if (d.second.first < distance) {
                distance = d.second.first;
                closest = graph.getNode(d.first);
            }
        }
    };
    
    return closest;
}