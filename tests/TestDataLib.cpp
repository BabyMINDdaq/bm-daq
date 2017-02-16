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

#include "UFEError.h"
#include "UFEContext.h"
#include "TestDataLib.h"
#include "FakeData.h"

using namespace std;

void TestDataLib::setUp() {
  
}

void TestDataLib::tearDown() {
  
}

void TestDataLib::TestFragment() {
  BMDBoardMemBank fr1;
  CPPUNIT_ASSERT( fr1.header_->size_ == sizeof(BMDDataFragmenHeader) );
  CPPUNIT_ASSERT( fr1.header_->size_ == fr1.size() );

  uint8_t data[64];
  for (int i=0; i<64; ++i)
    data[i] = i;


  fr1.UFEDataContainer::append(data, 64);
  fr1.header_->size_ += 64;
  CPPUNIT_ASSERT( fr1.header_->size_ == fr1.size() );

  setTrHeader(&fr1, 3, 9, 5);
  fr1.spill_headers_.push_back(3*4);
//   cerr << "\n@@ tag: " << (fr1.getDataWord( fr1.spill_headers_[0] )).GetSpillTag();

  BMDBoardMemBank fr2(fr1);
  fr1.header_->size_ = 128;

  CPPUNIT_ASSERT( fr2.header_->size_ == fr2.size() );
  CPPUNIT_ASSERT( fr2.header_->size_ == sizeof(BMDDataFragmenHeader) + 64 );
  CPPUNIT_ASSERT( fr2.buffer()[fr2.size()-1] == 63 );

  uint8_t *b_ptr = fr2.buffer();
  BMDDataFragmenHeader *h_ptr = fr2.header_;

  BMDBoardMemBank fr3 = std::move(fr2);
  CPPUNIT_ASSERT( fr2.header_  == nullptr );
  CPPUNIT_ASSERT( fr2.buffer() == nullptr );
  CPPUNIT_ASSERT( fr3.header_ == h_ptr );
  CPPUNIT_ASSERT( fr3.buffer() == b_ptr );
  CPPUNIT_ASSERT( fr3.size() == sizeof(BMDDataFragmenHeader) + 64 );
  CPPUNIT_ASSERT( fr3.header_->size_ == fr3.size() );
  CPPUNIT_ASSERT( fr2.size() == 0 );

  fr3.clear();
  CPPUNIT_ASSERT( fr3.header_ == h_ptr );
  CPPUNIT_ASSERT( fr3.buffer() == b_ptr );
  CPPUNIT_ASSERT( fr3.size() == sizeof(BMDDataFragmenHeader));
  CPPUNIT_ASSERT( fr3.header_->size_ == fr3.size() );

  BMDBoardMemBank fr4(64);
  CPPUNIT_ASSERT( fr4.memsize() == 64);
  CPPUNIT_ASSERT( fr4.header_->size_ == sizeof(BMDDataFragmenHeader) );
  CPPUNIT_ASSERT( fr4.header_->size_ == fr4.size() );
}

