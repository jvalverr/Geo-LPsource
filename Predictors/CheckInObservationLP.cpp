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

#include "CheckInObservationLP.h"

CheckInObservationLP::CheckInObservationLP( const Network& network ) : LinkPredictor(network) {
}

CheckInObservationLP::~CheckInObservationLP() {
}

double CheckInObservationLP::generateScore( index_v indexVertex1, index_v indexVertex2 ) {
    vector<index_p> vCommonP = network.getIntersectPlaces(indexVertex1, indexVertex2);
    double num = 0.0;

    for(index_p i = 0; i < vCommonP.size(); i++){
        double cik1 = network.getCheckinsUserAtPlace(indexVertex1, vCommonP[i]).size();
        double cik2 = network.getCheckinsUserAtPlace(indexVertex2, vCommonP[i]).size();
        num += cik1 + cik2;
    }//end of for

    if(num == 0.0)
        return 0.0;

    vector<Place> vPlaces1 = network.getVisitedPlaces(indexVertex1);
    vector<Place> vPlaces2 = network.getVisitedPlaces(indexVertex2);

    if(vPlaces1.size() == 0 || vPlaces2.size() == 0)
        return 0.0;

    double sum1 = 0.0, sum2 = 0.0;

    for(index_p i = 0; i < vPlaces1.size(); i++)
        sum1 += network.getCheckinsUserAtPlace(indexVertex1, vPlaces1[i].label).size();

    for(index_p i = 0; i < vPlaces2.size(); i++)
        sum2 += network.getCheckinsUserAtPlace(indexVertex2, vPlaces2[i].label).size();

    if(sum1 == 0 && sum2 == 0)
        return 0;

    return num/(sum1 + sum2);
}
