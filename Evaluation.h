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



#ifndef EVALUATION_H_INCLUDED
#define EVALUATION_H_INCLUDED

#include <iostream>
#include <fstream>
#include <algorithm>
#include <fstream>
#include <cstdlib>
#include <stdio.h>
#include <limits>
#include <string.h>
#include <math.h>


#include "Structures.h"

using namespace std;

class Evaluation{
    private:
        bool                        isUndirected;
        vector<Link>                TestLinks;
        vector<Link>                PredictedLinks;
        vector<double>              ExistentScoresList;
        vector<double>              NonExistentScoresList;
        vector< vector<Link> >      vAllStrengths;
        vector<string>              MethodsList;
        char                        rootOuts[250];

        bool searchLinkInTestList(Link);
        int  getPosition(vector<Link>&, Link);
        bool isGoodValue(string);
        unsigned int verifySize(unsigned int);
        void sortLinkListTest();
        void sortPredictedLinksList();
        double calculateAUC(unsigned int);
        double calculatePrecision(unsigned int);


    public:
        Evaluation();
        Evaluation(bool);
        ~Evaluation();
        void         readLinksTestFile          (const char*);
        void         readPredictedLinksFile     (const char*);
        void         setDirection               (bool);
        void         setRootPath                (const char*);
        void         setMethodsList             (vector<string>);
        vector<Link> getTestLinks               ();
        vector<Link> getCorrectlyPredictedLinks ();
        vector<Link> getCorrectlyPredictedLinks (const char*);
        void         printPrecisonResults       (const char*);
        void         printQuantitativeResults   (const char*);
        void         printAucResults            (const char*, unsigned int);
        void         push_back_strengths_vec    (vector<Link>);
        vector<Link> getStrengths_vec           (unsigned int);
};



#endif // EVALUATION_H_INCLUDED
