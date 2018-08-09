/*
------------------------------------------------
Copyright (C) 2016-2019 by Jorge C. Valverde-Rebaza
This file is part of Geo-LPsource.

Geo-LPsource is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
Geo-LPsource is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should
have received a copy of the GNU General Public License along with Geo-LPsource. If not, see <http://www.gnu.org/licenses/>.
------------------------------------------------
*/



#include <limits>
#include <iostream>

#include "../../Network.h"
#include "../../Evaluation.h"
#include "../../Timer.h"

//traditional methods based on location information
#include "../CollocationsLP.h"
#include "../PreferentialAttachmentPlacesLP.h"
#include "../CommonLocationsLP.h"
#include "../JaccardPlacesLP.h"
#include "../AdamicAdarEntropyLP.h"

//ICPR 2016 author proposals
#include "../TotalAndPartialOverlappingOfPlacesLP.h"
#include "../WithinAndOutsideCommonPlacesLP.h"
#include "../CommonNeighborsPlacesLP.h"


//IPM 2018 state-of-the-art methods
#include "../AdamicAdarPlacesLP.h"
#include "../LocationCategoryLP.h"
#include "../GeodistLP.h"
#include "../AdjustedHausdorffDistanceLP.h"

//IPM 2018 author proposals
#include "../CheckInObservationLP.h"
#include "../CheckInAllocationLP.h"
#include "../FriendAllocationLP.h"
#include "../CommonNeighborsNearbyPlacesLP.h"
#include "../NearbyDistanceAllocationLP.h"


using namespace std;

void dataSetup();
void printNetworkStatistics(const char*, const char*, const char*);
void friendshipPredictionSetup(const char*, const char*,  const char*, const char*);
void runLinkPredictionMethods(const Network&, pred_type);
void linkPredictionProcess(const char*, const Network&, pred_type);
void friendshipPredictionPerformanceEvaluation(const char*, const char*);
vector<string> getFriendshipPredictionMethods();
vector<string> getPlacePredictionMethods();



////////////////////////////////////////
////////// GLOBAL VARIABLES   //////////
//the directory to save the out files
const char* outPath                          = "output/";

//we consider a difference time of one (1) day as default, so, the value of it variable is 0 because comparison is made between days
threshold DIFF_TIME_VISIT   = 0;

//For CNNP methods (COMMON NEIGHBORS OF NEARBY PLACES) we consider as the maximum distance between two place 1500 meters
double MAX_DIST_PLACES      = 1500;
///////////////////////////////////////

typedef std::numeric_limits< double > dbl;

int main(){
    dataSetup();
    return 0;
}


//configuration methods

void dataSetup(){
    const char* isLinks    = "data/MyNetwork_edges.txt";
    const char* isCheckins = "data/MyNetwork_checkins.txt";
    const char* testFile   = "MyNetwork_test.txt";
    const char* statFile   = "MyNetwork_stats.xls";
    const char* date       = "2010-10";

    cout << "\n Process initiated\n";

    //print network statistics
    char statFileName[250]     = "";
    strcpy(statFileName, outPath);
    strcat(statFileName, statFile);
    printNetworkStatistics(isLinks, isCheckins, statFileName);

    //run different friendship prediction methods
    char testFileName[250]     = "";
    strcpy(testFileName, outPath);
    strcat(testFileName, testFile);
    friendshipPredictionSetup( isLinks, isCheckins, testFileName, date );

    //for calculate the evaluation measures
    friendshipPredictionPerformanceEvaluation(testFileName, date);

    cout << "\n Process finished\n";
}

/////////////////////////////////////////////////////
//////////////// LINK PREDICTION PROCESS ////////////
/////////////////////////////////////////////////////

