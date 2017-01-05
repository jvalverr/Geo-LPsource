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

#ifndef NETWORK_H_INCLUDED
#define NETWORK_H_INCLUDED

#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <fstream>
#include <math.h>
#include <time.h>
#include <cstdlib>
#include <stdio.h>
#include <limits>


#include "Structures.h"

using namespace std;

class Network {
    private:
        unsigned int numEdges;
        unsigned int numNodes;
        unsigned int maxPathLength;
        long         maxNumFriendsPerUser;
        bool         isWeighted;
        unsigned int numCheckIn;
        unsigned int numNodesWithCheckIn;
        unsigned int numTotalPlaces;
        unsigned int minDegreeToSampling;
        unsigned int minNumCheckinsToSampling;
        bool         calculateEntropyOfPlaces;
        bool         calculateHomes;
        bool         calculateBoundaries;
        unsigned int maxNumCheckins;
        double       maxEntropyValue;
        unsigned int numNodesDeleted;
        unsigned int minNumPlacesToSampling;

        vector<Node>  Users;       //list of all users
        vector<Link>  TestLinks;   //list of all test links
        vector<Place> Places;     //list of distinct places
        vector<PlaceTest> TestingPlaces; //list of place information for testing


        //methods
        void sortNeighborsOfUsers();
        void sortGroupsOfUsers();
        void sortLocationsOfPlaces();
        void setLocationInformation();
        void setEntropyOfPlace(index_p);
        void updateStructuralStatistics();
        void updateLocationInformationAfterSampling(index_v, time_v);
        bool isSamplingCandidate(index_v);
        bool isPeriodTimeCandidate(index_v, time_v);
        unsigned int getSignalPeriodTimeCandidate(index_v, index_p, time_v);

    public:
        Network();
       ~Network();
        Network& operator=( const Network& );
        double getSampleCorrelationCoefficient(vector<vector<double> >&, vector<vector<double> >&) const;
        double getSampleCorrelationCoefficient(vector<double>&, vector<double>&) const;


        //reads
        void readLinksFile         (const char*);
        void readCheckInFile       (const char*);
        void makeUndirected        (const char*, const char*);

        //prints
        void printLinksList ( ostream& );
        void printLinksTest ( ostream& );
        void printWeightedLinksTest ( ostream& );
        void printStatistics( ostream& );
        void printCheckinList( ostream& );
        void printPlacesList( ostream& );
        void printPlacesEntropy( ostream& );
        void printPlacesListOfUsers( ostream& );
        void printPlacesTest( ostream& );


        //sample
        void edgeRandomSubSampling( double );
        void edgeRandomSubSamplingBasedOnPeriod( time_v );
        void sampleSelectedProcessBasedOnPeriod( time_v );
        void saveLinkInTestingSet(index_v, index_v);
        void setTestLinksList(vector<Link>);
        void sortLinkListTest();
        void placeRandomSubSampling(double, double);
        void setMinNumPlacesToSampling(unsigned int);


        //structural properties
        bool          hasEdge( index_v, index_v )                           const;
        bool          isNetworkWeighted()                                   const;
        double        getWeight( index_v, index_v)                          const;
        double        getStrengthOfNode( index_v )                          const;
        unsigned int  getNumUsers()                                         const;
        unsigned int  getDegree( index_v )                                  const;
        unsigned int  getDiameter();
        unsigned long getNumEdgesFriendshipGraph();
        long          getMaxNumberFriendsPerUser();
        double        getAverageFriendsPerUser();
        double        getAverageDegree()                                    const;
        double        getClusteringCoefficient( index_v )                   const;
        double        getAverageClusteringCoefficient()                     const;
        double        getAverageAssortativityCoefficient()                  const;
        double        getHeterogeneity()                                    const;
        double        getAveragePathLenght();
        vector<index_v>     getNeighbors( index_v )                        const;
        vector<index_v>     getNeighbors( index_v, unsigned int )          const;
        vector<index_v>     getShortestPathLengths()                       const;
        vector<index_v>     getPathLengths( index_v )                      const;
        vector<index_v>     intersect(vector<index_v>, vector<index_v>)    const;
        vector<long>        intersect(vector<long>, vector<long>)          const;
        vector<index_v>     junction (vector<index_v>, vector<index_v>)    const;
        vector<index_p>     difference (vector<index_v>, vector<index_v>)  const;
        vector<Link>        getTestLinks()                                 const;


        //location
        time_v          monthToInt  (const char*);
        time_v          dayToInt    (const char*);
        void            deleteUser  (index_v);
        void            deleteLinkFromNetwork(index_v, index_v);
        void            deleteVisitantFromPlacesList(index_p, index_v, unsigned int);
        void            deletePlaceFromUser(index_v, long);
        void            setEntropyInformation();
        void            setBoundariesInformation();
        void            setHomeLocationInformation();
        void            setMinDegreeToSampling(unsigned int);
        void            setMinNumCheckinsToSampling(unsigned int);
        bool            isUserForPlacePrediction(index_v)                       const;
        bool            existIntersectPlaces(index_v, index_v)                  const;
        vector<Place>   getVisitedPlaces( index_v )                             const;
        vector<index_p> getIntersectPlaces( index_v, index_v )                  const;
        vector<Checkin> getCheckinsUserAtPlace(index_v, long)                   const;
        vector<Checkin> getCheckinsUser             (index_v)                   const;
        vector<index_p> getIndexPlacesOfAllCheckinsUser(index_v)                const;
        unsigned int    getTotalNumCheckIns()                                   const;
        unsigned int    getNumNodesWithCheckIn()                                const;
        unsigned int    getNumberTotalPlaces()                                  const;
        unsigned int    getTotalCheckinsAtPlace(index_p)                        const;
        unsigned int    getTotalCheckinsUser(index_v)                           const;
        unsigned int    getMinDegreeToSampling()                                const;
        unsigned int    getMinNumCheckinsToSampling()                           const;
        double          getEntropyOfPlace( index_p )                            const;
        double          getLocationDiversity( index_p )                         const;
        long            getHomeLocation(index_v)                                const;
        double          getGeographicDistance(double, double, double, double)   const; //distance in meters
        double          getGeographicDistanceAtMiles(index_p, index_p)          const; //distance in miles
        double          getGeographicDistanceAtKm(index_p, index_p)             const; //distance in kilometers
        double          getGeographicDistance(index_p, index_p)                 const; //distance in meters
        double          getAdjustedGeographicDistance(index_p, index_p)         const; //distance in meters
        long            getSupremumPlace(index_v)                               const;
        long            getInfimumPlace(index_v)                                const;
        double          getAverageCheckInPerUser()                              const;
        double          getAverageCheckInsPerLocation()                         const;
        double          getAverageEntropyOfPlaces()                             const;
        double          getMinEntropyOfPlaces()                                 const;
        double          getMaxEntropyOfPlaces()                                 const;
        unsigned int    getMaxNumUserCheckIns()                                 const;
        double          getMaxDiversityOfPlaces()                               const;
        unsigned int    getNumNodesDeleted()                                    const;
        vector<index_v> getVisitors(index_p)                                    const;
        vector<Place>   getPlacesVisitedByFriends( index_v )                    const;
        vector<index_p> getPotentialPlacesToVisit( index_v )                    const;
        double          getCosineSimByCheckinActivity(index_v, index_v)         const;
        unsigned int    getCheckinActivity(index_v, long)                       const;
};

#endif // NETWORK_H_INCLUDED
