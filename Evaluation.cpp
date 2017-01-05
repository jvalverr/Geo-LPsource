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


#include "Evaluation.h"


int myRandom(int i){ return std::rand()%i; }

typedef std::numeric_limits< double > dbl;

Evaluation::Evaluation() : isUndirected(true){
}

Evaluation::Evaluation(bool dir) : isUndirected(dir){
}

Evaluation::~Evaluation(){
}

void Evaluation::setRootPath(const char* path){
    strcpy(rootOuts, path);
}

void Evaluation::readLinksTestFile(const char* nameTestFile){
    ifstream isTest;
    isTest.open( nameTestFile, ios::in );

    if(isTest.is_open()){
        while(!isTest.eof()){
            Link link;
            link.score = 0.0;
            isTest >> link.indexVertex1 >> link.indexVertex2;

            //if it is undirected network, we just save the link in one direction
            if(isUndirected){
                if(link.indexVertex1 < link.indexVertex2)
                    TestLinks.push_back( link );
            }
            else
                TestLinks.push_back( link );
        }
     }
     else{
        cerr << "\nTest file for evaluation no found";
     }



    isTest.close();
    sortLinkListTest();
}

void Evaluation::sortLinkListTest(){
    //first we sort the list
    sort(TestLinks.begin(), TestLinks.end(), linkCompareByNode1() );

    index_v indAct = 0;
    for(index_v ind = 1; ind < TestLinks.size(); ind++){
        if( TestLinks[ind].indexVertex1 != TestLinks[indAct].indexVertex1){
            sort(TestLinks.begin() + indAct, TestLinks.begin() + ind, linkCompareByNode2() ); //sort
            indAct = ind;
        }
    }
    sort(TestLinks.begin() + indAct, TestLinks.end(), linkCompareByNode2() );

    //second, we remove duplicates
    vector<Link>   newTestList;
    if(TestLinks.size() > 0)
        newTestList.push_back(TestLinks[0]);

    for(index_v i = 1; i < TestLinks.size(); i++){
        if(TestLinks[i].indexVertex1 == newTestList[newTestList.size()-1].indexVertex1 && TestLinks[i].indexVertex2 == newTestList[newTestList.size()-1].indexVertex2)
            ; //we do not make nothing
        else
            newTestList.push_back(TestLinks[i]);
    }

    //finaly, we update the test links list
    TestLinks.swap(newTestList);
}

void Evaluation::sortPredictedLinksList(){
    //first we sort the list
    sort(PredictedLinks.begin(), PredictedLinks.end(), linkCompareByNode1() );

    index_v indAct = 0;
    for(index_v ind = 1; ind < PredictedLinks.size(); ind++){
        if( PredictedLinks[ind].indexVertex1 != PredictedLinks[indAct].indexVertex1){
            sort(PredictedLinks.begin() + indAct, PredictedLinks.begin() + ind, linkCompareByNode2() ); //sort
            indAct = ind;
        }
    }
    sort(PredictedLinks.begin() + indAct, PredictedLinks.end(), linkCompareByNode2() );
}

bool Evaluation::isGoodValue(string value){
    if(value == "inf" || value == "-inf" || value == "+inf" || value == "nan" || value == "-nan" || value == "+nan")
        return false;
    return true;
}

