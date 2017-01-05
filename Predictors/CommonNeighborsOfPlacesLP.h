/*
------------------------------------------------
Copyright (C) 2016-2017 by Jorge C. Valverde-Rebaza
Email: jvalverr@icmc.usp.br
This file is part of Geo-LPsource.

The code to calculate this link prediction measure is based on the paper [2].

[2] Jorge Valverde-Rebaza and Mathieu Roche and Pascal Poncelet and Alneu Lopes. "Exploiting social
    and mobility patterns for friendship prediction in location-based social networks". In Proc. of
    Int. Conf. on Pattern Recognition (ICPR 2016), 2016, to be published.

Geo-LPsource is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
Geo-LPsource is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should
have received a copy of the GNU General Public License along with Geo-LPsource. If not, see <http://www.gnu.org/licenses/>.
------------------------------------------------
*/


#ifndef COMMONNEIGHBORSOFPLACESLP_H_INCLUDED
#define COMMONNEIGHBORSOFPLACESLP_H_INCLUDED

#include "../Network.h"
#include "LinkPredictor.h"

class CommonNeighborsOfPlacesLP : public LinkPredictor {
	private:
	protected:
	public:
		CommonNeighborsOfPlacesLP( const Network& );
		~CommonNeighborsOfPlacesLP();
		double generateScore( index_v, index_v );
};

#endif // COMMONNEIGHBORSOFPLACESLP_H_INCLUDED
