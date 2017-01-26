#ifndef _FAKEDATA_H_
#define _FAKEDATA_H_ 1

#include "MDdataWordBM.h"
#include "UFEDataContainer.h"

UFEDataContainer* makeContainer(int board_id, size_t size);

void setTrHeader(UFEDataContainer *c, uint64_t pos32, int board_id, int tag);

void setTrTrailer(UFEDataContainer *c, uint64_t pos32, int board_id, int tag);

#endif


