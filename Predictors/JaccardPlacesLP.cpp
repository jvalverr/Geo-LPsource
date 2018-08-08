/*
------------------------------------------------
Copyright (C) 2016-2019 by Jorge C. Valverde-Rebaza
This file is part of Geo-LPsource.

The code to calculate this link prediction measure is based on the paper [4].

[4] J. Cranshaw, E. Toch, J. Hong, A. Kittur, N. Sadeh, Bridging the gap between physical
    location and online social networks. In Proc. of the 12th ACM International Conference
    on Ubiquitous Computing, UbiComp '10, ACM, 2010, pp. 119-128.

Geo-LPsource is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
Geo-LPsource is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should
have received a copy of the GNU General Public License along with Geo-LPsource. If not, see <http://www.gnu.org/licenses/>.
------------------------------------------------
*/

#include "JaccardPlacesLP.h"

JaccardPlacesLP::JaccardPlacesLP( const Network& network ) : LinkPredictor(network) {
}

JaccardPlacesLP::~JaccardPlacesLP() {
}

double JaccardPlacesLP::generateScore( index_v indexVertex1, index_v indexVertex2 ) {
   double nintersect = network.getIntersectPlaces(indexVertex1, indexVertex2).size();

   if( nintersect == 0 )
        return 0.0;

   double njunction = network.getVisitedPlaces(indexVertex1).size() + network.getVisitedPlaces(indexVertex2).size() - nintersect;
   if( njunction == 0 )
        return 0.0;

   return nintersect/njunction;
}
