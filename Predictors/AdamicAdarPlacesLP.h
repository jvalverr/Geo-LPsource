/*
------------------------------------------------
Copyright (C) 2016-2019 by Jorge C. Valverde-Rebaza
This file is part of Geo-LPsource.

The code to calculate this link prediction measure is based on the paper [5].

[3] O. Mengshoel, R. Desail, A. Chen, B. Tran, Will we connect again? machine learning for
    link prediction in mobile social networks. In Proc. of ACM MLG 2013, 2013.

Geo-LPsource is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
Geo-LPsource is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should
have received a copy of the GNU General Public License along with Geo-LPsource. If not, see <http://www.gnu.org/licenses/>.
------------------------------------------------
*/

#ifndef ADAMICADARCOMMONPLACESLP_H_INCLUDED
#define ADAMICADARCOMMONPLACESLP_H_INCLUDED

#include "../Network.h"
#include "LinkPredictor.h"

class AdamicAdarPlacesLP : public LinkPredictor {
	private:
	protected:
	public:
		AdamicAdarPlacesLP( const Network& );
		~AdamicAdarPlacesLP();
		virtual double generateScore( index_v, index_v );
};

#endif