void TestDataLib::TestEvent() {
  BMDEventMemBank ev1;
  CPPUNIT_ASSERT( ev1.header_->size_ == sizeof(BMDEventHeader) );
  CPPUNIT_ASSERT( ev1.header_->size_ == ev1.size() );
  CPPUNIT_ASSERT( ev1.header_->n_fragments_ == 0 );
  CPPUNIT_ASSERT( ev1.header_->n_subevents_ == 0 );
  CPPUNIT_ASSERT( ev1.header_->isSuperEvent() == false );

  ev1.header_->n_subevents_ = 3;
  ev1.header_->n_fragments_ = 1;
  ev1.header_->type_ = 1 << 7;
  CPPUNIT_ASSERT( ev1.header_->isSuperEvent() == true );

  ev1.header_->type_ = 0;
  CPPUNIT_ASSERT( ev1.header_->isSuperEvent() == false );

  uint8_t data[64];
  for (int i=0; i<64; ++i)
    data[i] = i;

  ev1.UFEDataContainer::append(data, 64);
  ev1.header_->size_ += 64;

  CPPUNIT_ASSERT( ev1.header_->size_ == ev1.size() );
  CPPUNIT_ASSERT( ev1.buffer()[ev1.size()-1] == 63 );

  BMDEventMemBank ev2(ev1);
  CPPUNIT_ASSERT( ev2.header_->size_ == ev1.size() );
  CPPUNIT_ASSERT( ev2.header_->size_ == ev2.size() );
  CPPUNIT_ASSERT( ev2.buffer()[ev2.size()-1] == 63 );
  CPPUNIT_ASSERT( ev2.header_->n_fragments_ == 1 );
  CPPUNIT_ASSERT( ev2.header_->n_subevents_ == 3 );
  CPPUNIT_ASSERT( ev2.header_->isSuperEvent() == false );

  uint8_t *b_ptr = ev1.buffer();
  BMDEventHeader *h_ptr = ev1.header_;

  BMDEventMemBank ev3(std::move(ev1));
  CPPUNIT_ASSERT( ev1.header_ == nullptr );
  CPPUNIT_ASSERT( ev1.buffer() == nullptr );
  CPPUNIT_ASSERT( ev3.header_ == h_ptr );
  CPPUNIT_ASSERT( ev3.buffer() == b_ptr );
  CPPUNIT_ASSERT( ev3.header_->size_ == ev2.size() );
  CPPUNIT_ASSERT( ev3.header_->size_ == ev3.size() );
  CPPUNIT_ASSERT( ev2.buffer()[ev2.size()-1] == 63 );
  CPPUNIT_ASSERT( ev3.header_->n_fragments_ == 1 );
  CPPUNIT_ASSERT( ev3.header_->n_subevents_ == 3 );
  CPPUNIT_ASSERT( ev3.header_->isSuperEvent() == false );

  ev3.clear();
  CPPUNIT_ASSERT( ev3.header_ == h_ptr );
  CPPUNIT_ASSERT( ev3.buffer() == b_ptr );
  CPPUNIT_ASSERT( ev3.size() == sizeof(BMDEventHeader));
  CPPUNIT_ASSERT( ev3.header_->size_ == ev3.size() );
  CPPUNIT_ASSERT( ev3.spill_headers_.size() == 0 );
  CPPUNIT_ASSERT( ev3.spill_trailers_.size() == 0 );
  CPPUNIT_ASSERT( ev3.header_->n_fragments_ == 0 );
  CPPUNIT_ASSERT( ev3.header_->n_subevents_ == 0 );

  BMDEventMemBank ev4(64);
  CPPUNIT_ASSERT( ev4.memsize() == 64);
  CPPUNIT_ASSERT( ev4.header_->size_ == sizeof(BMDEventHeader) );
  CPPUNIT_ASSERT( ev4.header_->size_ == ev4.size() );
}

