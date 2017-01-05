/*
------------------------------------------------
Copyright (C) 2016-2017 by Jorge C. Valverde-Rebaza
Email: jvalverr@icmc.usp.br

This file is part of Geo-LPsource.

Parts of the code of this file to calculate the topological properties of the network are based on parts of LPmade and LPsource softwares.
The main papers which are the base for this code are [1] and [2].

[1] Jorge Valverde-Rebaza and Alneu Lopes. "Link Prediction in Online Social Networks Using Group Information". The 14th
    International Conference on Computational Science and Its Applications (ICCSA 2014). Guimarães, Portugal, July 2014.

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


#include "Network.h"
#include "Utils.h"



Network::Network(): numEdges(0), numNodes(0), maxNumFriendsPerUser(INV_LONG), isWeighted(false), numCheckIn(0),  numNodesWithCheckIn(0), numTotalPlaces(0),
minDegreeToSampling(1), minNumCheckinsToSampling(1), calculateEntropyOfPlaces(false), calculateHomes(false), calculateBoundaries(false), maxNumCheckins(0),
maxEntropyValue(-1), numNodesDeleted(0), minNumPlacesToSampling(1) {
}

Network::~Network() {
}


Network& Network::operator=( const Network& network ) {
	this->numEdges              = network.numEdges;
    this->numNodes              = network.numNodes;
    this->isWeighted            = network.isWeighted;

    return *this;
}

bool Network::isNetworkWeighted()const{
    return  this->isWeighted;
}

double Network::getSampleCorrelationCoefficient(vector<vector<double> >& x, vector<vector<double> >& y)const{
    return sampleCorrelationCoefficient(x, y);
}

double Network::getSampleCorrelationCoefficient(vector<double>& x, vector<double>& y)const{
    return sampleCorrelationCoefficient(x, y);
}



////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////     READS                   ///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////


void Network::readLinksFile( const char* linksFile ){
    ifstream isLinks;
    isLinks.open( linksFile, ios::in );
    unsigned int maxNodes = 0;

     if(isLinks.is_open()){
        while(!isLinks.eof()){
            unsigned int idUser1, idUser2;
            isLinks >> idUser1 >> idUser2;

            //calculate the new max value of number of nodes
            if( idUser1 > maxNodes )
                maxNodes = idUser1;
            if( idUser2 > maxNodes )
                maxNodes = idUser2;

            //create new instances for the new users
            if(Users.size() < (maxNodes + 1)){
                for(index_v i = Users.size(); i < (maxNodes + 1); i++){
                    Node user;
                    user.Groups.reserve(1);
                    this->Users.push_back(user);
                }
            }

            //instance the new relationship
            this->Users[idUser1].Neighbors.push_back(idUser2);
            this->Users[idUser1].ntotalCheckins = 0;
            this->Users[idUser1].active = false;
            this->numEdges++;
        }
     }
     else{
        cerr << "\nEdges file no found";
     }

     numNodes = maxNodes + 1;
     isLinks.close();
     sortNeighborsOfUsers();
}

void Network::readCheckInFile( const char* checkInFile ){
    ifstream isCheckIn;
    isCheckIn.open( checkInFile, ios::in );
    long idNewUser = 0;
    vector<Place> checkinsOfUser;

    if( isCheckIn.is_open() ){
        long idUser;
        while( !isCheckIn.eof() ){
            long label       = INV_LONG;
            double latitude  = 0;
            double longitude = 0;
            Checkin checkin;

            isCheckIn >> idUser >> checkin.date >> latitude >> longitude >> label;

            if(label >= 0 && idUser < this->numNodes){
                //total number of effective checkins
                this->numCheckIn++;

                //we will add all check-ins assuming it as a place
                Place place;
                place.label     = label;
                place.latitude  = latitude;
                place.longitude = longitude;
                checkin.period  = monthToInt(checkin.date);
                checkin.day     = dayToInt(checkin.date);
                place.CheckinsList.push_back(checkin);

                if(idUser == idNewUser ){
                    checkinsOfUser.push_back(place);
                }
                else{
                    //save the user' place
                    this->Users[idNewUser].Places.swap(checkinsOfUser);
                    this->numNodesWithCheckIn++;

                     //free memory space
                    checkinsOfUser.clear();
                    idNewUser = idUser;

                    //add the last location for the new user
                    checkinsOfUser.push_back(place);
                }

                //update the list of users that visited the place
                long lastIndex = Places.size() - 1;
                if(label > lastIndex ){
                    //do not exist and we will create all the space that we need
                    for(long loc = Places.size(); loc <= label; loc++ ){
                        Place place_aux;
                        place_aux.label          = -1;
                        Places.push_back(place_aux);
                    }

                    //after, we update the information
                    place.UsersList.push_back(idUser);
                    Places[label] = place;
                }
                else{
                    //if exist, we only update the user list visiting the place
                    if( Places[label].label == -1 )
                        Places[label] = place;

                    Places[label].UsersList.push_back(idUser);
                }
            }
        }

        //for the last user
        if(idUser == idNewUser ){
            //save the user' locations
            this->Users[idNewUser].Places.swap(checkinsOfUser);
            this->numNodesWithCheckIn++;

            //free memory space
            checkinsOfUser.clear();
        }
    }
    else{
        cerr << "\nCheck-in file no found";
     }

    isCheckIn.close();
    setLocationInformation();
}

//we read a network and format it as undirected network, print it in a file, and after that, we remove all data from memory
void Network::makeUndirected(const char* linksFile, const char* outFile){
    ifstream isLinks;
    isLinks.open( linksFile, ios::in );
    unsigned int maxNodes = 0;

    if(isLinks.is_open()){
        while(!isLinks.eof()){

            unsigned int idUser1, idUser2;
            isLinks >> idUser1 >> idUser2;

            //calculate the new max value of number of nodes
            if( idUser1 > maxNodes )
                maxNodes = idUser1;
            if( idUser2 > maxNodes )
                maxNodes = idUser2;

            if(Users.size() < (maxNodes + 1)){
                for(index_v i = Users.size(); i < (maxNodes + 1); i++){
                    Node user;
                    this->Users.push_back(user);
                }
            }

            //instance the new relationship
            if(idUser2 > idUser1){
                this->Users[idUser1].Neighbors.push_back(idUser2);
                this->Users[idUser2].Neighbors.push_back(idUser1);
            }
        }
     }

    isLinks.close();
    sortNeighborsOfUsers();

    ofstream os;
    os.open(outFile);
    for(index_v i = 0; i < this->Users.size(); ++i ) {
          for(index_v j = 0; j < Users[i].Neighbors.size(); ++j) {
            os <<  i << " " << this->Users[i].Neighbors[j] << "\n";
          }
          //delete the user
          vector<index_v>  N;
          this->Users[i].Neighbors.swap(N);
    }
    os.close();
    vector<Node> N;
    Users.swap(N);
}


////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////     PRINTS                   ///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

void Network::printLinksList( ostream& os){
    os << "*Edges " << this->numEdges << "\n";

    for(index_v i = 0; i < this->Users.size(); ++i ) {
          for(index_v j = 0; j < Users[i].Neighbors.size(); ++j) {
            os <<  i << " " << this->Users[i].Neighbors[j] << "\n";
          }
    }
}

void Network::printLinksTest( ostream& os){
    for(index_v i = 0; i < TestLinks.size(); i++){
        os << TestLinks[i].indexVertex1 << " " << TestLinks[i].indexVertex2 << "\n";
    }
}

void Network::printWeightedLinksTest ( ostream& os ){
     for(index_v i = 0; i < TestLinks.size(); i++){
        os << TestLinks[i].indexVertex1 << " " << TestLinks[i].indexVertex2 << " " << TestLinks[i].score << "\n";
    }
}

void Network::printStatistics( ostream& os ){
    os << "\n\nGeneral information";
    //caracteristicas de alto nivel do grafo
    os << "\nNumber of users \t |V| \t"                      << getNumUsers();
    os << "\nNumber of links \t |E| \t"                      << getNumEdgesFriendshipGraph();
    os << "\nAverage number of friends per user \t - \t"     << getAverageFriendsPerUser();
    os << "\nMax number of friends per user \t - \t"         << getMaxNumberFriendsPerUser();

    os << "\n\nStructural information";
    os << "\nAverage degree             \t <k> \t"           << getAverageDegree();
    //os << "\nAverage path lenght            \t - \t"         << getAveragePathLenght();
    //os << "\nDiameter (max path length)         \t D2 \t"    << getDiameter();
    os << "\nClustering coefficient \t C \t"                 << getAverageClusteringCoefficient();
    os << "\nAssortativity coefficient \t r \t"              << getAverageAssortativityCoefficient();
    os << "\nHeterogeneity \t H \t"                          << getHeterogeneity();

    os << "\n\nLocation information";
    os << "\nNumber of check-ins \t - \t"                     << getTotalNumCheckIns();
    os << "\nNumber of users with >=1 check-ins  \t - \t"     << getNumNodesWithCheckIn();
    os << "\nAverage number of check-ins per user \t - \t"    << getAverageCheckInPerUser();
    os << "\nMax number of check-ins from all users \t - \t"  << getMaxNumUserCheckIns();
    os << "\nNumber of total places  \t - \t"                 << getNumberTotalPlaces();
    os << "\nAverage of check-ins per place       \t - \t"    << getAverageCheckInsPerLocation();
    os << "\nAverage entropy of places \t - \t"               << getAverageEntropyOfPlaces();
    os << "\nMin entropy of places \t - \t"                   << getMinEntropyOfPlaces();
    os << "\nMax entropy of places \t - \t"                   << getMaxEntropyOfPlaces();
    os << "\nMax diversity of places \t - \t"                 << getMaxDiversityOfPlaces();

/*
    os << "\n\nGroup information";
    os << "\nNumber of different groups  \t - \t"                   << getNumberOfGroups();
    os << "\nAverage of groups membership per user \t - \t"         << getAverageGroupMembershipPerUser();
    os << "\nAverage group size \t - \t"                            << getAverageGroupSize();
    os << "\nGroup Clustering coefficient \t C_g \t"                << getAverageGroupClusteringCoefficient();
    os << "\nAverage Overlapping Group Degree  \t k^g \t"           << getAverageOverlappingGroupsDegree();
    os << "\nOverlapping Group Clustering Coefficient  \t C^G \t"   << getAverageOverlappingGroupsClusteringCoefficient();
*/
}

