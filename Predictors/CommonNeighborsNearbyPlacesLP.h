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


#ifndef COMMONNEIGHBORSNEARBYPLACESLP_H_INCLUDED
#define COMMONNEIGHBORSNEARBYPLACESLP_H_INCLUDED


#include "../Network.h"
#include "LinkPredictor.h"

class CommonNeighborsNearbyPlacesLP : public LinkPredictor {
	private:
	    double maxDistance;
	protected:
	public:
	    CommonNeighborsNearbyPlacesLP( const Network& );
		CommonNeighborsNearbyPlacesLP( const Network&, double );
		~CommonNeighborsNearbyPlacesLP();
		void setMaxDistance(double);
		double generateScore( index_v, index_v );
};


#endif // COMMONNEIGHBORSNEARBYPLACESLP_H_INCLUDED
