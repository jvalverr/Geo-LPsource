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


#include "TotalAndPartialOverlappingOfPlacesLP.h"

TotalAndPartialOverlappingOfPlacesLP::TotalAndPartialOverlappingOfPlacesLP( const Network& network ): LinkPredictor(network){
}

TotalAndPartialOverlappingOfPlacesLP::~TotalAndPartialOverlappingOfPlacesLP() {
}

double TotalAndPartialOverlappingOfPlacesLP::generateScore( index_v indexVertex1, index_v indexVertex2 ) {
    double top = 0.0;
    double pop = 0.0;
    vector<index_v> vCommonFriends = network.intersect(network.getNeighbors(indexVertex1), network.getNeighbors(indexVertex2));

    for(index_v u = 0; u < vCommonFriends.size(); u++){
        bool flagIntersectxz = network.existIntersectPlaces(indexVertex1, vCommonFriends[u]);
        bool flagIntersectyz = network.existIntersectPlaces(indexVertex2, vCommonFriends[u]);

        //This predictor limits the space only for common neighbors of places
        if(flagIntersectxz == true || flagIntersectyz == true){
            if(flagIntersectxz == true && flagIntersectyz == true)
                top += 1.0;
            else
                pop += 1.0;
        }
    }//end of for

    if(pop == 0.0)
        return top;

    return top/pop;
}