//for friendship
void friendshipPredictionSetup(const char* isLinks, const char* isCheckins,  const char* nameFileTest, const char* dateToBeginTesting){
    //read the link and check-in files
    Network network = Network();
    network.readLinksFile(isLinks);
    network.readCheckInFile(isCheckins);

    //make the sub-sampling process
    time_v timeToBeginTesting = network.monthToInt(dateToBeginTesting);
    network.edgeRandomSubSamplingBasedOnPeriod(timeToBeginTesting);
    network.sortLinkListTest();

    //save the test file
    cout << "\n printing test links file";
    ofstream os2;
    os2.open( nameFileTest );
    network.printLinksTest( os2 );
    os2.close();

    //set some network information useful for some link prediction methods
    cout << "\n setting configurations";
    network.setEntropyInformation(); //to calculate the entropy of each place
    network.setHomeLocationInformation(); //to calculate the home location of each user
    network.setBoundariesInformation(); //to calculate the supremum and infimum of each user

    //execute different link prediction methods
    cout << "\n executing link prediction methods";
    runLinkPredictionMethods(network, FRIEND_PRED);
}

void runLinkPredictionMethods(const Network& network, pred_type typeOfPrediction ){
    vector<string> listMethods;

    if(typeOfPrediction == FRIEND_PRED)
        listMethods = getFriendshipPredictionMethods();
    else if (typeOfPrediction == PLACE_PRED)
        listMethods = getPlacePredictionMethods();

    for(unsigned int i = 0; i < listMethods.size(); i++){
        linkPredictionProcess( listMethods[i].c_str(), network, typeOfPrediction );
    }
}

vector<string> getFriendshipPredictionMethods(){
    vector<string> listMethods;

    //state-of-the-art methods
    listMethods.push_back( "Collocations" );
    listMethods.push_back( "CommonLocations" );
    listMethods.push_back( "JaccardPlaces" );
    listMethods.push_back( "AdamicAdarEntropy" );
    listMethods.push_back( "PreferentialAttachmentPlaces" );
    listMethods.push_back( "AdamicAdarPlaces" );
    listMethods.push_back( "LocationCategory" );
    listMethods.push_back( "Geodist" );
    listMethods.push_back( "AdjustedHausdorffDistance" );

    //author proposals at ICPR2016
    listMethods.push_back( "WithinAndOutsideCommonPlaces" );
    listMethods.push_back( "CommonNeighborsPlaces" );
    listMethods.push_back( "TotalAndPartialOverlappingOfPlaces" );

    //author proposal at IPM 2018
    listMethods.push_back( "CheckInObservation" );
    listMethods.push_back( "CheckInAllocation" );
    listMethods.push_back( "FriendAllocation" );
    listMethods.push_back( "CommonNeighborsNearbyPlaces" );
    listMethods.push_back( "NearbyDistanceAllocation" );


    return listMethods;
}

vector<string> getPlacePredictionMethods(){
    vector<string> listMethods;

    //TO DO

    return listMethods;
}

