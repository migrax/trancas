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

#ifndef NODE_H
#define	NODE_H

#include <string>

class Node {
public:
    Node(const std::string& name) noexcept : name(name) {}
    
    operator const std::string& () const noexcept {
        return name;
    }
private:
    std::string name;
    
    /*
     * Routes to every destination
     * Traffic for every neightbour. How? (Idea al nodo fuente "refresca" las rutas cuando llegan las hormigas de vuelta... en la práctica se haría con estimaciones de tráfico)
     * Links do not belong here. Another DB. 
     * 
     */
};

#endif	/* NODE_H */

