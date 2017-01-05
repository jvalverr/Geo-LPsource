/*
------------------------------------------------
Copyright (C) 2016-2017 by Jorge C. Valverde-Rebaza
Email: jvalverr@icmc.usp.br
This file is part of Geo-LPsource.

The code to calculate this link prediction measure is based on the paper [3].

[3] O. Mengshoel, R. Desail, A. Chen, B. Tran, Will we connect again? machine learning for
    link prediction in mobile social networks. In Proc. of ACM MLG 2013, 2013.

Geo-LPsource is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
LPsource is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should
have received a copy of the GNU General Public License along with Geo-LPsource. If not, see <http://www.gnu.org/licenses/>.
------------------------------------------------
*/

#ifndef COLLOCATIONSLP_H_INCLUDED
#define COLLOCATIONSLP_H_INCLUDED


#include "../Network.h"
#include "LinkPredictor.h"


class CollocationsLP : public LinkPredictor {
	private:
            threshold diffTime;
	protected:
	public:
		CollocationsLP( const Network& );
		CollocationsLP( const Network&, threshold);
		~CollocationsLP();
		void setThreshold(threshold);
		virtual double generateScore( index_v, index_v );
};



#endif // COLLOCATIONSLP_H_INCLUDED