//this function verify the size of predicted links vectors
unsigned int Evaluation::verifySize(unsigned int n){
    //if here no exist correctly predicted links
    if(ExistentScoresList.size() == 0)
        return IS_NOT_CANDIDATE;

    //if the factor is lesser than N, we will no consider any process
    unsigned int factor = n * 1 / 100; //factor is 1% of n
    if(ExistentScoresList.size() < factor){
        //we resize the vector of non existent predicted links if necessary
        if(NonExistentScoresList.size() < ExistentScoresList.size()){
            vector<double> newNonExistentVector;

            while(newNonExistentVector.size() < ExistentScoresList.size()){
                for(unsigned int i = 0; i < NonExistentScoresList.size(); i++)
                    newNonExistentVector.push_back(NonExistentScoresList[i]);
            }//end of while

            NonExistentScoresList.swap(newNonExistentVector);
            random_shuffle(NonExistentScoresList.begin(), NonExistentScoresList.end(), myRandom);
        }

        return IS_SMALL;
    }

    //if correcty and wrongly predicted vectors are lesser than N, we resize them
    if(ExistentScoresList.size() < n){
        vector<double> newExistentVector;

        while(newExistentVector.size() < n){
            for(unsigned int i = 0; i < ExistentScoresList.size(); i++){
                newExistentVector.push_back(ExistentScoresList[i]);
            }

            if(newExistentVector.size() < n)
                random_shuffle(ExistentScoresList.begin(), ExistentScoresList.end(), myRandom);
        }//end of while

        ExistentScoresList.swap(newExistentVector);
    }

    if(NonExistentScoresList.size() < n){
        vector<double> newNonExistentVector;

        while(newNonExistentVector.size() < n){
            for(unsigned int i = 0; i < NonExistentScoresList.size(); i++){
                newNonExistentVector.push_back(NonExistentScoresList[i]);
            }

            if(newNonExistentVector.size() < n)
                random_shuffle(NonExistentScoresList.begin(), NonExistentScoresList.end(), myRandom);
        }//end of while

        NonExistentScoresList.swap(newNonExistentVector);
    }

    return IS_RESIZED;
}

void Evaluation::readPredictedLinksFile(const char* nameFile){
    //we need free the space for a new predicted links file and other information
    vector<Link> PLnew;
    PredictedLinks.swap(PLnew);
    vector<double> V1;
    ExistentScoresList.swap(V1);
    vector<double> V2;
    NonExistentScoresList.swap(V2);

    ifstream isPredicted;
    isPredicted.open( nameFile, ios::in );

    if(isPredicted.is_open()){
        while(!isPredicted.eof()){
            Link link;
            string scorev;
            isPredicted >> link.indexVertex1 >> link.indexVertex2 >> scorev;

            if(isGoodValue(scorev)){
                link.score = atof(scorev.c_str());

                if(isUndirected){
                    if(link.indexVertex1 < link.indexVertex2){
                        if(searchLinkInTestList(link)){
                            ExistentScoresList.push_back(link.score);
                            link.classLink = true;
                        }
                        else{
                            NonExistentScoresList.push_back(link.score);
                            link.classLink = false;
                        }

                        PredictedLinks.push_back(link);
                    }
                }//end of if it is undirected
                else{//if it is a directed network
                    if(searchLinkInTestList(link)){
                        ExistentScoresList.push_back(link.score);
                        link.classLink = true;
                    }
                    else{
                        NonExistentScoresList.push_back(link.score);
                        link.classLink = false;
                    }

                    PredictedLinks.push_back(link);
                }//end of else
            }//end of it is good
        }//end of while
    }
    else{
        cerr << "\nPredicted links file no found";
     }

    isPredicted.close();
}

bool Evaluation::searchLinkInTestList(Link key){
    if(getPosition(TestLinks, key) == -1)
        return false;
    return true;
}

int Evaluation::getPosition(vector<Link>& List, Link key){
    int Iright    = List.size() - 1;
    int Ileft     = 0;
    int Icenter   = 0;
    bool goMoreSearch = false;

    //we make a binary seach of the first element of the key
    while ( Ileft <= Iright ){
        Icenter = (Iright + Ileft)/2;

        if(List[Icenter].indexVertex1 == key.indexVertex1){
            Ileft = Iright + 1; //to go from the bucle
            goMoreSearch = true;
        }
        else{
            if(key.indexVertex1 < List[Icenter].indexVertex1)
                Iright = Icenter - 1;
            else
                Ileft = Icenter + 1;
        }
    }

    if(goMoreSearch){//we search lineraly the second element of the key
        //first, we try from the current position to the right
        if(List[Icenter].indexVertex2 <= key.indexVertex2){
            for(index_v i = Icenter; i < List.size(); i++){
                if(List[i].indexVertex1 == key.indexVertex1){
                    if(List[i].indexVertex2 == key.indexVertex2)
                        return i;
                    if(List[i].indexVertex2 > key.indexVertex2)
                            break;
                }
                else
                    i = List.size();
            }//end of for
        }
        else{
            //second, we try from the current position to the left
            if(Icenter > 0){
                for(int i = Icenter - 1; i >= 0; i--){
                    if(List[i].indexVertex1 == key.indexVertex1){
                        if(List[i].indexVertex2 == key.indexVertex2)
                            return i;
                        if(List[i].indexVertex2 < key.indexVertex2)
                            break;
                    }
                    else
                        break;
                }
            }
        }//end of else
    }

    return -1;
}