void TestDataLib::TestFragmentBuilder() {
  DataFragmentBuilder fb1(9, 1024);
  BMDBoardMemBank *b = fb1.getMemBankPtr();

  CPPUNIT_ASSERT( b->memsize() == 1024 );
  BMDEventMemBank *ev_b = new BMDEventMemBank();
  fb1.setOutput( &ev_b );

  // Test empty container
  UFEDataContainer *c0 = makeContainer(9, 64*4);
  fb1.setInput( &c0 );
  fb1.process();

  CPPUNIT_ASSERT( b->size() == sizeof(BMDDataFragmenHeader) );
  CPPUNIT_ASSERT( b->header_->size_ = b->size() );
  CPPUNIT_ASSERT( b->spill_headers_.size() == 0 );
  CPPUNIT_ASSERT( b->spill_trailers_.size() == 0 );
  CPPUNIT_ASSERT( ev_b->size() == sizeof(BMDEventHeader) );


  UFEDataContainer *c1 = makeContainer(9, 64*4); // board 9, memsize 256
  setTrHeader(c1, 12, 9, 5);  // Container, pos 32b 12, board 9, spill tag 5

  fb1.setInput( &c1 );
  fb1.process();

//   for (int i=0; i<64; ++i)
//     cerr << i << " c1: "<< hex << (unsigned int) *(c1->get32bWordPtr(i)) << dec << endl;
// 
//   cerr << "\n\n\n";
//   for (unsigned int i=2; i<b->size()/4; ++i)
//     cerr << i << " fb: " << b->getDataWord( i*4 ) << dec << endl;

//   cerr << "\n b heaer: " << b->spill_headers_[0]/4 << endl;

  CPPUNIT_ASSERT( b->size() == (64-12)*4 + sizeof(BMDDataFragmenHeader) );
  CPPUNIT_ASSERT( b->header_->size_ = b->size() );
  CPPUNIT_ASSERT( b->spill_headers_.size() == 1 );
  CPPUNIT_ASSERT( *b->get32bWordPtr( b->spill_headers_[0]/4 ) == 0x1200005 );
  CPPUNIT_ASSERT( ev_b->size() == sizeof(BMDEventHeader) );

  UFEDataContainer *c2 = makeContainer(9, 64*4); // board 9, memsize 256
  delete c1;

  fb1.setInput( &c2 );
  fb1.process();

  CPPUNIT_ASSERT( b->size() == (2*64-12)*4 + sizeof(BMDDataFragmenHeader) );
  CPPUNIT_ASSERT( b->header_->size_ = b->size() );
  CPPUNIT_ASSERT( b->spill_headers_.size() == 1 );
  CPPUNIT_ASSERT( *b->get32bWordPtr( b->spill_headers_[0]/4 ) == 0x1200005 );
  CPPUNIT_ASSERT( ev_b->size() == sizeof(BMDEventHeader) );

  UFEDataContainer *c3 = makeContainer(9, 64*4); // board 9, memsize 256
  delete c2;
  setTrTrailer(c3, 17, 9, 5);  // Container, pos 32b 17, board 9, spill tag 5
  setTrHeader(c3, 25, 9, 6);  // Container, pos 32b 25, board 9, spill tag 6

  fb1.setInput( &c3 );
  fb1.process();

//   cerr << "\n\n\n";
//   for (unsigned int i=2; i<ev_b->size()/4; ++i)
//     cerr << i << " ev_b: " << ev_b->getDataWord( i*4 ) << dec << endl;
// 
//   cerr << "ev_b heaer: " << ev_b->spill_headers_[0]/4 << endl;

  CPPUNIT_ASSERT( ev_b->size() == (2*64-12+18)*4 + sizeof(BMDDataFragmenHeader) + sizeof(BMDEventHeader) );
  CPPUNIT_ASSERT( ev_b->size() == ev_b->header_->size_);
  CPPUNIT_ASSERT( ev_b->spill_headers_.size() == 1 );
  CPPUNIT_ASSERT( ev_b->spill_trailers_.size() == 1 );
//   cerr << "\nhd: " << ev_b->spill_headers_[0]/4 << "  tr: " << ev_b->spill_trailers_[0]/4 << endl;
//   cerr << "hd: 0x" << hex << (uint32_t) *ev_b->get32bWordPtr( ev_b->spill_headers_[0]/4 )
//        << "  tr: 0x" << (uint32_t) *ev_b->get32bWordPtr( ev_b->spill_trailers_[0]/4 ) << dec << endl;
  CPPUNIT_ASSERT( *ev_b->get32bWordPtr( ev_b->spill_headers_[0]/4 )  == 0x01200005 );
  CPPUNIT_ASSERT( *ev_b->get32bWordPtr( ev_b->spill_trailers_[0]/4 ) == 0x70000001 );

  CPPUNIT_ASSERT( b->size() == (64-25)*4 + sizeof(BMDDataFragmenHeader) );
  CPPUNIT_ASSERT( b->header_->size_ = b->size() );
  CPPUNIT_ASSERT( b->spill_headers_.size() == 1 );
  CPPUNIT_ASSERT( b->spill_trailers_.size() == 0 );
  CPPUNIT_ASSERT( *b->get32bWordPtr( b->spill_headers_[0]/4 ) == 0x1200006 );

  ev_b->clear();

  CPPUNIT_ASSERT( ev_b->size() == sizeof(BMDEventHeader) );
  CPPUNIT_ASSERT( ev_b->spill_headers_.size() == 0 );
  CPPUNIT_ASSERT( ev_b->spill_trailers_.size() == 0 );


  UFEDataContainer *c4 = makeContainer(9, 64*4); // board 9, memsize 256
  delete c3;
  setTrTrailer(c4, 57, 9, 6);

  fb1.setInput( &c4 );
  fb1.process();

  CPPUNIT_ASSERT( b->size() == sizeof(BMDDataFragmenHeader) );
  CPPUNIT_ASSERT( b->header_->size_ = b->size() );
  CPPUNIT_ASSERT( b->spill_headers_.size() == 0 );
  CPPUNIT_ASSERT( b->spill_headers_.size() == 0 );

  CPPUNIT_ASSERT( ev_b->size() == (64-25+58)*4 + sizeof(BMDDataFragmenHeader) + sizeof(BMDEventHeader) );
  CPPUNIT_ASSERT( ev_b->size() == ev_b->header_->size_);
  CPPUNIT_ASSERT( ev_b->spill_headers_.size() == 1 );
  CPPUNIT_ASSERT( ev_b->spill_trailers_.size() == 1 );

  delete ev_b;
}

