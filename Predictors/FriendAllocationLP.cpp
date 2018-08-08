
#include "FriendAllocationLP.h"

FriendAllocationLP::FriendAllocationLP( const Network& network ) : LinkPredictor(network) {
}

FriendAllocationLP::~FriendAllocationLP() {
}

double FriendAllocationLP::generateScore( index_v indexVertex1, index_v indexVertex2 ){
    vector<index_p> vCommonP = network.getIntersectPlaces( indexVertex1, indexVertex2 );
    if(vCommonP.size() == 0)
        return 0.0;

    vector<index_v> vCommonFriends = network.intersect(network.getNeighbors(indexVertex1), network.getNeighbors(indexVertex2));
    double faw = 0.0;

    for(index_v u = 0; u < vCommonFriends.size(); u++){
        vector<Place> vPlaces_z = network.getVisitedPlaces(vCommonFriends[u]);
        bool flagIntersect = false;

        for(index_p i = 0; i < vCommonP.size(); i++){
            for(index_p j = 0; j < vPlaces_z.size(); j++){
                if( (long) vCommonP[i] < vPlaces_z[j].label )
                    j = vPlaces_z.size();
                else{
                    if( (long) vCommonP[i] == vPlaces_z[j].label ){
                        flagIntersect = true;
                        j = vPlaces_z.size();
                        i = vCommonP.size();
                    }
                }//end of else
            }//end of for
        }//end of for

        if(flagIntersect == true){
            double ncheckins = network.getTotalCheckinsUser(vCommonFriends[u]);

            if(ncheckins > 0)
                faw += 1.0/ncheckins;
        }
    }

    return faw;
}
