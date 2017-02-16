/** This file is part of BabyMINDdaq software package. This software
 * package is designed for internal use for the Baby MIND detector
 * collaboration and is tailored for this use primarily.
 *
 * BabyMINDdaq is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BabyMINDdaq is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with BabyMINDdaq.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  \author   Yordan Karadzhov <Yordan.Karadzhov \at cern.ch>
 *            University of Geneva
 *
 *  \created  Jan 2017
 */

#include <iostream>

#include "UFEContext.h"
#include "UFEError.h"

#include "TestWorkers.h"
#include "FakeData.h"

using namespace std;

void TestWorkers::setUp() {
  UFEContext *ctx = UFEContext::instance();
  ctx->devs_.resize(2);
  ctx->devs_[0].boardsPtr()->push_back(0);
  ctx->devs_[0].boardsPtr()->push_back(5);
  ctx->devs_[1].boardsPtr()->push_back(9);
}

void TestWorkers::tearDown() {
  
}

void TestWorkers::TestEventBuilderInitClose() {
  BMDEventBuilder eb1;
  eb1.recorder().setRunNum(999);
  CPPUNIT_ASSERT( eb1.board_event_builders_.size() == 0 );
  CPPUNIT_ASSERT( eb1.spill_mem_bank_ == nullptr );

  UFEContext *ctx = UFEContext::instance();
  eb1.init();

  CPPUNIT_ASSERT( eb1.spill_mem_bank_->size() == sizeof(BMDEventHeader) );
  CPPUNIT_ASSERT( eb1.spill_mem_bank_->memsize() == 40000*ctx->ufec_ctx_->readout_buffer_size_ );

  CPPUNIT_ASSERT( ctx->devs_.size() == 2 );
  for (auto const & dev: ctx->devs_)
    for (auto const & b: dev.boards()) {
      CPPUNIT_ASSERT( eb1.board_event_builders_.find(b) != eb1.board_event_builders_.end() );
      CPPUNIT_ASSERT( eb1.board_event_builders_[b]->getMemBankPtr()->size() == sizeof(BMDDataFragmenHeader) );
      CPPUNIT_ASSERT( eb1.board_event_builders_[b]->getMemBankPtr()->memsize() == 2000*ctx->ufec_ctx_->readout_buffer_size_ );
    }

  eb1.close(proc_status_t::OK_s);
  CPPUNIT_ASSERT( eb1.board_event_builders_.size() == 0 );
  CPPUNIT_ASSERT( eb1.spill_mem_bank_ == nullptr );
}

