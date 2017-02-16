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
 *  \file    BMDWorkers.h
 *  \brief   File containing declarations of the Baby MIND DAQ workers.
 *  \author  Yordan Karadzhov
 *  \date    Nov 2016
 */

#ifndef UFE_WORKERS_H
#define UFE_WORKERS_H 1

// C++
#include <fstream>

// mic11api
#include "Worker.h"
#include "Macros.h"

// bm-unpack
#include "MDdataWordBM.h"

// libufecpp
#include "UFEDevice.h"
#include "UFEDataRecorder.h"

// bm-daq
#include "BMDDataLib.h"

/** @class BMDReadout
 *  Processor class for readout of the binary data over USB.
 */
class BMDReadout : public InProcessor<UFEDataContainer> {
public:
  BMDReadout() : InProcessor("BMDReadout") {}

  /** \brief Sets up the processor. */
  void init() override;

  /** \brief Receives data from the front-end board. */
  proc_status_t process() override;

  /** \brief Shutdowns the processor. */
  void close(proc_status_t st) override;

  /** \brief Sets a pointer to the UFEDevice used by the processor. */
  void setDevice(UFEDevice *d) {device_ = d;}

private:
  UFEDevice *device_;
  int read_size_;
};


/** @class BMDDataScanner
 *  Processor class for scanning of the raw binary data.
 */
class BMDDataScanner : public InProcessor<UFEDataContainer> {
public:
  /** Construct. */
  BMDDataScanner() : InProcessor("BMDDataScanner") {}

  /** \brief Sets up the processor. */
  void init() override;

  /** \brief Scans one data container. */
  proc_status_t process() override;

  /** \brief Shutdowns the processor. */
  void  close(proc_status_t st) override;

private:
  void read(uint32_t *data, uint64_t dwId);
  proc_status_t readAndCheck(uint32_t *data, uint64_t dwId);

  MDdataWordBM data_word_;
  unsigned int trigger_tag_;
};



/** @class BMDEventBuilder
 *  Adds the data of one data container to the current DAQ event.
 *  The information provided by the  Data Scanner worker is used in order to build the event properly.
 */
class BMDEventBuilder : public InProcessor<UFEDataContainer> {
public:
  /** Construct. */
  BMDEventBuilder()
  : InProcessor("BMDEventBuilder"), spill_mem_bank_(nullptr), event_id_(0) {}

  virtual ~BMDEventBuilder();

  /** \brief Sets up the processor. */
  void init() override;

  /** \brief Adds the data of one data container to the current DAQ event. */
  proc_status_t process() override;

  /** \brief Shutdowns the processor. */
  void close(proc_status_t st) override;

  /** \brief Returns a reference to the UFEDataRecorder object.*/
  UFEDataRecorder& recorder() {return recorder_;};

  /** \brief Map of the Fragment Builders. */
  std::map< int, std::shared_ptr<DataFragmentBuilder> >  board_event_builders_;

  /** \brief Local Memory bank used to build the event. */
  BMDEventMemBank *spill_mem_bank_;

private:
  uint32_t        event_id_;
  UFEDataRecorder recorder_;

};


IMPLEMENT_UWORKER(BMDDataScannerWorker, UFEDataContainer, BMDDataScanner)   // worker name, data type, processor name
IMPLEMENT_UWORKER(BMDReadoutWorker, UFEDataContainer, BMDReadout)   // worker name, data type, processor name
IMPLEMENT_UWORKER(BMDEventBuilderWorker, UFEDataContainer, BMDEventBuilder)   // worker name, data type, processor name

#endif