void Network::printCheckinList( ostream& os){
    os << "Printing check-ins list\n";
    for(index_v i = 0; i < this->Users.size(); ++i ) {
        for(index_v j = 0; j < Users[i].Places.size(); ++j) {
           for(index_v k = 0; k < Users[i].Places[j].CheckinsList.size(); k++){
                 os <<  i << " " << this->Users[i].Places[j].label << " " << this->Users[i].Places[j].CheckinsList[k].date << " ";
                 os << this->Users[i].Places[j].CheckinsList[k].period << " " << this->Users[i].Places[j].CheckinsList[k].day << "\n";
           }
        }
    }
}

void Network::printPlacesList(ostream& os){
    os << "Printing places list\n";
    for(index_v i = 0; i < this->Places.size(); ++i ){
        if(this->Places[i].label != -1){
            os << i << " " << this->Places[i].label << " " << this->Places[i].ntotalCheckins << " -> ";
            for(index_v j = 0; j < this->Places[i].UsersList.size(); ++j){
                os  << this->Places[i].UsersList[j] << " ";
            }
            os << "\n";
        }
    }
}

void Network::printPlacesEntropy( ostream& os){
    os << "Printing places entropy list\n";
    for(index_v i = 0; i < this->Places.size(); ++i ){
        if(this->Places[i].label != -1){
            os << i << " " << this->Places[i].label << " " << this->Places[i].ntotalCheckins << "   entropy: " << this->Places[i].entropy << "\n";
        }
    }
}

void Network::printPlacesListOfUsers( ostream& os){
    os << "Printing places list of users\n";
    for(index_v i = 0; i < this->Users.size(); ++i ) {
        for(index_v j = 0; j < Users[i].Places.size(); ++j) {
            os <<  i << " " << this->Users[i].Places[j].label << " " << this->Users[i].Places[j].CheckinsList.size() << "\n";
        }
        os << "----\n";
    }
}

void Network::printPlacesTest( ostream& os){
    for(index_v i = 0; i < TestingPlaces.size(); i++){
        os << TestingPlaces[i].indexUser << " " << TestingPlaces[i].labelPlace << " " << TestingPlaces[i].geoDistToHome << " " << TestingPlaces[i].ncheckins << "\n";
    }
}





////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////     SAMPLING                 ///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

void Network::edgeRandomSubSampling( double percentEdgesToTrain ){
    vector<index_v> edgeIndices = vector<index_v>();
    for( index_v index = 0; index < this->numEdges; index++ ) {
		edgeIndices.push_back( index );
	}

    srand( (unsigned)time(NULL) );
    random_shuffle( edgeIndices.begin(), edgeIndices.end(), randomNumberGeneratorPtr );
    double percentEdgesToTest = (100 - percentEdgesToTrain) / 100.0;
    unsigned int deletionCount = (unsigned int)( percentEdgesToTest * (this->numEdges/2) );
    edgeIndices.resize( deletionCount );
	sort( edgeIndices.begin(), edgeIndices.end() );

	index_v deletionIndex = 0, deletionCountIndex = 0;

	for(index_v i = 0; i < Users.size(); i++){
        for(index_v j = 0; j < Users[i].Neighbors.size(); j++){
            if( deletionCountIndex  == edgeIndices[deletionIndex] ){
                //save the test links index1->index2 and index2->index1
                Link link1;
                link1.indexVertex1 = i;
                link1.indexVertex2 = Users[i].Neighbors[j];
                Link link2;
                link2.indexVertex1 = Users[i].Neighbors[j];
                link2.indexVertex2 = i;

                TestLinks.push_back(link1);
                TestLinks.push_back(link2);

                //delete the links from the current network
                //delete the link formed by index1->index2
                Users[i].Neighbors.erase(remove(Users[i].Neighbors.begin(), Users[i].Neighbors.end(), Users[i].Neighbors[j]), Users[i].Neighbors.end());
                 //delete the link formed by index2->index1
                Users[link1.indexVertex2].Neighbors.erase(remove(Users[link1.indexVertex2].Neighbors.begin(), Users[link1.indexVertex2].Neighbors.end(), i), Users[link1.indexVertex2].Neighbors.end());

                deletionIndex++;
            }
            deletionCountIndex++;
        }
	}

	sortLinkListTest();
}

