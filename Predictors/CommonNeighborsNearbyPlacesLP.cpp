/*
------------------------------------------------
Copyright (C) 2016-2019 by Jorge C. Valverde-Rebaza
This file is part of Geo-LPsource.

The code to calculate this link prediction measure is based on the paper [5].


[8] Jorge Valverde-Rebaza, Mathieu Roche, Pascal Poncelet, Alneu Lopes. The role of location and social strength
    for friendship prediction in location-based social networks. Information Processing and Management, vol. 54,
    2018, pp.475-489.

Geo-LPsource is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
Geo-LPsource is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should
have received a copy of the GNU General Public License along with Geo-LPsource. If not, see <http://www.gnu.org/licenses/>.
------------------------------------------------
*/


#include "CommonNeighborsNearbyPlacesLP.h"

CommonNeighborsNearbyPlacesLP::CommonNeighborsNearbyPlacesLP( const Network& network ): LinkPredictor(network), maxDistance(300){
}

CommonNeighborsNearbyPlacesLP::CommonNeighborsNearbyPlacesLP( const Network& network, double maxD ): LinkPredictor(network), maxDistance(maxD){
}

CommonNeighborsNearbyPlacesLP::~CommonNeighborsNearbyPlacesLP() {
}

void CommonNeighborsNearbyPlacesLP::setMaxDistance(double maxD){
    maxDistance = maxD;
}

double CommonNeighborsNearbyPlacesLP::generateScore( index_v indexVertex1, index_v indexVertex2 ) {
    long home1 = network.getHomeLocation(indexVertex1);
    long home2 = network.getHomeLocation(indexVertex2);

    if(home1 == -1 || home2 == -1)
        return 0.0;

    vector<index_v> vCommonFriends = network.intersect(network.getNeighbors(indexVertex1), network.getNeighbors(indexVertex2));
    double cnearby = 0.0;

    for(index_v u = 0; u < vCommonFriends.size(); u++){
        long home3 = network.getHomeLocation(vCommonFriends[u]);
        if(home3 != -1){
            double dist1 = network.getGeographicDistance(home1, home3);
            double dist2 = network.getGeographicDistance(home2, home3);

            if(dist1 <= this->maxDistance || dist2 <= this->maxDistance)
                cnearby += 1.0;
        }
    }

    return cnearby;
}