double Evaluation::calculateAUC(unsigned int n){
    double nprim = 0, n2prim = 0;
    unsigned int value = verifySize(n);
    unsigned int ncomp = n;

    if(value == IS_NOT_CANDIDATE)
        return 0;
    if(value == IS_SMALL)
        ncomp = ExistentScoresList.size();

    for(unsigned int i = 0; i < ncomp; i++){
        if(ExistentScoresList[i] > NonExistentScoresList[i])
            nprim++;
        else if(ExistentScoresList[i] == NonExistentScoresList[i])
            n2prim++;
    }

    return (nprim + (0.5 * n2prim))/n;
}

double Evaluation::calculatePrecision(unsigned int L){
    unsigned int min_size = NonExistentScoresList.size();

    if(ExistentScoresList.size() < NonExistentScoresList.size())
        min_size = ExistentScoresList.size();
    if(L > min_size)
        L = min_size;

    double Lr = 0;
    for(unsigned int i = 0; i < L; i++){
        if(PredictedLinks[i].classLink == true)
            Lr++;
    }
    return (double) Lr/L;
}

void Evaluation::setMethodsList(vector<string> vMethds){
    MethodsList = vMethds;
}

void Evaluation::setDirection(bool dir){
    isUndirected = false;
}

void Evaluation::printPrecisonResults(const char* outFileName){
    if(MethodsList.size() == 0)
        cerr << "Please, you need set the methods list vector.";

    if(TestLinks.size() == 0)
        cerr << "Please, you need read a test links file.";

    vector<unsigned int> Llist;
    Llist.push_back(  100 );
    Llist.push_back( 1000 );
    Llist.push_back( 2000 );
    Llist.push_back( 3000 );
    Llist.push_back( 4000 );
    Llist.push_back( 5000 );

    ofstream out;
    out.open( outFileName );
    out << "Method \t ";
    for(unsigned int j = 0; j < Llist.size(); j++)
        out << "L = " << Llist[j] << "\t";

    out.precision(dbl::digits10);
    for(unsigned int i = 0; i < MethodsList.size(); i++){
        //we read the respective file name of predicted links
        char predFile[500];
        strcpy ( predFile , rootOuts);
        strcat ( predFile , MethodsList[i].c_str() );
        strcat ( predFile , "-PredScores.txt");

        //after that, we read the predicted links list
        readPredictedLinksFile(predFile);
        sort( PredictedLinks.begin(), PredictedLinks.end(), scoreLinkCompare());

        //calculate the precision for different L values
        out << "\n" << MethodsList[i].c_str() << "\t";
        for(unsigned int j = 0; j < Llist.size(); j++){
            double precision = calculatePrecision(Llist[j]);
            out << precision << "\t";
        }
    }

    out.close();
}

