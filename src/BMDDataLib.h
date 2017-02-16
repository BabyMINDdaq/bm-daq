/* This file is part of BabyMINDdaq software package. This software
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
 * along with BabyMINDdaq. If not, see <http://www.gnu.org/licenses/>.
 */


/**
 *  \file    BMDDataLib.h
 *  \brief   File containing declarations of the classes managing
 *  the Baby MIND DAQ binary data.
 *  \author  Yordan Karadzhov
 *  \date    Dec 2016
 */

#ifndef BMD_DATA_LIB_H
#define BMD_DATA_LIB_H 1

// C++
#include <cstdint>
#include <iostream>
#include <ctime>

// bm-unpack
#include "MDdataWordBM.h"

// libufecpp
#include "UFEDataContainer.h"

// bm-daq
#include "BMDDataHeaders.h"

/** \brief The function resets the BMDDataFragmenHeader. */
void header_clear(BMDDataFragmenHeader* h);

/** \brief The function resets the BMDEventHeader. */
void header_clear(BMDEventHeader* h);

/** @class BMDMemBank
 *  Abstract class that inherits from and provides data headers.
 *  The class is used when building the DAQ event.
 */
template<typename HeaderType>
class BMDMemBank : public UFEDataContainer {
public:

  /** \brief Default construct. */
  BMDMemBank();

  /** \brief Construct.
   *  \param s: Size of the Memory bank.
   */
  BMDMemBank(size_t s);

  /** \brief Copy construct. */
  BMDMemBank(const BMDMemBank &b);

  /** \brief Move construct. */
  BMDMemBank(BMDMemBank &&b);

  /** \brief Copy assignment operator */
  BMDMemBank& operator=(const BMDMemBank &b);

  /** \brief Move assignment operator */
  BMDMemBank& operator=(BMDMemBank &&b);

  /** \brief Provide access to the Data word object.
   *  \param pos: Position of the Data word in the bank.
   */
  MDdataWordBM& getDataWord(size_t pos)  {
    data_word_.SetDataPtr( this->get32bWordPtr(pos/4) );
    return data_word_;
  }

  /** \brief Resets the Memory bank. */
  void clear();

  /** \brief Appends the entire content of a UFEDataContainer to the Memory bank. */
  void append(const UFEDataContainer &c);

  /** \brief Appends the part of a UFEDataContainer to the Memory bank. */
  void append(const UFEDataContainer &c, unsigned int pos, size_t size);

  /**  Pointer, providing access to the header of the bank. */
  HeaderType   *header_;

  /** Data word object. */
  MDdataWordBM  data_word_;
};

#include "BMDDataLib-inl.h"

/** The type of the Fragmen Memory bank. */
typedef BMDMemBank<BMDDataFragmenHeader> BMDBoardMemBank;

/** The type of the Event Memory bank. */
typedef BMDMemBank<BMDEventHeader>       BMDEventMemBank;

/** @class DataFragmentBuilder
 *  The class is used to build the data fragment of one front-end board.
 */
class DataFragmentBuilder {
public:
  DataFragmentBuilder() = delete;

  /** \brief Construct.
   *  \param id: Id number of the front-end board.
   *  \param memsize: Size of the Memory bank.
   */
  DataFragmentBuilder(int id, size_t memsize) : board_id_(id), mem_bank_(memsize)  {}

  /** Returns the Id number of the front-end board. */
  int  boardId()   const  {return board_id_;}

  /** Sets the Id number of the front-end board. */
  void setBoardId( int b) {board_id_ = b;}

  /** Sets the data input of the builder. */
  void setInput(UFEDataContainer  **i) {in_data_ = i;}

  /** Sets the data output of the builder. */
  void setOutput(BMDEventMemBank **o)  {out_data_ = o;}

  /** Processes the data, available at the input location. If the building is completed,
   * the event is available at the output location.
   */
  void process();

  /** Provides access to the local Memory bank. */
  BMDBoardMemBank* getMemBankPtr()     {return &mem_bank_;}

private:
  void moveToEventBank();

  /** Id number of the front-end board. */
  int board_id_;


  UFEDataContainer  **in_data_;
  BMDEventMemBank   **out_data_;
  BMDBoardMemBank     mem_bank_;
};

#endif