void Network::edgeRandomSubSamplingBasedOnPeriod( time_v timeToBeginTestingSet ){
    this->numNodesDeleted = 0; //number of nodes deleted during the sampling process
    for(index_v index = 0; index < Users.size(); index++){
        //for consider a user, it should have checkins in training and probe period time and, also, it should be a sampling candidate
        if(isSamplingCandidate(index) && isPeriodTimeCandidate(index, timeToBeginTestingSet)){
            //A)if the user has a number of friends greater than the average degree of network, else the user go directly for training set
            if(getDegree(index) > getAverageDegree()){
                //we make the random sub sampling of the edges of this user

                //1) subsampling process: create a vector to save the vertex indices
                 vector<index_v> edgeIndices = vector<index_v>();
                 for( index_v i = 0; i < Users[index].Neighbors.size(); i++ ) {
                        edgeIndices.push_back( i );
                 }//end of for

                 //2) subsampling process: make the random sampling of vertex indices
                 srand( (unsigned)time(NULL) );
                 random_shuffle( edgeIndices.begin(), edgeIndices.end(), randomNumberGeneratorPtr );

                 //3) subsampling process: take 1/3 of total edges for testing set, the other 2/3 go to training set
                 unsigned int deletionCount = Users[index].Neighbors.size()/3;
                 edgeIndices.resize(deletionCount);
                 sort(edgeIndices.begin(), edgeIndices.end());

                 //4) subsampling process: we save the chosen edges in the testing set and delete it from network (training set)
                 for( index_v i = 0; i < edgeIndices.size(); i++ ) {
                    saveLinkInTestingSet(index, Users[index].Neighbors[edgeIndices[i]]);
                    deleteLinkFromNetwork(index, Users[index].Neighbors[edgeIndices[i]]);
                 }//end of for
            }//end of if the user have the number of friends necessary for a random subsampling process

            //B) we need update the data of the user in the training set
            updateLocationInformationAfterSampling(index, timeToBeginTestingSet);
        } //end of if user is a sampling candidate
        else{
            //user is not consider nor training nor for testing: user will be delete
            deleteUser(index);
            this->numNodesDeleted++;
        }
    }//end of for of Users

    //after that, we update the places vector
    this->numTotalPlaces = 0;
    for(index_v i = 0; i < this->Places.size(); ++i ){
        if(this->Places[i].label != -1){ //we re-count the places
            if(Places[i].ntotalCheckins <= 0){//but some places was deleted
                Places[i].entropy = INF_P;
                Places[i].ntotalCheckins = 0;
                vector<index_v> UL;
                Places[i].UsersList.swap(UL);
                Places[i].label = -1;
                this->numTotalPlaces--;
            }
            else
                this->numTotalPlaces++;
        }
    }

    //finally, we recalculate some information
    if(calculateEntropyOfPlaces == true)
        setEntropyInformation();

    if(calculateHomes == true)
        setHomeLocationInformation();

    if(calculateBoundaries == true)
        setBoundariesInformation();

    updateStructuralStatistics();
}

void Network::sampleSelectedProcessBasedOnPeriod( time_v timeToBeginTestingSet ){
    numNodesDeleted = 0; //number of nodes deleted during the sampling process

     for(index_v index = 0; index < Users.size(); index++){
        //for consider a user, it should have checkins in training and probe period time and, also, it should be a sampling candidate
        if(isSamplingCandidate(index) && isPeriodTimeCandidate(index, timeToBeginTestingSet)){
            ; //do not make nothing because it is a selected sample
        }
        else{
            //user is not consider nor training nor for testing: user will be delete
            deleteUser(index);
            numNodesDeleted++;
        }
    }

    //after that, we update the places vector
    for(index_v i = 0; i < this->Places.size(); ++i ){
        if(Places[i].ntotalCheckins == 0){
            Places[i].entropy = INF_P;
            vector<index_v> UL;
            Places[i].UsersList.swap(UL);
            Places[i].label = -1;
            this->numTotalPlaces--;
        }
    }
}

void Network::setMinNumPlacesToSampling(unsigned int value){
    if(value > 1)
        minNumPlacesToSampling = value;
    else
        minNumPlacesToSampling = 1;
}


/*
    The main idea here is:
    1) we select a percent of users
    2) for each selected user, we take a percent of their visited places to predict. Each selected places cannnot be their home location

*/
void Network::placeRandomSubSampling(double percentUsers, double percentPlacesToPredict){
     //step 0: we need have the home information of all users
    if(calculateHomes == false)
        setHomeLocationInformation();

    //step 1: we select a % of user for our experiments
    vector<index_v> userIndices = vector<index_v>(); //the vector of randomly selected users for predict their places
    for( index_v index = 0; index < Users.size(); index++ ) {
        userIndices.push_back( index );
	}

	srand( (unsigned)time(NULL) );
    random_shuffle( userIndices.begin(), userIndices.end(), randomNumberGeneratorPtr );
    double percentUsersToWork = percentUsers / 100.0;
    unsigned int newSize      = (unsigned int)(percentUsersToWork * Users.size());
    userIndices.resize(newSize);
    sort(userIndices.begin(), userIndices.end());


    //step 2: for each user, we randomly select the places to predict (test places)
    for(index_v ind_user = 0; ind_user < userIndices.size(); ind_user++){
        index_v user = userIndices[ind_user];

        if(getVisitedPlaces(user).size() >= minNumPlacesToSampling){
            //we randomly select the places for testing
            vector<index_p> placesToTestIndices = vector<index_p>();
            for(index_p i = 0; i < getVisitedPlaces(user).size(); i++){
                placesToTestIndices.push_back(i);
            }

            srand( (unsigned)time(NULL) );
            random_shuffle( placesToTestIndices.begin(), placesToTestIndices.end(), randomNumberGeneratorPtr );
            double percentPlacesToTest = percentPlacesToPredict/100.0;
            unsigned int numPlacesToTest = (unsigned int)(percentPlacesToTest * getVisitedPlaces(user).size());
            placesToTestIndices.resize(numPlacesToTest);
            sort(placesToTestIndices.begin(), placesToTestIndices.end());

            //we take out from the network structure the places selected for test and save into other
            vector<Place> tempUserPlaces = getVisitedPlaces(user);

            for(index_p j = 0; j < placesToTestIndices.size(); j++){
                Place placeTesting = tempUserPlaces[placesToTestIndices[j]];

                //if it is not the home location
                if(placeTesting.label != Users[user].labelHomeLocation){
                    //step A: we delete the information of this place from the user
                    deletePlaceFromUser(user, placeTesting.label);

                    //step B: we delete the user information related to this place in the place vector
                    deleteVisitantFromPlacesList(placeTesting.label, user, placeTesting.CheckinsList.size());

                    //step C: we save this place for testing in our vector of testing's places
                    PlaceTest tplace;
                    tplace.indexUser     = user;
                    tplace.labelPlace    = placeTesting.label;
                    tplace.geoDistToHome = getGeographicDistanceAtKm(Users[user].labelHomeLocation, placeTesting.label);
                    tplace.ncheckins     = placeTesting.CheckinsList.size();;
                    TestingPlaces.push_back(tplace);

                    //step D: we mark the user as a user
                    Users[user].active = true;
                }//end of if
            }//end of for of places for testing
        } //end of if of validation of min num of places to sampling
    }//end of for of all the users
}

void Network::deletePlaceFromUser(index_v indexUser, long labelPlace){
    //the list of places of each user is ordered by label, so it is possible use a binary search
    index_p first = 0, last = Users[indexUser].Places.size() - 1, middle = (first+last)/2;
    index_p indexPlace =  Users[indexUser].Places.size();

    while (first <= last) {
         if (Users[indexUser].Places[middle].label < labelPlace)
            first = middle + 1;
         else if (Users[indexUser].Places[middle].label == labelPlace) {
                indexPlace = middle;
                first = last + 1;
         }
         else
            last = middle - 1;

        middle = (first + last)/2;
    }

    if(indexPlace < Users[indexUser].Places.size()){
        Users[indexUser].Places.erase(Users[indexUser].Places.begin() + indexPlace);
        Users[indexUser].ntotalCheckins -= Users[indexUser].Places[middle].CheckinsList.size();
    }
}


////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////     SORT                 ///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