void Evaluation::printQuantitativeResults(const char* outFileName){
    if(MethodsList.size() == 0)
        cerr << "Please, you need set the methods list vector.";

    if(TestLinks.size() == 0)
        cerr << "Please, you need read a test links file.";

    ofstream out;
    out.open( outFileName );
    out << "Method \t #correctly \t #wrongly \t test set size \t Precision \t Recall \t F-measure";

    out.precision(dbl::digits10);
    for(unsigned int i = 0; i < MethodsList.size(); i++){
        //we read the respective file name of predicted links
        char predFile[300];
        strcpy ( predFile , rootOuts);
        strcat ( predFile , MethodsList[i].c_str() );
        strcat ( predFile , "-PredScores.txt");

        //after that, we read the predicted links list
        readPredictedLinksFile(predFile);

        double tp = ExistentScoresList.size();
        double fp = NonExistentScoresList.size();
        double fn = TestLinks.size() - ExistentScoresList.size();

        out << "\n" << MethodsList[i].c_str() << "\t" << ExistentScoresList.size() << "\t";
        out << NonExistentScoresList.size() << "\t" << TestLinks.size() << "\t";

        double den1 = tp + fp;
        double precision = 0;
        if(den1 != 0)
            precision = tp/den1;

        double den2 = tp + fn;
        double recall = 0;
        if(den2 != 0)
            recall = tp/den2;

        double den3 = precision + recall;
        double fmeasure  = 0;
        if(den3 != 0)
            fmeasure  = 2 * ((precision * recall)/den3);

        out << precision << "\t" << recall << "\t" << fmeasure;
    }

    out.close();
}

void Evaluation::printAucResults(const char* outFileName, unsigned int n){
    if(MethodsList.size() == 0){
        cerr << "\nPlease, you need set the methods list vector.";
    }

    if(TestLinks.size() == 0){
        cerr << "\nPlease, you need read a test links file.";
    }

    ofstream out;
    out.open( outFileName );
    out << "Method \t AUC (N = " << n << ")";

    out.precision(dbl::digits10);
    for(unsigned int i = 0; i < MethodsList.size(); i++){
        //we read the respective file name of predicted links
        char predFile[300];
        strcpy ( predFile , rootOuts);
        strcat ( predFile , MethodsList[i].c_str() );
        strcat ( predFile , "-PredScores.txt");

        //after that, we read the predicted links list, we do not need to sort the links
        readPredictedLinksFile(predFile);

        //we apply a random suffle process
        random_shuffle(   ExistentScoresList.begin(),    ExistentScoresList.end(), myRandom );
        random_shuffle(NonExistentScoresList.begin(), NonExistentScoresList.end(), myRandom );

        //calculate and print the auc value
        double auc = calculateAUC(n);
        out << "\n" << MethodsList[i].c_str() << "\t" << auc;
    }

    out.close();
}


vector<Link> Evaluation::getCorrectlyPredictedLinks(){
    vector<Link> correctlyPL;

     for(unsigned int i = 0; i < PredictedLinks.size(); i++){
        //just consider the truth ground predictions
        if(PredictedLinks[i].classLink == true)
            correctlyPL.push_back(PredictedLinks[i]);
    }

    return correctlyPL;
}

vector<Link> Evaluation::getCorrectlyPredictedLinks (const char* predictedFileName){
    char predFile[300];
    strcpy ( predFile , predictedFileName );
    strcat ( predFile , "-PredScores.txt");

    ifstream isPredicted;
    isPredicted.open( predFile, ios::in );
    vector<Link> correctlyPL;

    if(isPredicted.is_open()){
        while(!isPredicted.eof()){
            Link link;
            string scorev;
            isPredicted >> link.indexVertex1 >> link.indexVertex2 >> scorev;

            if(isGoodValue(scorev)){
                link.score = atof(scorev.c_str());

                if(isUndirected){
                    if(link.indexVertex1 < link.indexVertex2){
                        if(searchLinkInTestList(link)){
                            link.classLink = true;
                            correctlyPL.push_back(link);
                        }
                    }
                }//end of if it is undirected
                else{//if it is a directed network
                    if(searchLinkInTestList(link)){
                        link.classLink = true;
                        correctlyPL.push_back(link);
                    }
                }//end of else
            }
        }//end of while
    }

    isPredicted.close();
    return correctlyPL;
}

void Evaluation::push_back_strengths_vec(vector<Link> StrgL){
    vAllStrengths.push_back(StrgL);
}

vector<Link> Evaluation::getStrengths_vec(unsigned int pos){
    return vAllStrengths[pos];
}

vector<Link> Evaluation::getTestLinks(){
    return TestLinks;
}
