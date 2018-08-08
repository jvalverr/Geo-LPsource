/*
------------------------------------------------
Copyright (C) 2016-2019 by Jorge C. Valverde-Rebaza
This file is part of Geo-LPsource.

The code to calculate this link prediction measure is based on the paper [5].

[5] S. Scellato, A. Noulas, C. Mascolo, Exploiting place features in link prediction
on location-based social networks. In Proc. of ACM KDD, 2011, pp. 1046-1054

Geo-LPsource is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
Geo-LPsource is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should
have received a copy of the GNU General Public License along with Geo-LPsource. If not, see <http://www.gnu.org/licenses/>.
------------------------------------------------
*/

#include "AdamicAdarEntropyLP.h"

AdamicAdarEntropyLP::AdamicAdarEntropyLP( const Network& network ) : LinkPredictor(network) {
}

AdamicAdarEntropyLP::~AdamicAdarEntropyLP() {
}

double AdamicAdarEntropyLP::generateScore( index_v indexVertex1, index_v indexVertex2 ) {
    vector<index_p> vCommonP = network.getIntersectPlaces(indexVertex1, indexVertex2);
    double aaE = 0;

    for(index_p i = 0; i < vCommonP.size(); i++){
        double entropy = network.getEntropyOfPlace( vCommonP[i] );
        if( entropy != 0 )
            aaE += 1.0/(log10(entropy));
    }

    return aaE;
}
