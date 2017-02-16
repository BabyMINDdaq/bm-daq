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
 *  \file    BMDWorkers.cpp
 *  \brief   File containing implementations of the Baby MIND DAQ workers.
 *  \author  Yordan Karadzhov
 *  \date    Nov 2016
 */

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <memory>

#include "BMDWorkers.h"
#include "UFEError.h"
#include "UFEContext.h"

using namespace std;

void BMDReadout::init() {
  if (!device_->isOpen())
    device_->init();
}

proc_status_t BMDReadout::process() {
  // UFEDevice::getData does not throw exceptions, so no try/catch module here.
  device_->getData(*output_, &this->read_size_);
  if(this->device_->status() == 0 && this->read_size_ != 0)
    return proc_status_t::OK_s;

  // Something is wrong. Stop the work.
  ufe_error_print("readout error (status: %i, read size: %i", device_->status(), read_size_);
  return proc_status_t::Error_s;
}

void BMDReadout::close(proc_status_t st) {
//   device_->close();
}

void BMDDataScanner::init() {
  trigger_tag_ = 0;
}

proc_status_t BMDDataScanner::process() {
  uint64_t nDw = (**output_).size()/4;
  if (nDw == 0) {
    ufe_error_print("empty data container.");
    return proc_status_t::Error_s;
  }

  uint32_t *data = (**output_).get32bWordPtr(0);
  trigger_tag_ = 0;
  uint64_t xDw=0;
  while (trigger_tag_ == 0 && xDw<nDw) {
    this->read(data, xDw);
    ++xDw;
  }

//   ufe_error_print("first trigger: %i", trigger_tag_);

  while (xDw<nDw) {
    proc_status_t status = this->readAndCheck(data, xDw);
    if (status != proc_status_t::OK_s)
      return status;

    ++xDw;
  }

  return proc_status_t::OK_s;
}

void BMDDataScanner::close(proc_status_t st) {}

void BMDDataScanner::read(uint32_t *data, uint64_t dwId) {
  data_word_.SetDataPtr(data + dwId);

  switch (data_word_.GetDataType()) {
    case MDdataWordBM::TrigHeader :
      trigger_tag_ = data_word_.GetTriggerTag();
      break;

    case MDdataWordBM::SpillHeader :
      (**output_).spill_headers_.push_back(dwId*4);
//       cerr << "@ " << this->getCount() << endl;
//       cerr << data_word_ << endl;
      break;

    case MDdataWordBM::SpillTrailer2 :
      (**output_).spill_trailers_.push_back(dwId*4);
//       cerr << "@ " << this->getCount() << endl;
//       cerr << data_word_ << endl;
      break;

    default:
      break;
  }
}

proc_status_t BMDDataScanner::readAndCheck(uint32_t *data, uint64_t dwId) {
  data_word_.SetDataPtr(data + dwId);

  switch (data_word_.GetDataType()) {
    case MDdataWordBM::TrigHeader :
      trigger_tag_ = data_word_.GetTriggerTag();
      break;

    case MDdataWordBM::TrigTrailer1 :
      if (trigger_tag_ != data_word_.GetTriggerTag()) {
        ufe_error_print("Trigger tag mismatch ( %i != %i )", trigger_tag_, data_word_.GetTriggerTag());
//         return proc_status_t::Error_s;
      }

      break;

    case MDdataWordBM::SpillHeader :
      (**output_).spill_headers_.push_back(dwId*4);
//       cerr << "@@ " << this->getCount() << endl;
//       cerr << data_word_ << endl;
      break;

    case MDdataWordBM::SpillTrailer2 :
      (**output_).spill_trailers_.push_back(dwId*4);
//       cerr << "@@ " << this->getCount() << endl;
//       cerr << data_word_ << endl;
      break;

    default:
      break;
  }

  return proc_status_t::OK_s;
}

BMDEventBuilder::~BMDEventBuilder() {
  board_event_builders_.clear();

  if (spill_mem_bank_)
    delete spill_mem_bank_;
}

void BMDEventBuilder::init() {
  recorder_.init();
  UFEContext *ctx = UFEContext::instance();
  size_t xMemSize = ctx->ufec_ctx_->readout_buffer_size_;

  spill_mem_bank_ = new BMDEventMemBank(xMemSize*40000);

  for (auto const & dev: ctx->devs_)
    for (auto const & b: dev.boards()) {
      board_event_builders_[b] = std::make_shared<DataFragmentBuilder>(b, xMemSize*2000);
      board_event_builders_[b]->setInput(output_);
      board_event_builders_[b]->setOutput(&spill_mem_bank_);
    }
}

proc_status_t BMDEventBuilder::process() {
  // Exceptions may be thrown here.
  try {
    int boardId = (**output_).boardId();

    int nSpillHeaders = (**output_).spill_headers_.size();
    int nSpillTrailers = (**output_).spill_trailers_.size();

    MDdataWordBM data_word_;

    if (nSpillTrailers) {
      for (auto const & t: (**output_).spill_trailers_) {
        data_word_.SetDataPtr( (**output_).buffer() + t - 8);
        cerr << "$$1 " << data_word_ << endl;

        data_word_.SetDataPtr( (**output_).buffer() + t - 4);
        cerr << "$$2 " << data_word_ << endl;

        data_word_.SetDataPtr( (**output_).buffer() + t );
        cerr << "$$3 " << data_word_ << endl;
      }

      cerr << "\n\n\n";
    }

    if (nSpillHeaders) {
      for (auto const & h: (**output_).spill_headers_) {
        data_word_.SetDataPtr( (**output_).buffer() + h );
        cerr << "$$ " << data_word_ << endl;
      }
    }

    // Add the incoming data to the memory bank of the corresponding fragment builder.
    // If the fragment is completed, the fragment builder will move the data to the
    // memory bank of the spill and will reset the fragment bank.
    board_event_builders_[boardId]->process();

    // If all fragments are completed, write the data and reset the spill bank.
    if (spill_mem_bank_->header_->n_fragments_ == board_event_builders_.size()) {
      spill_mem_bank_->header_->time_     = time(nullptr);
      spill_mem_bank_->header_->event_id_ = event_id_++;
      recorder_.write(spill_mem_bank_);
      spill_mem_bank_->clear();
    }
  } catch (UFEError &e) {
    cerr << e.getDescription() << endl;
    cerr << e.getLocation() << endl;
    // Change the status according to the severity of the error.
    if (e.getSeverity() == UFEError::SERIOUS)
      return proc_status_t::Error_s;
    else if (e.getSeverity() == UFEError::FATAL)
      return proc_status_t::FatalError_s;
  }

  // Job done. Recycle the container.
  (**output_).clear();
  return proc_status_t::OK_s;
}

void BMDEventBuilder::close(proc_status_t st) {
  recorder_.close();
  board_event_builders_.clear();

  delete spill_mem_bank_;
  spill_mem_bank_ = nullptr;
}
