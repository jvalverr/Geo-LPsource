/*
------------------------------------------------
Copyright (C) 2016-2017 by Jorge C. Valverde-Rebaza
Email: jvalverr@icmc.usp.br
This file is part of Geo-LPsource.

The code to calculate this link prediction measure is based on the paper [5].

[5] S. Scellato, A. Noulas, C. Mascolo, Exploiting place features in link prediction
on location-based social networks. In Proc. of ACM KDD, 2011, pp. 1046-1054

Geo-LPsource is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
LPsource is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should
have received a copy of the GNU General Public License along with Geo-LPsource. If not, see <http://www.gnu.org/licenses/>.
------------------------------------------------
*/

#ifndef ADAMICADARENTROPYLP_H_INCLUDED
#define ADAMICADARENTROPYLP_H_INCLUDED

#include "../Network.h"
#include "LinkPredictor.h"

class AdamicAdarEntropyLP : public LinkPredictor {
	private:
	protected:
	public:
		AdamicAdarEntropyLP( const Network& );
		~AdamicAdarEntropyLP();
		virtual double generateScore( index_v, index_v );
};

#endif // ADAMICADARENTROPYLP_H_INCLUDED
