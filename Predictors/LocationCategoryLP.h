/*
------------------------------------------------
Copyright (C) 2016-2019 by Jorge C. Valverde-Rebaza
This file is part of Geo-LPsource.

The code to calculate this link prediction measure is based on the paper [5].

[6] Luo, H., Guo, B., Zhiwenyu, Wang, Z., Feng, Y. Friendship prediction based on the fusion of
    topology and geographical features in LBSN. 2013 IEEE 10th Int. Conf. on high performance
	computing and communications & 2013 IEEE international conference on embedded and ubiquitous
	computing, HPCC-EUC, 2013, IEEE, pp. 2224–2230.

Geo-LPsource is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
Geo-LPsource is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should
have received a copy of the GNU General Public License along with Geo-LPsource. If not, see <http://www.gnu.org/licenses/>.
------------------------------------------------
*/

#ifndef LOCATIONCATEGORYLP_H_INCLUDED
#define LOCATIONCATEGORYLP_H_INCLUDED


#include "../Network.h"
#include "LinkPredictor.h"

class LocationCategoryLP : public LinkPredictor {
	private:
            double maxEntropy;
	protected:
	public:
		LocationCategoryLP( const Network& );
		LocationCategoryLP( const Network&, double );
		~LocationCategoryLP();
		void setMaxEntropy(double);
		virtual double generateScore( index_v, index_v );
};


#endif // LOCATIONCATEGORYLP_H_INCLUDED
