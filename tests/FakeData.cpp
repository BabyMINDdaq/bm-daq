#include "FakeData.h"

UFEDataContainer* makeContainer(int board_id, size_t size) {
  UFEDataContainer *c = new UFEDataContainer();
  c->resize(c->memsize());
  for (size_t i=0; i<c->memsize()/4; ++i) {
    *(c->get32bWordPtr(i)) = 0x30000000 + i;
  }
  c->setBoardId(board_id);
  c->resize(size);

  return c;
}

void setTrHeader(UFEDataContainer *c, uint64_t pos32, int board_id, int tag) {
  uint32_t *dw = c->get32bWordPtr(pos32);
  *dw = board_id << 21;
  *dw |= tag;
  c->spill_headers_.push_back(pos32*4);
//   cerr << "\n" << hex << *dw << dec << endl;
}

void setTrTrailer(UFEDataContainer *c, uint64_t pos32, int board_id, int tag) {
  uint32_t *dw = c->get32bWordPtr(pos32);
  *dw = 0x70000001;
  c->spill_trailers_.push_back(pos32*4);
  *(dw-1) = 0x60000000;
  *(dw-1) |= board_id << 21;

  *(dw-2) = 0x60000000;
  *(dw-2) |= board_id << 21;
  *(dw-2) |= tag;

//   cerr << hex << *dw << " " << *(dw-1) << " " << *(dw-2) << dec << endl;
}