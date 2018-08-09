Geo-LPsource
============
Copyright (C) 2016-2019 by `Jorge C. Valverde-Rebaza`_

.. _Jorge C. Valverde-Rebaza: http://www.labic.icmc.usp.br/jvalverr/

This file is part of Geo-LPsource.

Geo-LPsource is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. Geo-LPsource is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with Geo-LPsource. If not, see http://www.gnu.org/licenses/.

About
=====
Geo-LPsource is a link prediction framework built for work over Location-based Social Networks (LBSNs). Geo-LPsource implements different link prediction methods from the literature. Link prediction methods aim to predict different types of new relationships among users, locations and other objects in LBSNs. 

Considering the context of friendship prediction, we also implement some innovative methods based on combine social and mobility patterns, defined in [1]_ :

    * Within and Outside of Common Places (WOCP)
    * Common Neighbors of Places (CNP)
    * Total and Partial Overlapping of Places (TPOP)
    
Also, we implement friendship prediction methods defined in [2]_ :
   
   * Check-in observation (ChO)
   * Check-in allocation (ChA)
   * Friendship allocation within common places (FAW)
   * Common neighbors of nearby places (CNNP)
   * Nearby distance allocation (NDA)
  

How it works?
============
To build Geo-LPsource and prepare it for use, you must execute the Makefile file using the command 'make'. This will compile all the components of Geo-LPsource framework. After this process completes without errors, you can make use of the Geo-LPsource binary saved into the root directory with the name "bin".

The following steps describe the process to quickly use the automated build system for link prediction on sample data sets:

1. Follow the instructions above to prepare the Geo-LPsource binary
2. Save the network to use into the directory ./data. The network consist of two files: the edges file and the check-ins file.
3. Execute the binary file of Geo-LPsource using directly the command './bin' or 'nohup ./bin > geo-lpsource.log &' to execute the binary as background (recommended for large scale networks)
4. After the execution, find into the directory ./output the ouput files (one for each link prediction measure) containing, each one, a list of predicted links and their corresponding scores. The statistics file containing the main statistics of the network as well as the files containing the evaluation metrics results will be in this directory too.

Input files
============

As previously mentiones, Geo-LPsource need of two input files to can work. The first file is the edges file containing all the existent links forming the network and representing the existent friendships among the users. Therefore, this file contains a sorted list of pairs of users. Each line in this file contains a pair of users with indices starting from 0. An example of the format of this file can be found in data/MyNetwork_edges.txt.

The second file contains all the check-ins made by each one of the users. Therefore, each line of this file contains the index of the user, the date of the check-in, latitude and longitude of the location where the check-in was made, and the label of the check-in. The labels of the check-ins start from 0. An example of the format of this file can be found in data/MyNetwork_checkins.txt.


References
==========

.. [1] Valverde-Rebaza J., Roche M., Poncelet P. and Lopes A.: Exploiting Social and Mobility Patterns for Friendship Prediction in Location-Based Social Networks. In The 23rd International Conference on Pattern Recognition (ICPR), 2016, pp. 2526-2531.

.. [2] Valverde-Rebaza J., Roche M., Poncelet P. and Lopes A.: The role of location and social strength for friendship prediction in location-based social networks. Information Processing and Management, vol. 54, 2018, pp. 475–489.


