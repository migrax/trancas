Trancas
=======

A simulator for a new Energy Aware routing algorithm based on Ant Colonization

Overview
--------

This is the simulator for the routing algorithm described in the paper ["An Ant Colonization Routing Algorithm to Reduce Network Energy Consumption"](http://dx.doi.org/10.1016/j.jnca.2015.08.011).

Usage
-----

The simulator reads two files described in the file Configuration.txt. One file describes the network topology, while the second one control the behavior of the agents and the traffic matrix.

Invocation
----------

`trancas prefix`

Where `prefix` is the filename prefix common to the two configuration files.

Output
------

The output of the simulator is controlled by the configuration file. The simulator can print the current routes for the traffic, and he associated energy demands.

Legal
-----

Copyright © Miguel Rodríguez Pérez <miguel@det.uvigo.es> 2011—2014

This simulator is licensed under the GNU General Public License, version 3 (GPL-3.0). For for information see LICENSE.txt

