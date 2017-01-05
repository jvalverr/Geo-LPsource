/*
------------------------------------------------
Copyright (C) 2016-2017 by Jorge C. Valverde-Rebaza
Email: jvalverr@icmc.usp.br

This file is part of Geo-LPsource.

Parts of the code of this file to calculate the topological properties of the network are based on parts of LPsource software.
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


#ifndef STRUCTURES_H_INCLUDED
#define STRUCTURES_H_INCLUDED

#include <vector>


using std::vector;
using std::pair;

typedef unsigned int index_v;
typedef unsigned int index_g;
typedef          int time_v;
typedef unsigned int threshold;
typedef unsigned int index_p;
typedef unsigned int pred_type;

const long   INV_LONG                        = -1;
const double INF_P                           = 990000;
const unsigned int MAX_GROUP_SIZE            = 8000000;
const time_v YEAR_BASE                       = 1900;
const unsigned int IS_NOT_CANDIDATE         = 0;
const unsigned int IS_IN_TRAINING           = 1;
const unsigned int IS_IN_TESTING            = 2;
const unsigned int IS_IN_BOTH_PERIODS       = 3;
const unsigned int IS_SMALL                 = 4;
const unsigned int IS_RESIZED               = 5;
const pred_type FRIEND_PRED                 = 0;
const pred_type PLACE_PRED                  = 1;
static const double EARTH_RADIUS_IN_METERS   =  6372797.560856;
static const double BIG_EARTH_DISTANCE       = 19118391; // (almost) three times earth radious
static const double DEG_TO_RAD               = 0.017453292519943295769236907684886;



/******************************************************/
/********** Places of a network      **********/
/********************************************************/

struct Checkin{
    char date[25];
    time_v period;
    time_v day;
};


struct Place{
    long label; //sera o id
    double latitude;
    double longitude;
    vector<Checkin> CheckinsList; //useful only for each user
    unsigned int ntotalCheckins; //useful only for the list of place in all the network
    vector<index_v> UsersList; //useful only for the list of place in all the network
    double entropy;            //useful only for the list of place in all the network
};

/******************************************************/
/***************** Node of a network      **************/
/********************************************************/
struct Node{
    vector<index_v>  Neighbors;   //list of indexes of neighborhood of the node
    vector<double>   Weights;     //list of weights of relationships between the node and their neighbors
    vector<index_g>  Groups;      //list of labels of groups to which the node belong to
    vector<Place>    Places;  //list of nodes's locations (all checkins)
    unsigned int     ntotalCheckins; //number of all checkins made at all visited places
    long             labelHomeLocation;
    long             labelSupremum;
    long             labelInfimum;
    bool             active;
};




/******************************************************/
/********** Link between a pair of nodes      **********/
/********************************************************/
struct Link{
    index_v indexVertex1;
    index_v indexVertex2;
    double score;
    bool classLink; //0 = non-existent, 1 = existent
};

/******************************************************/
/********** Pair indicating number of friends with a **/
/**********       determinate distance      **********/
/********************************************************/

struct PairElement{
    double label;
    double comLocRatio;
    unsigned int nPairs;
    unsigned int nFriends;
};


/******************************************************/
/********** Weight of a user  **/
/********************************************************/
struct WeightU{
    index_v indexUser;
    double weight;
};


struct PlaceTest{
    index_v indexUser;
    long    labelPlace;
    double  geoDistToHome;
    unsigned int ncheckins;
};




struct linkCompareByNode1{
    bool operator() (const Link& link1, const Link& link2 )const { return (link1.indexVertex1 < link2.indexVertex1); }
};

struct linkCompareByNode2{
    bool operator() (const Link& link1, const Link& link2 )const { return (link1.indexVertex2 < link2.indexVertex2); }
};

struct scoreLinkCompare{
    bool operator() (const Link& link1, const Link& link2 )const { return (link1.score > link2.score); }
};

struct placeComparedByLabel{
    bool operator() (const Place& place1, const Place& place2) const{ return (place1.label < place2.label); }
};

struct placeComparedByNumberCheckins{
    bool operator() (const Place& place1, const Place& place2) const{ return (place1.ntotalCheckins < place2.ntotalCheckins); }
};

struct checkinComparedByDayDate{
    bool operator() (const Checkin& checkin1, const Checkin& checkin2) const{ return (checkin1.day < checkin2.day); }
};


struct UsersWeightCompare{
    bool operator() (const WeightU& weight1, const WeightU& weight2 )const { return (weight1.weight > weight2.weight); }
};


#endif // STRUCTURES_H_INCLUDED