void Network::sortNeighborsOfUsers(){
   if(isWeighted == false){ //we will sort just the neighbors
     for(index_v i = 0; i < Users.size(); i++)
        sort(Users[i].Neighbors.begin(), Users[i].Neighbors.end());
   }
   /*else{ //if is weighted, we will sort neighbors and their weights, it is more slow
        for(index_v i = 0; i < Users.size(); i++)
            sortWeightedLinks(0, Users[i].Neighbors.size(), Users[i].Neighbors, Users[i].Weights);
   }*/
}

void Network::sortGroupsOfUsers(){
   for(index_v i = 0; i < Users.size(); i++){
        sort(Users[i].Groups.begin(), Users[i].Groups.end());
   }
}

void Network::sortLinkListTest(){
    sort(TestLinks.begin(), TestLinks.end(), linkCompareByNode1() );

    index_v indAct = 0;
    for(index_v ind = 1; ind < TestLinks.size(); ind++){
        if( TestLinks[ind].indexVertex1 != TestLinks[indAct].indexVertex1){
            sort(TestLinks.begin() + indAct, TestLinks.begin() + ind, linkCompareByNode2() );
            indAct = ind;
        }
    }
    sort(TestLinks.begin() + indAct, TestLinks.end(), linkCompareByNode2() );
}




////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////   STRUCTURAL  PROPERTIES                 ////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

bool Network::hasEdge( index_v indexVertex1, index_v indexVertex2 ) const{
    long first = 0, last = Users[indexVertex1].Neighbors.size() - 1, middle = (first+last)/2;

    while (first <= last) {
         if (Users[indexVertex1].Neighbors[middle] < indexVertex2)
            first = middle + 1;
         else if (Users[indexVertex1].Neighbors[middle] == indexVertex2) {
             return true;
         }
         else
            last = middle - 1;


        middle = (first + last)/2;
    }
    return false;
}

double Network::getWeight(index_v indexVertex1, index_v indexVertex2 ) const{
    if(isWeighted == false)
        return 1;

    long first = 0, last = Users[indexVertex1].Neighbors.size() - 1, middle = (first+last)/2;

    while (first <= last) {
         if (Users[indexVertex1].Neighbors[middle] < indexVertex2)
            first = middle + 1;
         else if (Users[indexVertex1].Neighbors[middle] == indexVertex2) {
             return Users[indexVertex1].Weights[middle];
         }
         else
            last = middle - 1;


        middle = (first + last)/2;
    }
    return 0.0;
}

double Network::getStrengthOfNode( index_v indexVertex ) const{
    double strength = 0.0;

    if(isWeighted == true){
        for(unsigned int i = 0; i < Users[indexVertex].Neighbors.size(); i++)
            strength += Users[indexVertex].Weights[i];
    }
    else{
        for(unsigned int i = 0; i < Users[indexVertex].Neighbors.size(); i++)
            strength++;
    }

    return strength;
}

unsigned int Network::getNumUsers() const{
    return this->numNodes;
}

unsigned int Network::getDegree( index_v indexVertex ) const{
    return Users[indexVertex].Neighbors.size();
}

unsigned int Network::getDiameter(){
    if(maxPathLength <= 0){
       getAveragePathLenght();
    }
    return maxPathLength;
}

unsigned long Network::getNumEdgesFriendshipGraph(){
    return numEdges/2;
}

long Network::getMaxNumberFriendsPerUser(){
    if(maxNumFriendsPerUser == INV_LONG){
        unsigned long maxTemp = 0;
        for(index_v i = 0; i < Users.size(); i++){
            if( Users[i].Neighbors.size() > maxTemp )
                maxTemp = Users[i].Neighbors.size();
        }
        maxNumFriendsPerUser = maxTemp;
    }

    return maxNumFriendsPerUser;
}

double Network::getAverageFriendsPerUser(){
    return floor((double) numEdges / Users.size());
}

double Network::getAverageDegree() const{
    double sum = 0.0;
    for( index_v indexVertex = 0; indexVertex < Users.size(); indexVertex++ ) {
        if( Users[indexVertex].Neighbors.size() > 0 )
            sum += getDegree(indexVertex);
    }
    return sum / Users.size();
}

double Network::getClusteringCoefficient( index_v indexVertex ) const {
    if(Users[indexVertex].Neighbors.size() < 2)
        return 0.0;

    unsigned int edgeCount = 0;
    for(index_v index1 = 0; index1 < Users[indexVertex].Neighbors.size(); index1++){
        const long neighbor1 = Users[indexVertex].Neighbors[index1];
        for(index_v index2 = index1 + 1; index2 < Users[indexVertex].Neighbors.size(); index2++){
            const long neighbor2 = Users[indexVertex].Neighbors[index2];
            bool edge1 = hasEdge( neighbor1, neighbor2 );
            bool edge2 = hasEdge( neighbor2, neighbor1 );
            edgeCount += (unsigned int)edge1 + (unsigned int)edge2;
        }
    }
    return (double)edgeCount / (Users[indexVertex].Neighbors.size()*(Users[indexVertex].Neighbors.size()-1));
}

double Network::getAverageClusteringCoefficient() const{
    double sum = 0.0;
    for( index_v indexVertex = 0; indexVertex < Users.size(); indexVertex++ ) {
		sum += getClusteringCoefficient( indexVertex );
	}
	return sum / Users.size();
}

double Network::getAverageAssortativityCoefficient() const {
    vector<index_v> x = vector<index_v>();
	vector<index_v> y = vector<index_v>();

	for(index_v i = 0; i < Users.size(); i++){
	    for(index_v j = 0; j < Users[i].Neighbors.size(); j++){
            x.push_back(Users[i].Neighbors.size());
            y.push_back(getDegree(Users[i].Neighbors[j]));
	    }
	}
	return sampleCorrelationCoefficient( x, y );
}

double Network::getHeterogeneity()const{
    double num = 0, den = 0;
    for( index_v vertex = 0; vertex < Users.size(); vertex++ ) {
        double degree = getDegree(vertex);
        num += (degree * degree);
        den += degree;
    }

    double k1 = num / Users.size();
    double k2 = (den / Users.size()) *  (den / Users.size());
    return k1 / k2;
}

double Network::getAveragePathLenght(){
    double sum = 0.0;
    maxPathLength = 0;
    vector<unsigned int> lengths = getShortestPathLengths();
    for( index_v i = 0; i < lengths.size(); i++ ){
        sum = sum + lengths[i];

        if( lengths[i] > maxPathLength )
            maxPathLength = lengths[i];
    }
    return sum * (2.0 / (Users.size() * (Users.size() -1 ) ) );
}

//return the sorted (by id) list of friends
vector<index_v> Network::getNeighbors( index_v indexVertex )const{
    return Users[indexVertex].Neighbors;
}

vector<index_v> Network::getNeighbors( index_v indexVertex, unsigned int degree )const{
    vector<bool> found = vector<bool>( Users.size() );
    vector<index_v>* oldSearch = new vector<index_v>();
	vector<index_v>* newSearch = new vector<index_v>();

	found.at( indexVertex ) = true;
	newSearch->push_back( indexVertex );

	for( unsigned int currentDegree = 0; currentDegree < degree; currentDegree++ ){
        delete oldSearch;
		oldSearch = newSearch;
		newSearch = new vector<index_v>();

		for( vector<index_v>::const_iterator searchIterator = oldSearch->begin(); searchIterator != oldSearch->end(); searchIterator++ ){
		    const index_v searchVertex = *searchIterator;

		    for(index_v j = 0; j < Users[searchVertex].Neighbors.size(); j++){
                index_v indexNeighbor = Users[searchVertex].Neighbors[j];

                if( !found.at( indexNeighbor ) ) {
                    found.at( indexNeighbor ) = true;
                    newSearch->push_back( indexNeighbor );
                }
		    }
		}
	}

	vector<index_v> results = vector<index_v>( newSearch->size() );
	for( index_v searchVertex = 0; searchVertex < newSearch->size(); searchVertex++ ) {
		results.at( searchVertex ) = newSearch->at( searchVertex );
	}

	delete newSearch;
	delete oldSearch;

	return results;
}