void TestWorkers::TestEBProcessCase1() {
  BMDEventBuilder eb2;
  eb2.recorder().setRunNum(1999);

  UFEDataContainer *c1 = makeContainer(9, 70*4);
  eb2.setOutputObj(&c1);
  eb2.init();

  UFEContext *ctx = UFEContext::instance();
  CPPUNIT_ASSERT( ctx->devs_.size() == 2 );
  CPPUNIT_ASSERT( eb2.board_event_builders_.size() == 3 );

  setTrHeader(c1, 12, 9, 5);

  CPPUNIT_ASSERT( eb2.spill_mem_bank_->spill_headers_.size() == 0 );
  CPPUNIT_ASSERT( eb2.spill_mem_bank_->spill_trailers_.size() == 0 );
  CPPUNIT_ASSERT( eb2.spill_mem_bank_->size() == sizeof(BMDEventHeader) );

  CPPUNIT_ASSERT( eb2.board_event_builders_.find(9) != eb2.board_event_builders_.end() );
  CPPUNIT_ASSERT( eb2.board_event_builders_[9]->getMemBankPtr()->spill_headers_.size() == 0 );
  CPPUNIT_ASSERT( eb2.board_event_builders_[9]->getMemBankPtr()->spill_trailers_.size() == 0 );
  CPPUNIT_ASSERT( eb2.board_event_builders_[9]->getMemBankPtr()->size() == sizeof(BMDDataFragmenHeader) );

  eb2.process();

  CPPUNIT_ASSERT( eb2.board_event_builders_[9]->getMemBankPtr()->size() == (70-12)*4 + sizeof(BMDDataFragmenHeader) );
  CPPUNIT_ASSERT( eb2.board_event_builders_[9]->getMemBankPtr()->spill_headers_.size() == 1 );
  CPPUNIT_ASSERT( eb2.board_event_builders_[9]->getMemBankPtr()->spill_trailers_.size() == 0 );
  CPPUNIT_ASSERT( c1->size() == 0 );
  CPPUNIT_ASSERT( c1->spill_headers_.size() == 0 );
  CPPUNIT_ASSERT( c1->spill_trailers_.size() == 0 );


  delete c1;
  c1 = makeContainer(5, 79*4);
  setTrHeader(c1, 26, 5, 5);

  CPPUNIT_ASSERT( eb2.board_event_builders_.find(5) != eb2.board_event_builders_.end() );
  CPPUNIT_ASSERT( eb2.board_event_builders_[5]->getMemBankPtr()->size() == sizeof(BMDDataFragmenHeader) );

  eb2.process();

  CPPUNIT_ASSERT( eb2.board_event_builders_[5]->getMemBankPtr()->size() == (79-26)*4 + sizeof(BMDDataFragmenHeader) );
  CPPUNIT_ASSERT( eb2.board_event_builders_[5]->getMemBankPtr()->spill_headers_.size() == 1 );
  CPPUNIT_ASSERT( eb2.board_event_builders_[5]->getMemBankPtr()->spill_trailers_.size() == 0 );
  CPPUNIT_ASSERT( c1->size() == 0 );
  delete c1;

  c1 = makeContainer(9, 81*4);
  eb2.process();

  CPPUNIT_ASSERT( eb2.board_event_builders_[9]->getMemBankPtr()->size() == (70-12+81)*4 + sizeof(BMDDataFragmenHeader) );
  CPPUNIT_ASSERT( eb2.board_event_builders_[9]->getMemBankPtr()->spill_headers_.size() == 1 );
  CPPUNIT_ASSERT( eb2.board_event_builders_[9]->getMemBankPtr()->spill_trailers_.size() == 0 );
  CPPUNIT_ASSERT( c1->size() == 0 );
  delete c1;

  c1 = makeContainer(5, 111*4);
  eb2.process();

  CPPUNIT_ASSERT( eb2.board_event_builders_[5]->getMemBankPtr()->size() == (79-26+111)*4 + sizeof(BMDDataFragmenHeader) );
  CPPUNIT_ASSERT( eb2.board_event_builders_[5]->getMemBankPtr()->spill_headers_.size() == 1 );
  CPPUNIT_ASSERT( eb2.board_event_builders_[5]->getMemBankPtr()->spill_trailers_.size() == 0 );
  CPPUNIT_ASSERT( c1->size() == 0 );
  delete c1;

  c1 = makeContainer(0, 97*4);
  setTrHeader(c1, 76, 0, 5);
  eb2.process();

  CPPUNIT_ASSERT( eb2.board_event_builders_[0]->getMemBankPtr()->size() == (97-76)*4 + sizeof(BMDDataFragmenHeader) );
  CPPUNIT_ASSERT( eb2.board_event_builders_[0]->getMemBankPtr()->spill_headers_.size() == 1 );
  CPPUNIT_ASSERT( eb2.board_event_builders_[0]->getMemBankPtr()->spill_trailers_.size() == 0 );
  CPPUNIT_ASSERT( c1->size() == 0 );
  delete c1;

  c1 = makeContainer(9, 97*4);
  setTrTrailer(c1, 36, 9, 5);
  eb2.process();

  unsigned int spillSize1 = eb2.spill_mem_bank_->size();
  CPPUNIT_ASSERT( eb2.spill_mem_bank_->header_->n_fragments_ == 1 );
  CPPUNIT_ASSERT( spillSize1 == (70+81+36-12+1)*4 + sizeof(BMDDataFragmenHeader) + sizeof(BMDEventHeader) );
  CPPUNIT_ASSERT( eb2.spill_mem_bank_->spill_headers_.size() == 1);
  CPPUNIT_ASSERT( eb2.spill_mem_bank_->spill_trailers_.size() == 1);
  CPPUNIT_ASSERT( eb2.board_event_builders_[9]->getMemBankPtr()->size() ==  eb2.board_event_builders_[9]->getMemBankPtr()->header_->size_);
  CPPUNIT_ASSERT( eb2.board_event_builders_[9]->getMemBankPtr()->size() == sizeof(BMDDataFragmenHeader) );
  CPPUNIT_ASSERT( *eb2.spill_mem_bank_->get32bWordPtr( sizeof(BMDDataFragmenHeader)/4.+sizeof(BMDEventHeader)/4.  ) == 0x01200005 );
  CPPUNIT_ASSERT( *eb2.spill_mem_bank_->get32bWordPtr( eb2.spill_mem_bank_->spill_headers_[0]/4 )  == 0x01200005 );
  CPPUNIT_ASSERT( *eb2.spill_mem_bank_->get32bWordPtr( eb2.spill_mem_bank_->spill_trailers_[0]/4 ) == 0x70000001 );
  CPPUNIT_ASSERT( *eb2.spill_mem_bank_->get32bWordPtr(spillSize1/4.-3) == 0x61200005 );
  CPPUNIT_ASSERT( *eb2.spill_mem_bank_->get32bWordPtr(spillSize1/4.-2) == 0x61200000 );
  CPPUNIT_ASSERT( *eb2.spill_mem_bank_->get32bWordPtr(spillSize1/4.-1) == 0x70000001 );
  CPPUNIT_ASSERT( c1->size() == 0 );
  delete c1;

  c1 = makeContainer(5, 59*4);
  setTrTrailer(c1, 43, 5, 5);
  eb2.process();

  unsigned int spillSize2 = eb2.spill_mem_bank_->size();
  CPPUNIT_ASSERT( eb2.spill_mem_bank_->header_->n_fragments_ == 2 );
  CPPUNIT_ASSERT( spillSize2 == spillSize1 + (79+111+43-26+1)*4 + sizeof(BMDDataFragmenHeader) );
  CPPUNIT_ASSERT( eb2.board_event_builders_[5]->getMemBankPtr()->size() == sizeof(BMDDataFragmenHeader) );
  CPPUNIT_ASSERT( eb2.board_event_builders_[5]->getMemBankPtr()->size() ==  eb2.board_event_builders_[5]->getMemBankPtr()->header_->size_);
  CPPUNIT_ASSERT( eb2.spill_mem_bank_->spill_headers_.size() == 2);
  CPPUNIT_ASSERT( eb2.spill_mem_bank_->spill_trailers_.size() == 2);
  CPPUNIT_ASSERT( eb2.spill_mem_bank_->spill_headers_[0] != eb2.spill_mem_bank_->spill_headers_[1] );
  CPPUNIT_ASSERT( eb2.spill_mem_bank_->spill_trailers_[0] != eb2.spill_mem_bank_->spill_headers_[1] );
  CPPUNIT_ASSERT( *eb2.spill_mem_bank_->get32bWordPtr( eb2.spill_mem_bank_->spill_headers_[1]/4 )  == 0x00a00005 );
  CPPUNIT_ASSERT( *eb2.spill_mem_bank_->get32bWordPtr( eb2.spill_mem_bank_->spill_trailers_[1]/4 ) == 0x70000001 );

  CPPUNIT_ASSERT( *eb2.spill_mem_bank_->get32bWordPtr(spillSize1/4.+ sizeof(BMDDataFragmenHeader)/4 ) == 0x00a00005 );
  CPPUNIT_ASSERT( *eb2.spill_mem_bank_->get32bWordPtr( eb2.spill_mem_bank_->spill_headers_[1]/4 ) == 0x00a00005 );
  CPPUNIT_ASSERT( *eb2.spill_mem_bank_->get32bWordPtr(spillSize2/4.-3) == 0x60a00005 );
  CPPUNIT_ASSERT( *eb2.spill_mem_bank_->get32bWordPtr(spillSize2/4.-2) == 0x60a00000 );
  CPPUNIT_ASSERT( *eb2.spill_mem_bank_->get32bWordPtr(spillSize2/4.-1) == 0x70000001 );
  delete c1;

  c1 = makeContainer(0, 59*4);
  setTrTrailer(c1, 4, 0, 5);
  eb2.process();

  CPPUNIT_ASSERT( eb2.spill_mem_bank_->header_->n_fragments_ == 0 );
  CPPUNIT_ASSERT( eb2.spill_mem_bank_->size() == sizeof(BMDEventHeader) );

  eb2.close(proc_status_t::OK_s);
}

