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


#include "AdjustedHausdorffDistanceLP.h"

AdjustedHausdorffDistanceLP::AdjustedHausdorffDistanceLP( const Network& network ) : LinkPredictor(network) {
    ceilAH = BIG_EARTH_DISTANCE * network.getMaxDiversityOfPlaces();
}

AdjustedHausdorffDistanceLP::~AdjustedHausdorffDistanceLP() {
}

double AdjustedHausdorffDistanceLP::generateScore( index_v indexVertex1, index_v indexVertex2 ) {
    //for user 1
    long ele      = network.getSupremumPlace(indexVertex1);
    long eleprima = network.getInfimumPlace(indexVertex2);

    if(ele == -1 || eleprima == -1)
        return 0;

    double d1 = network.getAdjustedGeographicDistance(ele, eleprima);

    //for user 2
    ele       = network.getSupremumPlace(indexVertex2);
    eleprima  = network.getInfimumPlace(indexVertex1);

    if(ele == -1 || eleprima == -1)
        return 0;

    double d2 = network.getAdjustedGeographicDistance(ele, eleprima);

    //we take the max value as Hausdorff distance, but for link prediction task we need the minimum value of this distance
    if(d2 > d1)
        return ceilAH - d2;

    return ceilAH - d1;
}
