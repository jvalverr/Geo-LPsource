/*
------------------------------------------------
Copyright (C) 2016-2017 by Jorge C. Valverde-Rebaza
Email: jvalverr@icmc.usp.br
This file is part of Geo-LPsource.

Geo-LPsource is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
Geo-LPsource is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should
have received a copy of the GNU General Public License along with Geo-LPsource. If not, see <http://www.gnu.org/licenses/>.
------------------------------------------------
*/


#include "LinkPredictor.h"
typedef std::numeric_limits< double > dbl;

LinkPredictor::LinkPredictor( const Network& network ) : network(network), vertex(INF_P), neighbor(INF_P)  {
}

LinkPredictor::~LinkPredictor() {
}

//for friendship
void LinkPredictor::printFriendshipScores( ostream& os, unsigned int degree){
    if(degree <= 1){
        cerr << "For degree = " <<  degree << " we assume that the degree is 2";
        degree = 2;
    }

    os.precision(dbl::digits10);
    for(index_v indexVertex = 0; indexVertex < network.getNumUsers(); indexVertex++){
        vector<index_v> verticesToAnalize = network.getNeighbors( indexVertex, degree );

        for(unsigned int j = 0; j < verticesToAnalize.size(); j++ ){
            index_v indexNeighbor = verticesToAnalize[j];
            if(indexNeighbor > indexVertex){//because we consider that it is an undirected network
                double predictValue = this->generateScore(indexVertex, indexNeighbor);

                if( predictValue != 0.0 ){
                    os << indexVertex << " " << indexNeighbor << " " << predictValue <<"\n";
                }
            }
        }
    }
}

//for friendship
vector<Link> LinkPredictor::getListScores( unsigned int degree ){
    vector<Link> listLinks;
    if(degree <= 1){
        cerr << "For degree = " <<  degree << " we assume that the degree is 2";
        degree = 2;
    }


    for(index_v indexVertex = 0; indexVertex < network.getNumUsers(); indexVertex++){
        vector<index_v> verticesToAnalize = network.getNeighbors( indexVertex, degree );

        for(unsigned int j = 0; j < verticesToAnalize.size(); j++ ){
            index_v indexNeighbor = verticesToAnalize[j];
            if(indexNeighbor > indexVertex){//because we consider that it is an undirected network
                double predictValue = this->generateScore(indexVertex, indexNeighbor);

                if( predictValue != 0.0 ){
                    Link link;
                    link.indexVertex1 = indexVertex;
                    link.indexVertex2 = indexNeighbor;
                    link.score        = predictValue;
                    listLinks.push_back(link);
                }
            }
        }
    }
    return listLinks;
}

//for place prediction
void LinkPredictor::printPlacePredictionScores( ostream& os ){
    os.precision(dbl::digits10);
    for(index_v indexVertex = 0; indexVertex < network.getNumUsers(); indexVertex++){
        if(network.isUserForPlacePrediction(indexVertex) == true){
            //we take the potential places to be visited by the use
            vector<index_p> vPotentialPlaces = network.getPotentialPlacesToVisit(indexVertex);
            for(index_p i = 0; i < vPotentialPlaces.size(); i++){
                double predictValue = this->generateScore(indexVertex, vPotentialPlaces[i]);

                if( predictValue != 0.0 ){
                     os << indexVertex << " " << vPotentialPlaces[i] << " " << predictValue <<"\n";
                }
            }

        }//end of if
    }//end of for
}