void TestDataLib::TestErrors() {
  DataFragmentBuilder fb2(9, 1024);
  BMDBoardMemBank *b = fb2.getMemBankPtr();
  BMDEventMemBank *ev_b = new BMDEventMemBank();
  fb2.setOutput( &ev_b );

  // Test multi-spils
  UFEDataContainer *c1 = makeContainer(9, 64*4); // board 9, memsize 256
  setTrHeader(c1, 12, 9, 5);  // Container, pos 32b 12, board 9, spill tag 5
  setTrTrailer(c1, 27, 9, 5);  // Container, pos 32b 17, board 9, spill tag 5
  setTrHeader(c1, 31, 9, 6);  // Container, pos 32b 12, board 9, spill tag 5

  fb2.setInput( &c1 );

  int n_errors(0);

  try {
    fb2.process();
  } catch (UFEError &e) {
//     cerr << "\n" << e.getDescription() << endl;
    CPPUNIT_ASSERT( e.getDescription() == std::string("*** Data corruption (multiple spills). ***") );
    ++n_errors;
  }

  CPPUNIT_ASSERT( n_errors == 1);

  UFEDataContainer *c2 = makeContainer(9, 64*4); // board 9, memsize 256
  delete c1;

  // Test missing trailer
  setTrHeader(c2, 18, 9, 5);  // Container, pos 32b 12, board 9, spill tag 5
  fb2.setInput( &c2 );
  fb2.process();

  UFEDataContainer *c3 = makeContainer(9, 64*4); // board 9, memsize 256
  delete c2;

  setTrHeader(c3, 48, 9, 5);  // Container, pos 32b 12, board 9, spill tag 5
  fb2.setInput( &c3 );

  try {
    fb2.process();
  } catch (UFEError &e) {
//     cerr << e.getDescription() << endl;
    CPPUNIT_ASSERT( e.getDescription() == std::string("*** Data corruption (inconsistent data). ***") );
    ++n_errors;
  }

  CPPUNIT_ASSERT( n_errors == 2);
  b->clear();

  // Test missing header
  UFEDataContainer *c4 = makeContainer(9, 64*4); // board 9, memsize 256
  delete c3;

  setTrHeader(c4, 18, 9, 5);  // Container, pos 32b 12, board 9, spill tag 5
  fb2.setInput( &c4 );
  fb2.process();

  UFEDataContainer *c5 = makeContainer(9, 64*4); // board 9, memsize 256
  delete c4;

  setTrTrailer(c5, 7, 9, 5);  // Container, pos 32b 17, board 9, spill tag 5
  fb2.setInput( &c5 );
  fb2.process();

  UFEDataContainer *c6 = makeContainer(9, 64*4); // board 9, memsize 256
  delete c5;

  setTrTrailer(c6, 31, 9, 6);  // Container, pos 32b 12, board 9, spill tag 5
  fb2.setInput( &c6 );

  try {
    fb2.process();
  } catch (UFEError &e) {
//     cerr << e.getDescription() << endl;
    CPPUNIT_ASSERT( e.getDescription() == std::string("*** Data corruption (inconsistent data). ***") );
    ++n_errors;
  }

  CPPUNIT_ASSERT( n_errors == 3);

  // Test wrong Spill tag
  UFEDataContainer *c7 = makeContainer(9, 64*4); // board 9, memsize 256
  delete c6;

  setTrHeader(c7, 12, 9, 5);  // Container, pos 32b 12, board 9, spill tag 5
  setTrTrailer(c7, 27, 9, 7);  // Container, pos 32b 17, board 9, spill tag 7
  fb2.setInput( &c7 );

  try {
    fb2.process();
  } catch (UFEError &e) {
//     cerr << e.getDescription() << endl;
    CPPUNIT_ASSERT( e.getDescription() == std::string("*** Data corruption (spill data mismatch). ***") );
    ++n_errors;
  }

  CPPUNIT_ASSERT( n_errors == 4);
  b->clear();

  // Test wrong board Id
  UFEDataContainer *c8 = makeContainer(9, 64*4); // board 9, memsize 256
  delete c7;

  setTrHeader(c8, 12, 9, 5);  // Container, pos 32b 12, board 9, spill tag 5
  setTrTrailer(c8, 27, 4, 5);  // Container, pos 32b 17, board 4, spill tag 5
  fb2.setInput( &c8 );

  try {
    fb2.process();
  } catch (UFEError &e) {
//     cerr << e.getDescription() << endl;
    CPPUNIT_ASSERT( e.getDescription() == std::string("*** Data corruption (spill data mismatch). ***") );
    ++n_errors;
  }

  CPPUNIT_ASSERT( n_errors == 5);

  UFEDataContainer *c9 = makeContainer(7, 64*4); // board 7, memsize 256
  delete c8;
  fb2.setInput( &c9 );

  try {
    fb2.process();
  } catch (UFEError &e) {
//     cerr << e.getDescription() << endl;
    CPPUNIT_ASSERT( e.getDescription() == std::string("*** Data corruption (wrong board Id). ***") );
    ++n_errors;
  }

  CPPUNIT_ASSERT( n_errors == 6);
}



