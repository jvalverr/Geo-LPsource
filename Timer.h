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



#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
using namespace std;

class Timer {
private:

    timeval startTime;

public:

    void start(){
        gettimeofday(&startTime, NULL);
    }

    double stop(){
        timeval endTime;
        long seconds, useconds;
        double duration;

        gettimeofday(&endTime, NULL);

        seconds  = endTime.tv_sec  - startTime.tv_sec;
        useconds = endTime.tv_usec - startTime.tv_usec;

        duration = seconds + useconds/1000000.0;

        return duration;
    }

    static void printTime(double duration){
        if( duration >= 60 ){
            double dmins = duration / 60.0;
            printf("%5.6f mins\n", dmins);
        }
        else  if( duration >= 3600 ){
            double dhrs = duration / 3600.0;
            printf("%5.6f hours\n", dhrs);
        }
        else
            printf("%5.6f seconds\n", duration);
    }
};
