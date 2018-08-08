/*
------------------------------------------------
Copyright (C) 2016-2019 by Jorge C. Valverde-Rebaza
This file is part of Geo-LPsource.

The code to calculate this link prediction measure is based on the paper [5].

[7] Zhang, Y., Pang, J. Distance and friendship: A distance-based model for link prediction in
    social networks. Proceedings of the 17th asia-pacific web conference: Web technologies and
	applications, APWeb, 2015, Springer, pp. 55–66.

Geo-LPsource is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
Geo-LPsource is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should
have received a copy of the GNU General Public License along with Geo-LPsource. If not, see <http://www.gnu.org/licenses/>.
------------------------------------------------
*/


#include "GeodistLP.h"


GeodistLP::GeodistLP( const Network& network ) : LinkPredictor(network) {
}

GeodistLP::~GeodistLP() {
}

double GeodistLP::generateScore( index_v indexVertex1, index_v indexVertex2 ) {
    long indexHome1 = network.getHomeLocation(indexVertex1);
    if(indexHome1 == -1)
        return 0.0;

    long indexHome2 = network.getHomeLocation(indexVertex2);
    if(indexHome2 == -1)
        return 0.0;

    //for this method, two users are more similar if the geographic distance is a low value
    return BIG_EARTH_DISTANCE - network.getGeographicDistance(indexHome1, indexHome2);
}