vector<index_v>  Network::getShortestPathLengths() const{
    vector<index_v> lengths;
    for( index_v i = 0; i < Users.size(); i++ ){
        vector<unsigned int> vertexLengths = getPathLengths( i );
        lengths.insert( lengths.end(), vertexLengths.begin(), vertexLengths.end() );
    }
    return lengths;
}

vector<index_v> Network::getPathLengths( index_v vertex ) const {
	vector<index_v> found = vector<unsigned int>( Users.size() );
	vector<index_v> oldSearch;
	found.at( vertex ) = 0;
	oldSearch.push_back( vertex );
	unsigned int currentDegree = 1;
	while( !oldSearch.empty() ) {
        vector<index_v> newSearch;
        for( index_v i = 0; i < oldSearch.size(); i++ ) {
			const index_v searchVertex = oldSearch[i];
			vector<index_v> neighbors = getNeighbors(searchVertex);
            for(index_v j = 0; j < neighbors.size(); j++){
                const index_v neighborVertex = neighbors[j];
                if( neighborVertex != vertex && found.at( neighborVertex ) == 0 ) {
                    found.at( neighborVertex ) = currentDegree;
                    newSearch.push_back( neighbors[j] );
                }
            }
        }
        ++currentDegree;
        oldSearch = newSearch;
    } //end of while

    return found;
}

vector<index_v> Network::intersect( vector<index_v> neighborsV1, vector<index_v> neighborsV2)const{
    vector<index_v> intesectList(neighborsV1.size() + neighborsV2.size());
    vector<index_v>::iterator it;
    it = set_intersection( neighborsV1.begin(), neighborsV1.end(), neighborsV2.begin(), neighborsV2.end(), intesectList.begin());
    intesectList.resize(it - intesectList.begin());
    return intesectList;
}

vector<long> Network::intersect( vector<long> neighborsV1, vector<long> neighborsV2)const{
    vector<long> intesectList(neighborsV1.size() + neighborsV2.size());
    vector<long>::iterator it;
    it = set_intersection(neighborsV1.begin(), neighborsV1.end(), neighborsV2.begin(), neighborsV2.end(), intesectList.begin());
    intesectList.resize(it - intesectList.begin());
    return intesectList;
}

vector<index_v> Network::junction(vector<index_v> neighborsV1, vector<index_v> neighborsV2)const{
    vector<index_v> unionList(neighborsV1.size() + neighborsV2.size());
    vector<index_v>::iterator it;
    it = set_union(neighborsV1.begin(), neighborsV1.end(), neighborsV2.begin(), neighborsV2.end(), unionList.begin());
    unionList.resize(it - unionList.begin());
    return unionList;
}

vector<index_p> Network::difference(vector<index_p> placesV1, vector<index_p> placesV2)const{
    vector<index_p> diffList(placesV1.size() + placesV2.size());
    vector<index_p>::iterator it;
    it = set_difference(placesV1.begin(), placesV1.end(), placesV2.begin(), placesV2.end(), diffList.begin());
    diffList.resize(it - diffList.begin());
    return diffList;
}





////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////   GEO-LOCATION  PROPERTIES                   ////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////



void Network::setHomeLocationInformation(){
    calculateHomes = true;
    for(index_v indexVertex = 0; indexVertex < Users.size(); indexVertex++){
        unsigned int ncheckins  = 0;
        unsigned int indexPlace = 0;
        long homeLabel          = -1;

        for(index_v i = 0; i < Users[indexVertex].Places.size(); i++){
            if(Users[indexVertex].Places[i].CheckinsList.size() != 0){
                if(Users[indexVertex].Places[i].CheckinsList.size() > ncheckins){
                    ncheckins  = Users[indexVertex].Places[i].CheckinsList.size();
                    homeLabel  = Users[indexVertex].Places[i].label;
                    indexPlace = i;
                }
                else{
                    //among places with the same number of check-ins we consider the more old as the home
                    if(Users[indexVertex].Places[i].CheckinsList.size() == ncheckins){
                        if(Users[indexVertex].Places[i].CheckinsList[0].day < Users[indexVertex].Places[indexPlace].CheckinsList[0].day){//checkin list is ordered by day
                            homeLabel  = Users[indexVertex].Places[i].label;
                            indexPlace = i;
                        }
                    }
                }
            }
        }

        Users[indexVertex].labelHomeLocation = homeLabel;
    }
}

void Network::updateStructuralStatistics(){
    maxNumCheckins = 0;

    //set the new value of the max number of check-ins from all users
    for(index_v indexVertex = 0; indexVertex < Users.size(); indexVertex++){
        if(Users[indexVertex].ntotalCheckins > maxNumCheckins)
                maxNumCheckins = Users[indexVertex].ntotalCheckins;
    }
}

void Network::setEntropyInformation(){
    calculateEntropyOfPlaces = true;
    double maxE = INF_P * -1;

    for(index_p i = 0; i < Places.size(); i++){
        if(Places[i].entropy == INF_P){
            setEntropyOfPlace(i);

            if(Places[i].entropy > maxE)
                maxE = Places[i].entropy;
        }
    }
    maxEntropyValue = maxE;
}

void Network::setBoundariesInformation(){
    unsigned int tvalueUp = getAverageCheckInPerUser()/2;
    this->calculateBoundaries = true;
    //for each user
    for(index_v u = 0; u < Users.size(); u++){
        //we select the places visited by the user
        vector<Place> vPlacesAux;
        for(index_p i = 0; i < Users[u].Places.size(); i++){
            Place place;
            place.label = Users[u].Places[i].label;
            place.ntotalCheckins = Users[u].Places[i].CheckinsList.size();
            vPlacesAux.push_back(place);
        }//end of for

        //sort the vector of places by check-ins frequency
        sort(vPlacesAux.begin(), vPlacesAux.end(), placeComparedByNumberCheckins());

        //we take only the vector of frequencies
        vector<unsigned int> vAuxFreq;
        for(unsigned int j = 0; j < vPlacesAux.size(); j++){
            vAuxFreq.push_back(vPlacesAux[j].ntotalCheckins);
        }

        //we take the boundaries
        unsigned int tvalueLow = Users[u].ntotalCheckins/3;
        vector<unsigned int>::iterator low, up;
        low = lower_bound (vAuxFreq.begin(), vAuxFreq.end(), tvalueLow);
        up  = upper_bound (vAuxFreq.begin(), vAuxFreq.end(), tvalueUp);

        //set the boundaries
        unsigned int ilow = low - vAuxFreq.begin();
        unsigned int iup  = up - vAuxFreq.begin();

        if(ilow >= 0 && ilow < vAuxFreq.size())
            Users[u].labelInfimum  = vPlacesAux[ilow].label;
        else
            Users[u].labelInfimum  = -1;


        if(iup >= 0 && iup < vAuxFreq.size())
            Users[u].labelSupremum = vPlacesAux[iup].label;
        else
            Users[u].labelSupremum = -1;
    }
}

time_v Network::monthToInt( const char* date ){
    char time[25] = "";
    strcpy(time, date);
    int year  = atoi(strtok (time,"-"));
    int month = atoi(strtok (NULL,"-"));
    return (time_v) month + (year - YEAR_BASE)*12;
}

