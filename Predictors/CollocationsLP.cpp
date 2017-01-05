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
Geo-LPsource is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should
have received a copy of the GNU General Public License along with Geo-LPsource. If not, see <http://www.gnu.org/licenses/>.
------------------------------------------------
*/


#include "CollocationsLP.h"

CollocationsLP::CollocationsLP( const Network& network ) : LinkPredictor(network), diffTime(0) {
}

CollocationsLP::CollocationsLP( const Network& network, threshold  thrTime ) : LinkPredictor(network), diffTime(thrTime) {
}

CollocationsLP::~CollocationsLP() {
}

void CollocationsLP::setThreshold(threshold thrTime){
    if(thrTime < 0)
        diffTime = 0;

    diffTime =  thrTime;
}

double CollocationsLP::generateScore( index_v indexVertex1, index_v indexVertex2 ) {
    vector<Place> v1 = network.getVisitedPlaces(indexVertex1);
    vector<Place> v2 = network.getVisitedPlaces(indexVertex2);
    double ncol = 0;

    //v1 and v2 are sorted
    for(index_v i = 0; i < v1.size(); i++){
        for(index_v j = 0; j < v2.size(); j++){
            if( v1[i].label < v2[j].label || v1[i].label > v2[v2.size()-1].label ){ //we limit the search space
                j = v2.size();
            }
            else{
                if( v1[i].label == v2[j].label ){ //a match, we will calculate the collocations
                    for(index_v m = 0; m < v1[i].CheckinsList.size(); m++) {
                        for(index_v n = 0; n < v2[j].CheckinsList.size(); n++){
                            //checkin list also are sorted
                            if( v1[i].CheckinsList[m].day < v2[j].CheckinsList[n].day || v1[i].CheckinsList[m].day > v2[j].CheckinsList[v2[j].CheckinsList.size()-1].day){
                                n = v2[j].CheckinsList.size();
                            }
                            else{
                                threshold diffTimeVisit = abs(v1[i].CheckinsList[m].day - v2[j].CheckinsList[n].day);
                                if( diffTimeVisit <= this->diffTime )
                                    ncol++;
                            }
                        }
                    }//end of for
                }
            }//end of else
        }//end of for
    }//end of for

    return ncol;
}
