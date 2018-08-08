#ifndef FRIENDALLOCATIONLP_H_INCLUDED
#define FRIENDALLOCATIONLP_H_INCLUDED


#include "../Network.h"
#include "LinkPredictor.h"

class FriendAllocationLP : public LinkPredictor {
	private:
	protected:
	public:
		FriendAllocationLP( const Network& );
		~FriendAllocationLP();
		virtual double generateScore( index_v, index_v );
};

#endif // FRIENDALLOCATIONLP_H_INCLUDED