time_v Network::dayToInt( const char* date ){
    char time[25] = "";
    strcpy(time, date);
    int year  = atoi(strtok (time,"-T:"));
    int month = atoi(strtok (NULL,"-T:"));
    int day   = atoi(strtok (NULL,"-T:"));

    //cout << "\n year = " << year << " month = " << month << " day = "<< day;

    bool flagBi = false;
    int totalDaysByYear;
    if(year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) //bisiest
        flagBi = true;

    totalDaysByYear = (year - 1 - YEAR_BASE) * 365;

    //cout << "\n total days by year = " << totalDaysByYear;

    vector<int> vMes;
    for(unsigned int m = 0; m < 12; m++)
        vMes.push_back(30);
    vMes[0]  = 31; //jan
    vMes[2]  = 31; //march
    vMes[4]  = 31; //may
    vMes[6]  = 31; //july
    vMes[7]  = 31; //Aug
    vMes[9]  = 31; //Oct
    vMes[11] = 31; //dez

    if( month == 2 && flagBi == true )
        vMes[1] = 29;
    else if( month == 2 && flagBi == false )
        vMes[1] = 28;
    //cout <<" b ";

    time_v totalDaysByMonth = 0;
    if(month > 1){
        int indexMonth = month - 1;
        if( indexMonth >= 0 ){
            for( int m = 0; m <= indexMonth; m++ )
                totalDaysByMonth += vMes[m];
        }
    }

    //cout << "\n total days by month = " << totalDaysByMonth;
    return  totalDaysByYear + totalDaysByMonth + day;
}

vector<Place> Network::getVisitedPlaces( index_v indexVertex) const{
    return Users[indexVertex].Places;
}

vector<Place> Network::getPlacesVisitedByFriends(index_v indexVertex) const{
    vector<Place> vPlacesVisitedByFriends;

    //we will take all the places visited by each friend without repetitions of places
    for(index_v i = 0; i < Users[indexVertex].Neighbors.size(); i++){
        index_v indexFriend = Users[indexVertex].Neighbors[i];

        for(index_p j = 0; j < Users[indexFriend].Places.size(); j++){
            Place place;
            place.label = Users[indexFriend].Places[j].label;

            //we linear sear in the vector of visited places, we expect that this search has not very cost but we expect also improve it
            bool flag = false;
            index_p indexVisitedPlace = 0;
            for(index_p k = 0; k < vPlacesVisitedByFriends.size(); k++){
                if(vPlacesVisitedByFriends[k].label == place.label){
                    indexVisitedPlace = k;
                    k = vPlacesVisitedByFriends.size();
                    flag = true;
                }
            }//end of for of linear search


            if(flag == false){//here we have a new place to add to our vector
                place.UsersList.push_back(indexFriend);
                vPlacesVisitedByFriends.push_back(place);
            }
            else{ //here, we have a place in our list, so we only update the list of users
                vPlacesVisitedByFriends[indexVisitedPlace].UsersList.push_back(indexFriend);
            }

        } //end of for for visited places of each friend
    }//end of for for friends of user

    return vPlacesVisitedByFriends; //this vector contains only the label of each place and the list of users that visited such place
}

//we consider as potential places to visit all the places visited by friends and friends of friends was not visited by user
vector<index_p> Network::getPotentialPlacesToVisit( index_v indexVertex ) const{
    vector<index_p> vPotentialPlaces;

    //for all friends of user
    for(index_v i = 0; i < Users[indexVertex].Neighbors.size(); i++){
        index_v indexFriend = Users[indexVertex].Neighbors[i];

        //step 1: we take all the visited places of each friend
        for(index_p j = 0; j < Users[indexFriend].Places.size(); j++)
            vPotentialPlaces.push_back(Users[indexFriend].Places[j].label);

        //step 2: for each friend, we take the places of its friends
        for(index_v k = 0; k < Users[indexFriend].Neighbors.size(); k++){
            index_v indexFriendOfFriend = Users[indexFriend].Neighbors[k];

            for(index_p m = 0; m < Users[indexFriendOfFriend].Places.size(); m++)
                vPotentialPlaces.push_back(Users[indexFriendOfFriend].Places[m].label);
        }
    }

    //we sort the vector of places and delete the duplicates
    sort(vPotentialPlaces.begin(), vPotentialPlaces.end());
    vPotentialPlaces.erase(unique(vPotentialPlaces.begin(), vPotentialPlaces.end()), vPotentialPlaces.end()); //remove duplicates

    //finally, we remove the places visited by the user
    vector<index_p> vPlacesOfUser;
    for(index_p i = 0; i < Users[indexVertex].Places.size(); i++)
        vPlacesOfUser.push_back(Users[indexVertex].Places[i].label);

    return difference(vPotentialPlaces, vPlacesOfUser);
}

vector<index_p> Network::getIntersectPlaces(index_v indexVertex1, index_v  indexVertex2) const{
    vector<index_p> vCommonP;
    for(index_v i = 0; i < Users[indexVertex1].Places.size(); i++){
        for(index_v j = 0; j < Users[indexVertex2].Places.size(); j++){
            if( Users[indexVertex1].Places[i].label < Users[indexVertex2].Places[j].label ||
                Users[indexVertex1].Places[i].label > Users[indexVertex2].Places[Users[indexVertex2].Places.size()-1].label ){ //we limit the search space
                    j = Users[indexVertex2].Places.size();
            }
            else{
                if(Users[indexVertex1].Places[i].label == Users[indexVertex2].Places[j].label)
                    vCommonP.push_back(Users[indexVertex1].Places[i].label);
            }
        }
    }
    return vCommonP;
}

//we just we know if exist at least one element making intersection
bool Network::existIntersectPlaces(index_v indexVertex1, index_v  indexVertex2) const{
    for(index_v i = 0; i < Users[indexVertex1].Places.size(); i++){
        for(index_v j = 0; j < Users[indexVertex2].Places.size(); j++){
            if( Users[indexVertex1].Places[i].label < Users[indexVertex2].Places[j].label ||
                Users[indexVertex1].Places[i].label > Users[indexVertex2].Places[Users[indexVertex2].Places.size()-1].label ){ //we limit the search space
                    j = Users[indexVertex2].Places.size();
            }
            else{
                if(Users[indexVertex1].Places[i].label == Users[indexVertex2].Places[j].label)
                    return true;
            }
        }
    }
    return false;
}

unsigned int Network::getTotalCheckinsAtPlace(index_p indexPlace) const{
    return Places[indexPlace].ntotalCheckins;
}

unsigned int Network::getTotalCheckinsUser(index_v indexUser) const{
    return Users[indexUser].ntotalCheckins;
}

vector<Checkin> Network::getCheckinsUserAtPlace(index_v indexVertex, long indexPlace) const{
    long left = 0;
    long right = Users[indexVertex].Places.size() - 1;

    while(left <= right){
        long middle = (left + right) / 2;

        if( Users[indexVertex].Places[middle].label == indexPlace )
            return Users[indexVertex].Places[middle].CheckinsList;
        else if( Users[indexVertex].Places[middle].label > indexPlace )
                right = middle - 1;
        else
                left = middle + 1;
    }

    vector<Checkin> lEmpty;
    return lEmpty;
}

vector<Checkin> Network::getCheckinsUser(index_v indexVertex) const{
    vector<Checkin> vAllCheckins;
    for(index_v i = 0; i < Users[indexVertex].Places.size(); i++){
        vAllCheckins.insert(vAllCheckins.end(), Users[indexVertex].Places[i].CheckinsList.begin(), Users[indexVertex].Places[i].CheckinsList.end());
    }
    return vAllCheckins;
}

vector<index_p> Network::getIndexPlacesOfAllCheckinsUser(index_v indexVertex) const{
    vector<index_p> vAllIndexPlaces;
    for(index_v i = 0; i < Users[indexVertex].Places.size(); i++){
        for(index_v j = 0; j < Users[indexVertex].Places[i].CheckinsList.size(); j++){
            vAllIndexPlaces.push_back(Users[indexVertex].Places[i].label);
        }
    }

    return vAllIndexPlaces;
}

