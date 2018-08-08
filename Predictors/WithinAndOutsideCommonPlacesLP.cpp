/*
------------------------------------------------
Copyright (C) 2016-2019 by Jorge C. Valverde-Rebaza
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

#include "WithinAndOutsideCommonPlacesLP.h"

WithinAndOutsideCommonPlacesLP::WithinAndOutsideCommonPlacesLP( const Network& network ): LinkPredictor(network) {
}

WithinAndOutsideCommonPlacesLP::~WithinAndOutsideCommonPlacesLP() {
}

double WithinAndOutsideCommonPlacesLP::generateScore( index_v indexVertex1, index_v indexVertex2 ) {
    vector<index_p> vCommonP = network.getIntersectPlaces( indexVertex1, indexVertex2 );
    if(vCommonP.size() == 0)
        return 0.0;

    //we are going to calculate wcp and ocp values
    double wcp = 0.0, ocp = 0.0;
    vector<index_v> vCommonFriends = network.intersect(network.getNeighbors(indexVertex1), network.getNeighbors(indexVertex2));
    for(index_v u = 0; u < vCommonFriends.size(); u++){
        vector<Place> vPlaces_z = network.getVisitedPlaces(vCommonFriends[u]);
        bool flagIntersect = false;
        //we just need know if z had visited at leat one place in common with x and y
        for(index_p i = 0; i < vCommonP.size(); i++){
            for(index_p j = 0; j < vPlaces_z.size(); j++){
                if( (long) vCommonP[i] < vPlaces_z[j].label )
                    j = vPlaces_z.size();
                else{
                    if( (long) vCommonP[i] == vPlaces_z[j].label ){
                        flagIntersect = true;
                        j = vPlaces_z.size();
                        i = vCommonP.size();
                    }
                }//end of else
            }//end of for
        }//end of for

        if(flagIntersect == true)
            wcp += 1.0;
        else
            ocp += 1.0;
    }//end of for

    if(ocp == 0.0)
        return wcp;

    return wcp / ocp;
}

