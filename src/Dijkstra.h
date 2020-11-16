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

#ifndef DIJKSTRA_H
#define	DIJKSTRA_H

#include "ShortestPath.h"
#include "Distance.h"

#include <map>
#include <utility>
#include <set>

#include "Node.h"

class Network;

/* This is NOT the complete Dijkstra algorithm. It stops once the route
 * to dst has been found. */

class Dijkstra : public ShortestPath {
public:
    enum CostFunction {Real, Constant};

    Dijkstra(const Node &src, const Node &dst,
             const Network &graph, double lambda, CostFunction cost = Real);


private:    
    typedef std::map<std::string, std::pair<Distance<double>, Node::NodePair> > distances_map;
    CostFunction costFunc;
    
    void updateDistances(distances_map *distances, Node newest) const;
    Node getNearestNode(const distances_map& distances, const std::set<Node>& reachable) const noexcept;
};

#endif	/* DIJKSTRA_H */

