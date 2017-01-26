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

#include "UFEError.h"

#include "BMDDataLib.h"

void header_clear(BMDDataFragmenHeader* h) {
  h->size_ = sizeof(BMDDataFragmenHeader);
}

void header_clear(BMDEventHeader* h) {
  h->size_ = sizeof(BMDEventHeader);
  h->n_fragments_ = 0;
  h->n_subevents_ = 0;
  h->type_ = 0;
}

void DataFragmentBuilder::process() {
  // Check the board Id of the data in the container.
  if ( (**in_data_).boardId() != this->board_id_) {
    throw UFEError( "Data corruption (wrong board Id).",
                    "void DataFragmentBuilder::process()",
                    UFEError::FATAL);
  }

  // For the moment the case of multiple spills contained in the input stream is not supported.
  // In principle this should never happen, unless the data container size is huge.
  // TODO Implement the multi-spill case.
  if ( (**in_data_).spill_headers_.size() > 1 ||
       (**in_data_).spill_trailers_.size() > 1 ||
          mem_bank_.spill_trailers_.size() > 0 ) {
    throw UFEError( "Data corruption (multiple spills).",
                    "void DataFragmentBuilder::process()",
                    UFEError::FATAL);
  }

  size_t xSize;
  if ( mem_bank_.spill_headers_.size() == 0 ) {
    // No spill data so far. We expect a Spill header.

    if ( (**in_data_).spill_headers_.size() != 0 ) {
      // We have the header.
      if ( (**in_data_).spill_trailers_.size() == 0 ) {
        // This container has 1 header and 0 trailers.
        // Ignor the data befor the header.
        xSize = (**in_data_).size() - (**in_data_).spill_headers_[0];
        mem_bank_.append( **in_data_, (**in_data_).spill_headers_[0], xSize );
        mem_bank_.header_->size_ += xSize;
        return;
      } else if ( (**in_data_).spill_trailers_[0] >= (**in_data_).spill_headers_[0] ) {
        // This container has 1 header and 1 trailer, and the trailer comes after the header.
        // Ignor the data befor the header and after the trailer.
        xSize = (**in_data_).spill_trailers_[0] - (**in_data_).spill_headers_[0] + 4;
        mem_bank_.append( **in_data_, (**in_data_).spill_headers_[0], xSize );
        mem_bank_.header_->size_ += xSize;
        // Move the completed fragment to the Event memory bank.
        moveToEventBank();
        return;
      } else {
        // This container has 1 header and 1 trailer, but the trailer comes before the header.
        // TODO this case must be treated as error, but for the moment we will ignore the data
        // before the header. This is done due to the existing bug in the firmware which affects
        // the first spill of the run. When the bug is fixed, remove the whole "else" block.
        xSize = (**in_data_).size() - (**in_data_).spill_headers_[0];
        mem_bank_.append( **in_data_, (**in_data_).spill_headers_[0], xSize );
        mem_bank_.header_->size_ += xSize;
        return;
      }
    } else if ( (**in_data_).spill_headers_.size() == 0 &&
                (**in_data_).spill_trailers_.size() == 0 ) {
      // No spill data in this contained.
      return;
    }
  } else if ( mem_bank_.spill_headers_.size() == 1 ) {
    // We are in the middle of the spill. We expect a Spill trailer.

    if ( (**in_data_).spill_headers_.size() == 0 &&
         (**in_data_).spill_trailers_.size() == 0 ) {
      // No trailer in this container. Append the whole container.
      mem_bank_.append( **in_data_ );
      mem_bank_.header_->size_ += (**in_data_).size();
      return;
    } else if ( (**in_data_).spill_headers_.size() == 0 &&
                (**in_data_).spill_trailers_.size() == 1) {
      // This spill ends here, but the next one has not started yet.
      size_t xSize = (**in_data_).spill_trailers_[0] + 4;
      mem_bank_.append( **in_data_, 0, xSize );
      mem_bank_.header_->size_ += xSize;

      // Move the completed fragment to the Event memory bank.
      moveToEventBank();
      return;
    } else if ( (**in_data_).spill_headers_.size() == 1 &&
                (**in_data_).spill_trailers_.size() == 1 &&
                (**in_data_).spill_trailers_[0] <= (**in_data_).spill_headers_[0] ) {
      // This spill ends here and the next one starts after this.
      size_t xSize = (**in_data_).spill_trailers_[0] + 4;
      mem_bank_.append( **in_data_, 0, xSize );
      mem_bank_.header_->size_ += xSize;

      // Move the completed fragment to the Event memory bank.
      moveToEventBank();

      // Add the data fron the following spill to the local memory bank which is now empty.
      xSize = (**in_data_).size() - (**in_data_).spill_headers_[0];
      mem_bank_.append( **in_data_, (**in_data_).spill_headers_[0], xSize );
      mem_bank_.header_->size_ += xSize;
      return;
    }
  }

  // If you are here, this means that something is wrong!!!
  throw UFEError( "Data corruption (inconsistent data).",
                  "void DataFragmentBuilder::process()",
                  UFEError::FATAL);
}

void DataFragmentBuilder::moveToEventBank() {
  MDdataWordBM h_dw;
  // Get the header data word.
  h_dw.SetDataPtr( mem_bank_.buffer() + mem_bank_.spill_headers_[0] );

  MDdataWordBM t_dw;
  // Get the first data word of the trailer.
  // Remember that the trailer contains 3 data words.
  t_dw.SetDataPtr( mem_bank_.buffer() + mem_bank_.spill_trailers_[0] -8 );

  // Confirm that the header and the trailer are consistent.
  if ( ( h_dw.GetSpillTag() != t_dw.GetSpillTag() ) ||
       ( h_dw.GetBoardId()  != t_dw.GetBoardId() ) ) {
    throw UFEError( "Data corruption (spill data mismatch).",
                    "void DataFragmentBuilder::moveToEventBank()",
                    UFEError::FATAL);
  }

  cerr << "moveToEventBank tag: " << h_dw.GetSpillTag() << " / " <<  t_dw.GetSpillTag() << endl;
  (**out_data_).append(this->mem_bank_);
  (**out_data_).header_->size_ += this->mem_bank_.size();
  (**out_data_).header_->n_fragments_++;

  this->mem_bank_.clear();
}


