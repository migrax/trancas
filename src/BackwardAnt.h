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

#ifndef BACKWARDANT_H
#define	BACKWARDANT_H

#include "ForwardAnt.h"
#include "Network.h"

#include <utility>
#include <vector>
#include <map>

class BackwardAnt : public Ant {
public:
    BackwardAnt(ForwardAnt &&ant);

    Node advance();
    Route getRoute();

private:
    std::vector<linkInfo> reversedNodes = std::vector<linkInfo>();
    std::map<Node::NodePair, double> unvisitedLinks;
    Route newRoute;
    
    bool prepared = false;
    double routeCost = 0;
    double extraCost;

    void prepareRoute();
    double calcExtraCost(const Node::NodePair &np) const;
};

#endif	/* BACKWARDANT_H */

