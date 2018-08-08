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


#include "NearbyDistanceAllocationLP.h"

NearbyDistanceAllocationLP::NearbyDistanceAllocationLP( const Network& network ) : LinkPredictor(network) {
}

NearbyDistanceAllocationLP::~NearbyDistanceAllocationLP() {
}

double NearbyDistanceAllocationLP::generateScore( index_v indexVertex1, index_v indexVertex2 ) {
    vector<index_v> vCommonFriends = network.intersect(network.getNeighbors(indexVertex1), network.getNeighbors(indexVertex2));
    long home1 = network.getHomeLocation(indexVertex1);
    long home2 = network.getHomeLocation(indexVertex2);
    double nda = 0;

    if(home1 == -1 || home2 == -1)
        return 0;

    //we work with the common neighbors of places
    for(index_v u = 0; u < vCommonFriends.size(); u++){
        //first we find by common places between x and z
        bool flagIntersect = network.existIntersectPlaces(indexVertex1, vCommonFriends[u]);

        if(flagIntersect == false){
            //we find common places between y and z
            flagIntersect = network.existIntersectPlaces(indexVertex2, vCommonFriends[u]);
        }

        //after first or second attemp, we count
        if(flagIntersect == true){
            long home3 = network.getHomeLocation(vCommonFriends[u]);

            if(home3 != -1){
                //calculate the adjusted distance for users x and z
                double adjd1 = network.getAdjustedGeographicDistance(home1, home3);

                //calculate the adjusted distance for users y and z
                double adjd2 = network.getAdjustedGeographicDistance(home2, home3);

                //allocate the minimum value
                double minad = adjd1;

                if(adjd2 < adjd1)
                    minad = adjd2;

                if(minad > 0)
                    nda += 1.0/minad;
            }//end of home 3
        }//end of if
    }//end of for

    return nda;
}
