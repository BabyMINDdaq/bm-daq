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
 *  \created  Nov 2016
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

class BMDReadout : public InProcessor<UFEDataContainer> {
public:
  BMDReadout() : InProcessor("BMDReadout") {}

  void           init() override;
  proc_status_t  process() override;
  void           close(proc_status_t st) override;

  void setDevice(UFEDevice *d) {device_ = d;}
private:
  UFEDevice *device_;
  int read_size_;
};

IMPLEMENT_UWORKER(BMDReadoutWorker, UFEDataContainer, BMDReadout)   // worker name, data type, processor name


class BMDDataScanner : public InProcessor<UFEDataContainer> {
public:
  BMDDataScanner() : InProcessor("BMDDataScanner") {}

  void           init() override;
  proc_status_t  process() override;
  void           close(proc_status_t st) override;

private:
  void read(uint32_t *data, uint64_t dwId);
  void readAndCheck(uint32_t *data, uint64_t dwId);

  MDdataWordBM data_word_;
  unsigned int trigger_tag_;
};

IMPLEMENT_UWORKER(BMDDataScannerWorker, UFEDataContainer, BMDDataScanner)   // worker name, data type, processor name


class BMDEventBuilder : public InProcessor<UFEDataContainer> {
public:
  BMDEventBuilder()
  : InProcessor("BMDEventBuilder"), spill_mem_bank_(nullptr), event_id_(0) {}

  virtual ~BMDEventBuilder();

  void           init() override;
  proc_status_t  process() override;
  void           close(proc_status_t st) override;

  UFEDataRecorder& recorder() {return recorder_;};

  std::map< int, std::shared_ptr<DataFragmentBuilder> >  board_event_builders_;
  BMDEventMemBank                    *spill_mem_bank_;

private:
  uint32_t        event_id_;
  UFEDataRecorder recorder_;

};

IMPLEMENT_UWORKER(BMDEventBuilderWorker, UFEDataContainer, BMDEventBuilder)   // worker name, data type, processor name

#endif