void Network::setEntropyOfPlace( index_p indexPlace ){
    double Cpk = getTotalCheckinsAtPlace(indexPlace);
    if(Cpk == 0)
        Places[indexPlace].entropy = 0.0;
    else{
        double entropy = 0.0;
        for(index_v i = 0; i < Places[indexPlace].UsersList.size(); i++){
            unsigned int userid = Places[indexPlace].UsersList[i];
            double cik = getCheckinsUserAtPlace( userid, indexPlace).size();
            double qik = cik/Cpk;

            if(qik != 0)
                entropy += qik * log10(qik);
        }

        if(entropy == 0)
            Places[indexPlace].entropy = 0.0;
        else
            Places[indexPlace].entropy = -1 * entropy;
    }
}

//return the sorted (by id) list of visitors of a place
vector<index_v> Network::getVisitors(index_p indexPlace) const{
    return Places[indexPlace].UsersList;
}

double Network::getGeographicDistance(double latitudA, double longitudA, double latitudB, double longitudB) const{
    double latitudeArc  = (latitudB - latitudA)   * DEG_TO_RAD;
    double longitudeArc = (longitudB - longitudA) * DEG_TO_RAD;
    double latitudeH    = sin(latitudeArc  * 0.5);
    double lontitudeH   = sin(longitudeArc * 0.5);
    latitudeH  *= latitudeH;
    lontitudeH *= lontitudeH;
    double c = cos(latitudA * DEG_TO_RAD) * cos(latitudB * DEG_TO_RAD);

    //return the distance in meters
    return (EARTH_RADIUS_IN_METERS * 2.0 * asin(sqrt(latitudeH + c * lontitudeH)));
}

double Network::getGeographicDistance(index_p indexPlace1, index_p indexPlace2) const{
    return getGeographicDistance(Places[indexPlace1].latitude, Places[indexPlace1].longitude, Places[indexPlace2].latitude, Places[indexPlace2].longitude);
}

double Network::getGeographicDistanceAtMiles(index_p indexPlace1, index_p indexPlace2) const{
    return 0.000621371 * getGeographicDistance(Places[indexPlace1].latitude, Places[indexPlace1].longitude, Places[indexPlace2].latitude, Places[indexPlace2].longitude);
}

double Network::getGeographicDistanceAtKm(index_p indexPlace1, index_p indexPlace2) const{
    return 0.001 * getGeographicDistance(Places[indexPlace1].latitude, Places[indexPlace1].longitude, Places[indexPlace2].latitude, Places[indexPlace2].longitude);
}

double Network::getEntropyOfPlace( index_p indexPlace ) const{
    return Places[indexPlace].entropy;
}

double Network::getLocationDiversity( index_p indexPlace ) const{
    return exp(Places[indexPlace].entropy);
 }

long Network::getHomeLocation(index_v indexVertex) const{
    return Users[indexVertex].labelHomeLocation;
}

long Network::getInfimumPlace(index_v indexVertex) const{
    return Users[indexVertex].labelInfimum;
}

long Network::getSupremumPlace(index_v indexVertex) const{
    return Users[indexVertex].labelSupremum;
}

double Network::getAdjustedGeographicDistance(index_p labelPlace1, index_p labelPlace2) const{
    double maxlocdiv  = getLocationDiversity(labelPlace1);
    double locdiv2    = getLocationDiversity(labelPlace2);

    if(locdiv2 > maxlocdiv)
        maxlocdiv = locdiv2;

    return maxlocdiv * getGeographicDistance(labelPlace1, labelPlace2);
}

unsigned int Network::getTotalNumCheckIns() const{
    return this->numCheckIn;
}

unsigned int Network::getNumNodesWithCheckIn() const{
    return this->numNodesWithCheckIn;
}

unsigned int Network::getNumberTotalPlaces() const{
    return this->numTotalPlaces;
}

double Network::getAverageCheckInPerUser() const{
   return floor((1.0 * this->numCheckIn)/this->numNodesWithCheckIn);
}

double Network::getAverageCheckInsPerLocation() const{
    return floor((1.0 * this->numCheckIn)/this->numTotalPlaces);
}

double Network::getAverageEntropyOfPlaces() const{
    double esum = 0;
    for(index_p i = 0; i < Places.size(); i++){
        if(Places[i].label != -1)
            esum += Places[i].entropy;
    }

    return esum/this->numTotalPlaces;
}

double Network::getMinEntropyOfPlaces() const{
    double minE = INF_P;

    for(index_p i = 0; i < Places.size(); i++){
        if(Places[i].label != -1){
            if(Places[i].entropy < minE)
                minE = Places[i].entropy;
        }
    }

    return minE;
}

double Network::getMaxEntropyOfPlaces() const{
     return maxEntropyValue;
}

double Network::getMaxDiversityOfPlaces() const{
    return exp(maxEntropyValue);
}

void Network::setMinDegreeToSampling(unsigned int value){
    minDegreeToSampling = value;
}

void Network::setMinNumCheckinsToSampling(unsigned int value){
    minNumCheckinsToSampling = value;
}

unsigned int Network::getMinDegreeToSampling() const{
    return minDegreeToSampling;
}

unsigned int Network::getMinNumCheckinsToSampling() const{
    return minNumCheckinsToSampling;
}

bool Network::isSamplingCandidate(index_v indexUser){
    if(getDegree(indexUser) >= getMinDegreeToSampling() &&  getTotalCheckinsUser(indexUser) >= getMinNumCheckinsToSampling() )
        return true;

    return false;
}

bool Network::isPeriodTimeCandidate(index_v indexUser, time_v timeToBeginTestingSet){
    for(index_v i = 0; i < Users[indexUser].Places.size(); i++){
        if( getSignalPeriodTimeCandidate(indexUser, i, timeToBeginTestingSet) == IS_IN_BOTH_PERIODS ){
            return true;
        }//end of if
    }//end of foe

    return false;
}

unsigned int Network::getSignalPeriodTimeCandidate(index_v indexUser, index_p indexPlace, time_v timeToBeginTestingSet){
    unsigned int signal   = IS_NOT_CANDIDATE;
    unsigned int sizeList = Users[indexUser].Places[indexPlace].CheckinsList.size();

    if(sizeList > 0){
        if(Users[indexUser].Places[indexPlace].CheckinsList[0].period < timeToBeginTestingSet)
            signal = IS_IN_TRAINING;

        if(Users[indexUser].Places[indexPlace].CheckinsList[sizeList-1].period >= timeToBeginTestingSet){
                if(signal == IS_IN_TRAINING)
                    return IS_IN_BOTH_PERIODS;
                else
                    return IS_IN_TESTING;
        }
    }

    return signal;
}

void Network::saveLinkInTestingSet(index_v index1, index_v index2){
    Link link1;
    link1.indexVertex1 = index1;
    link1.indexVertex2 = index2;

    Link link2;
    link2.indexVertex1 = index2;
    link2.indexVertex2 = index1;

    TestLinks.push_back( link1 );
    TestLinks.push_back( link2 );
}

void Network::deleteLinkFromNetwork(index_v index1, index_v index2){
    Users[index1].Neighbors.erase(remove( Users[index1].Neighbors.begin(), Users[index1].Neighbors.end(), index2 ), Users[index1].Neighbors.end() );
    Users[index2].Neighbors.erase(remove( Users[index2].Neighbors.begin(), Users[index2].Neighbors.end(), index1 ), Users[index2].Neighbors.end() );
    this->numEdges -= 2;
}