void linkPredictionProcess(const char* predictorName, const Network& network, pred_type typeOfPrediction){
    bool predictorExist = true;
    LinkPredictor* predictor;
    //for friendship prediction: state-of-the-art
    if( strcmp( predictorName, "Collocations" ) == 0 ) {
        predictor = new CollocationsLP(network, DIFF_TIME_VISIT);
     }
     else if( strcmp( predictorName, "CommonLocations" ) == 0 ) {
        predictor = new  CommonLocationsLP(network);
     }
     else if( strcmp( predictorName, "JaccardPlaces" ) == 0 ) {
        predictor = new JaccardPlacesLP(network);
     }
     else if( strcmp( predictorName, "AdamicAdarEntropy" ) == 0 ) {
        predictor = new AdamicAdarEntropyLP(network);
     }
     else if( strcmp( predictorName, "PreferentialAttachmentPlaces" ) == 0 ) {
        predictor = new PreferentialAttachmentPlacesLP(network);
     }
     else if( strcmp( predictorName, "AdamicAdarPlaces" ) == 0 ) {
        predictor = new AdamicAdarPlacesLP(network);
     }
     else if( strcmp( predictorName, "LocationCategory" ) == 0 ) {
        double max_entropy_allowed = network.getAverageEntropyOfPlaces();
        predictor = new LocationCategoryLP(network, max_entropy_allowed);
     }
     else if( strcmp( predictorName, "Geodist" ) == 0 ) {
        predictor = new GeodistLP(network);
     }
     else if( strcmp( predictorName, "AdjustedHausdorffDistance" ) == 0 ) {
        predictor = new AdjustedHausdorffDistanceLP(network);
     }

     // for friendship prediction: author proposals ICP2016
     else if( strcmp( predictorName, "WithinAndOutsideCommonPlaces" ) == 0 ) {
        predictor = new WithinAndOutsideCommonPlacesLP(network);
     }
     else if( strcmp( predictorName, "CommonNeighborsPlaces" ) == 0 ) {
        predictor = new CommonNeighborsPlacesLP(network);
     }
     else if( strcmp( predictorName, "TotalAndPartialOverlappingOfPlaces" ) == 0 ) {
        predictor = new TotalAndPartialOverlappingOfPlacesLP(network);
     }

    // for friendship prediction: author proposals IPM 2018
     else if( strcmp( predictorName, "CheckInObservation" ) == 0 ) {
        predictor = new CheckInObservationLP(network);
     }
     else if( strcmp( predictorName, "CheckInAllocation" ) == 0 ) {
        predictor = new CheckInAllocationLP(network);
     }
     else if( strcmp( predictorName, "FriendAllocation" ) == 0 ) {
        predictor = new FriendAllocationLP(network);
     }
     else if( strcmp( predictorName, "CommonNeighborsNearbyPlaces" ) == 0 ) {
        predictor = new CommonNeighborsNearbyPlacesLP(network, MAX_DIST_PLACES);
     }
     else if( strcmp( predictorName, "NearbyDistanceAllocation" ) == 0 ) {
        predictor = new NearbyDistanceAllocationLP(network);
     }

     //for other case
     else{
        cerr << "\n... The predictor " << predictorName << " does not exist";
        predictorExist = false;
    }

    if( predictorExist == true ){
        char predictFileAval[250] = "";
        strcpy( predictFileAval, outPath);
        strcat( predictFileAval , predictorName );
        strcat( predictFileAval ,  "-PredScores.txt" );

        cout << "\n ... processing " << predictorName;
        ofstream fileScoresOut;
        fileScoresOut.open( predictFileAval );

        if(typeOfPrediction == FRIEND_PRED){
            unsigned int degree = 2;
            predictor->printFriendshipScores( fileScoresOut , degree );
        }
        else if (typeOfPrediction == PLACE_PRED){
            predictor->printPlacePredictionScores( fileScoresOut );
        }

        fileScoresOut.close();
    }
}


/////////////////////////////////////////////////////
//////////////// statistics /////////////////////////
/////////////////////////////////////////////////////

void printNetworkStatistics(const char* isLinks, const char* isCheckins, const char* outName ){
    ofstream os;
    os.open(outName);

    Network network = Network();
    network.readLinksFile( isLinks );
    network.readCheckInFile(isCheckins);
    network.setEntropyInformation();

    network.printStatistics(os);

    os.close();
}


/////////////////////////////////////////////////////
//////////////// evaluation /////////////////////////
/////////////////////////////////////////////////////


void friendshipPredictionPerformanceEvaluation(const char* testFile, const char* dateToBeginTesting){

    //set the configuration
    cout << "\n executing performance evaluation";
    Evaluation eval = Evaluation(true);//we have undirected networks
    eval.readLinksTestFile(testFile);
    vector<string> listMethods = getFriendshipPredictionMethods();
    eval.setMethodsList(listMethods);
    eval.setRootPath(outPath);

    //print evaluation results
    char aucFile[250] =  "";
    strcpy(aucFile, outPath);
    strcat(aucFile, "MyNetwork_auc.xls");
    unsigned int Nauc    = 20;
    eval.printAucResults(aucFile, Nauc);

    char quantFile[250] = "";
    strcpy(quantFile, outPath);
    strcat(quantFile, "MyNetwork_quantitative.xls");
    eval.printQuantitativeResults(quantFile);

    char precFile[250] = "";
    strcpy(precFile, outPath);
    strcat(precFile, "MyNetwork_precision.xls");
    eval.printPrecisonResults(precFile);
}