void TestWorkers::TestEBProcessCase2() {
  BMDEventBuilder eb3;
  eb3.recorder().setRunNum(2999);

  UFEDataContainer *c1 = makeContainer(9, 70*4);
  eb3.setOutputObj(&c1);
  eb3.init();

  setTrHeader(c1, 12, 9, 6);
  setTrTrailer(c1, 17, 9, 6);
  eb3.process();
  delete c1;

  c1 = makeContainer(5, 70*4);
  setTrHeader(c1, 12, 5, 6);
  setTrTrailer(c1, 17, 5, 6);
  eb3.process();
  delete c1;

  c1 = makeContainer(0, 70*4);
  setTrHeader(c1, 12, 0, 6);
  setTrTrailer(c1, 17, 0, 6);
  eb3.process();
  delete c1;

  c1 = makeContainer(0, 70*4);
  setTrHeader(c1, 12, 9, 7);
  setTrTrailer(c1, 17, 9, 7);
  eb3.process();
  delete c1;

  c1 = makeContainer(5, 70*4);
  setTrHeader(c1, 12, 5, 7);
  setTrTrailer(c1, 17, 5, 7);
  eb3.process();
  delete c1;

  c1 = makeContainer(0, 70*4);
  setTrHeader(c1, 12, 0, 7);
  setTrTrailer(c1, 17, 0, 7);
  eb3.process();
  delete c1;


  eb3.close(proc_status_t::OK_s);
}