void Network::updateLocationInformationAfterSampling(index_v indexUser, time_v timeToBeginTestingSet){
    Users[indexUser].ntotalCheckins = 0;
    //we delete the check-ins make by the user in a time >= timeToBeginTestingSet because these check-ins are int testing set period
    for(index_p indexPlace = 0; indexPlace < Users[indexUser].Places.size(); indexPlace++){
        unsigned int signal = getSignalPeriodTimeCandidate(indexUser, indexPlace, timeToBeginTestingSet);
         //if the user only has checkins in training period in this place, do not make nothing
        //if the user only has checkins in testing period in this place, we remove all checkins from vector
        //if the user has checkins in both training and testing periods, we keep only the checkins in training period
        if(signal != IS_IN_TRAINING){
            vector<Checkin> newCheckinVector;
            unsigned int ncheckins_intestingperiod = Users[indexUser].Places[indexPlace].CheckinsList.size();

            if(signal == IS_IN_TESTING){
                //we remove the user as visitant from the places vector
                deleteVisitantFromPlacesList(Users[indexUser].Places[indexPlace].label, indexUser, ncheckins_intestingperiod);
            }
            else{
                if(signal == IS_IN_BOTH_PERIODS){
                    //select the checkins in training period
                    for(index_p i = 0; i < Users[indexUser].Places[indexPlace].CheckinsList.size(); i++){
                        if(Users[indexUser].Places[indexPlace].CheckinsList[i].period < timeToBeginTestingSet){
                            newCheckinVector.push_back(Users[indexUser].Places[indexPlace].CheckinsList[i]);
                        }
                    }//end of for of checkins vector


                    ncheckins_intestingperiod -= newCheckinVector.size();
                    Places[Users[indexUser].Places[indexPlace].label].ntotalCheckins -= ncheckins_intestingperiod;

                }//end of if it is in both periods
            }

            //update the vector of checkins
            Users[indexUser].Places[indexPlace].CheckinsList.swap(newCheckinVector);

            //update the count of number of check-ins in the network
            this->numCheckIn -= ncheckins_intestingperiod;

            //we set the place visited for recalculate its entropy
            Places[Users[indexUser].Places[indexPlace].label].entropy = INF_P;
        }

        //update the count of number of check-ins of user
        Users[indexUser].ntotalCheckins += Users[indexUser].Places[indexPlace].CheckinsList.size();
    }//end of for of places vector
}

void Network::deleteVisitantFromPlacesList(index_p labelPlace, index_v indexVisitant, unsigned int nmadecheckins){
   Places[labelPlace].UsersList.erase(remove(Places[labelPlace].UsersList.begin(), Places[labelPlace].UsersList.end(), indexVisitant), Places[labelPlace].UsersList.end() );
   Places[labelPlace].ntotalCheckins -= nmadecheckins;
}

void Network::setTestLinksList( vector<Link> linksList ){
    TestLinks.swap(linksList);
}

void Network::setLocationInformation(){
    //First, we organize check-in information for each user
    for(index_v i = 0; i < Users.size(); i++){
        if(Users[i].Places.size() > 0){
            //we sort by place label
            sort(Users[i].Places.begin(), Users[i].Places.end(), placeComparedByLabel());

            vector<Place> newPlaces;
            newPlaces.push_back(Users[i].Places[0]);

            for(unsigned int j = 1; j < Users[i].Places.size(); j++){
                unsigned int last = newPlaces.size() - 1;
                if(Users[i].Places[j].label == newPlaces[last].label){
                    newPlaces[last].CheckinsList.push_back(Users[i].Places[j].CheckinsList[0]); //because we have only one element in check-in list
                }
                else{
                    newPlaces.push_back(Users[i].Places[j]);
                }
            }

            //we update the places information of the user
            Users[i].Places.swap(newPlaces);

            //we sort the check-ins of each place visited by date and calculate the total number of check-ins
            for(unsigned int j = 0; j < Users[i].Places.size(); j++){
                sort(Users[i].Places[j].CheckinsList.begin(), Users[i].Places[j].CheckinsList.end(), checkinComparedByDayDate());
                Users[i].ntotalCheckins += Users[i].Places[j].CheckinsList.size();
            }

            //calculate the max number of check-ins from all users
            if(Users[i].ntotalCheckins > maxNumCheckins)
                maxNumCheckins = Users[i].ntotalCheckins;
        }
    }

    //Second, we organize information about distinct locations in the network
    this->numTotalPlaces = 0;
    for(index_v i = 0; i < this->Places.size(); ++i ){
        if(this->Places[i].label != -1){
            //The UsersList set is naturally ordered from a requirement of the checkins file
            //so, we just will count the total number of visits of users (representing the number of check-ins)
            //and delete the number of duplicate users
            Places[i].ntotalCheckins = Places[i].UsersList.size();
            Places[i].entropy = INF_P;
            Places[i].UsersList.erase( unique(Places[i].UsersList.begin() , Places[i].UsersList.end()), Places[i].UsersList.end());//remove duplicates

            //also, we count it as valid visited place
            this->numTotalPlaces++;
        }
    }
}

void Network::deleteUser(index_v indexUser){
    //delete from the places vector all information of the user
    for(index_p indexPlace = 0; indexPlace < Users[indexUser].Places.size(); indexPlace++){
        //we remove the user as visitant from the places vector
        unsigned int ncheckins_ofuser = Users[indexUser].Places[indexPlace].CheckinsList.size();
        deleteVisitantFromPlacesList(Users[indexUser].Places[indexPlace].label, indexUser, ncheckins_ofuser);
        Places[Users[indexUser].Places[indexPlace].label].entropy = INF_P;
    }

    //update the count of number of check-ins of user of user checkin in the network
    if(Users[indexUser].ntotalCheckins > 0){
        this->numNodesWithCheckIn--;
        this->numCheckIn -= Users[indexUser].ntotalCheckins;
        Users[indexUser].ntotalCheckins = 0;
    }

    for(index_v i = 0; i < Users[indexUser].Neighbors.size(); i++){
        deleteLinkFromNetwork(indexUser, Users[indexUser].Neighbors[i]);
    }

    //clean another vectors
    vector<double>   W;
    vector<index_g>  G;
    vector<Place>    P;
    Users[indexUser].Weights.swap(W);
    Users[indexUser].Groups.swap(G);
    Users[indexUser].Places.swap(P);
    Users[indexUser].labelHomeLocation = INV_LONG;
    Users[indexUser].labelSupremum     = INV_LONG;
    Users[indexUser].labelInfimum      = INV_LONG;
}

unsigned int Network::getMaxNumUserCheckIns() const{
 return maxNumCheckins;
}

unsigned int Network::getNumNodesDeleted() const{
 return numNodesDeleted;
}

vector<Link> Network::getTestLinks() const{
    return TestLinks;
}

//check-in activity considers only the values 1 and 0 for a vector when the user visit or not some place
double Network::getCosineSimByCheckinActivity(index_v indexUserA, index_v indexUserB) const{
    vector<index_p> AB = getIntersectPlaces(indexUserA, indexUserB);
    vector<Place>    A = getVisitedPlaces(indexUserA);
    vector<Place>    B = getVisitedPlaces(indexUserB);

    if(A.size() == 0 || B.size() == 0 || AB.size() == 0)
        return 0.0;

    return (1.0 * AB.size())/(sqrt(A.size()) * sqrt(B.size()));
}

bool Network::isUserForPlacePrediction(index_v indexUser) const{
    return Users[indexUser].active;
}

unsigned int Network::getCheckinActivity(index_v indexUser, long labelPlace) const{
    if(getCheckinsUserAtPlace(indexUser, labelPlace).size() > 0)
        return 1;

    return 0;
}
