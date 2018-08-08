/*
------------------------------------------------
Copyright (C) 2016-2017 by Jorge C. Valverde-Rebaza
This file is part of Geo-LPsource.

The code to calculate this link prediction measure is based on the paper [2].

[2] Jorge Valverde-Rebaza and Mathieu Roche and Pascal Poncelet and Alneu Lopes. "Exploiting social
    and mobility patterns for friendship prediction in location-based social networks". In Proc. of
    Int. Conf. on Pattern Recognition (ICPR 2016), 2016, pp.2526--2531.

Geo-LPsource is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
Geo-LPsource is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should
have received a copy of the GNU General Public License along with Geo-LPsource. If not, see <http://www.gnu.org/licenses/>.
------------------------------------------------
*/


#include "CommonNeighborsPlacesLP.h"

CommonNeighborsPlacesLP::CommonNeighborsPlacesLP( const Network& network ): LinkPredictor(network) {
}

CommonNeighborsPlacesLP::~CommonNeighborsPlacesLP() {
}


double CommonNeighborsPlacesLP::generateScore( index_v indexVertex1, index_v indexVertex2 ) {
    double cnp = 0.0;
    vector<index_v> vCommonFriends = network.intersect(network.getNeighbors(indexVertex1), network.getNeighbors(indexVertex2));

    for(index_v u = 0; u < vCommonFriends.size(); u++){
        //first we find by common places between x and z
        bool flagIntersect = network.existIntersectPlaces(indexVertex1, vCommonFriends[u]);

        if(flagIntersect == false){
            //we find common places between y and z
            flagIntersect = network.existIntersectPlaces(indexVertex2, vCommonFriends[u]);
        }

        //after first or second attemp, we count
        if(flagIntersect == true)
            cnp++;
    }

    return cnp;
}
