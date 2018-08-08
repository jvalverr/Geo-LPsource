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


#include "LocationCategoryLP.h"

LocationCategoryLP::LocationCategoryLP( const Network& network ) : LinkPredictor(network), maxEntropy(5.0) {

}

LocationCategoryLP::LocationCategoryLP( const Network& network, double maxE ) : LinkPredictor(network), maxEntropy(maxE) {

}

LocationCategoryLP::~LocationCategoryLP() {
}

void LocationCategoryLP::setMaxEntropy(double maxE){
    maxEntropy = maxE;
}

double LocationCategoryLP::generateScore( index_v indexVertex1, index_v indexVertex2 ) {
    vector<Place> vPlaces1 = network.getVisitedPlaces(indexVertex1);
    vector<Place> vPlaces2 = network.getVisitedPlaces(indexVertex2);
    double score = 0.0;

    //we consider that a place has the same category than other if only both have an entropy no greater than maxEntropy
    for(index_p i = 0; i < vPlaces1.size(); i++){
        if( network.getEntropyOfPlace(vPlaces1[i].label) < this->maxEntropy){
            double nCi1 = network.getCheckinsUserAtPlace(indexVertex1, vPlaces1[i].label).size();//we obtain the number of check-ins made by user at place
            double nci = network.getTotalCheckinsAtPlace(vPlaces1[i].label); //we obtain the total number of check-ins at place

            for(index_p j = 0; j < vPlaces2.size(); j++){
                if( network.getEntropyOfPlace(vPlaces2[j].label) < this->maxEntropy){
                    double nCj2 = network.getCheckinsUserAtPlace(indexVertex2, vPlaces2[j].label).size();//we obtain the number of check-ins made by users at places
                    double ncj = network.getTotalCheckinsAtPlace(vPlaces2[j].label);  //we obtain the total number of check-ins at places

                    double num =  nci + ncj;
                    double den = nCi1 + nCj2;

                    if(num != 0 && den !=0)
                        score += num/den;

                }//end of if
            }//end of for
        }//end of if
    }//end of for

    return score;
}
