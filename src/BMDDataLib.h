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

#ifndef BMD_DATA_LIB_H
#define BMD_DATA_LIB_H 1

#include <cstdint>
#include <iostream>
#include <ctime>

// bm-unpack
#include "MDdataWordBM.h"

// libufecpp
#include "UFEDataContainer.h"

#include "BMDDataHeaders.h"

void header_clear(BMDDataFragmenHeader* h);
void header_clear(BMDEventHeader* h);

template<typename HeaderType>
class BMDMemBank : public UFEDataContainer {
public:
  BMDMemBank();
  BMDMemBank(size_t s);
  BMDMemBank(const BMDMemBank &b);
  BMDMemBank(BMDMemBank &&b);

  BMDMemBank& operator=(const BMDMemBank &b);
  BMDMemBank& operator=(BMDMemBank &&b);

  MDdataWordBM& getDataWord(int s)  {
    data_word_.SetDataPtr( this->get32bWordPtr(s/4) );
    return data_word_;
  }

  void clear();

  HeaderType   *header_;
  MDdataWordBM  data_word_;
};

#include "BMDDataLib-inl.h"

typedef BMDMemBank<BMDDataFragmenHeader> BMDBoardMemBank;
typedef BMDMemBank<BMDEventHeader>       BMDEventMemBank;

class DataFragmentBuilder {
public:
  DataFragmentBuilder() = delete;
  DataFragmentBuilder(int id, size_t memsize) : board_id_(id), mem_bank_(memsize)  {}

  int  boardId()   const  {return board_id_;}
  void setBoardId( int b) {board_id_ = b;}

  void setInput(UFEDataContainer  **i) {in_data_ = i;}
  void process();

  void setOutput(BMDEventMemBank **o)  {out_data_ = o;}
  BMDBoardMemBank* getMemBankPtr()     {return &mem_bank_;}

private:
  void moveToEventBank();

  int board_id_;
  UFEDataContainer  **in_data_;
  BMDEventMemBank   **out_data_;
  BMDBoardMemBank     mem_bank_;
};

#endif




