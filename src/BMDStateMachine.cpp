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

// C++
#include <unistd.h>
#include <algorithm>

// libufec
#include "libufe.h"
#include "libufe-core.h"
#include "libufe-tools.h"


#include "BMDStateMachine.h"
#include "BMDDeff.h"
#include "UFEError.h"
#include "UFEDevice.h"
#include "UFEContext.h"
#include "UFEDeff.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////////
/*                        BMDIdle                                           */
/////////////////////////////////////////////////////////////////////////////////
void BMDIdle::whenEnteringStateDo(Fsm *sm, fsm_state_t old_state) {
  if (old_state == fsm_state_t::Undefined_s) { // This means that the program starts now.
    UFEContext *ctx = UFEContext::instance();
    try {
      // Load the configuration of the system from a Json file.
      // TODO : the Json file must be replaced by a database.
      ctx->loadBMConfig( string(BMDAQ_DIR) + string("/conf/babyMIND-config.json") );
    } catch (UFEError &e) {
      cerr << e.getDescription() << endl;
      cerr << e.getLocation() << endl;
      if (e.getSeverity() == UFEError::SERIOUS) {
        sm->changeState(fsm_state_t::Failure_s);
        return;
      } else if (e.getSeverity() == UFEError::FATAL) {
        ufe_init(&ctx->ufec_ctx_);
        sm->changeState(fsm_state_t::FatalError_s);
        return;
      }
    }

    // Initialize the ufec library for a new session.
    int status = ufe_init(&ctx->ufec_ctx_);
    if(status < 0) {
      // There was an error.
      ufe_error_print("init Error. %i", status);
      sm->changeState(fsm_state_t::FatalError_s);
      return;
    }

    // Search the list of the connected USB devices for BM front-end boards.
    size_t nFebDevs = ufe_get_bm_device_list(ctx->ufec_ctx_->usb_ctx_, &ctx->febs_);
    if (nFebDevs == 0) {
      // No UFE board have been found.
//       ufe_error_print("no UFE board found.");
      sm->changeState(fsm_state_t::FatalError_s);
      return;
    }

    if (nFebDevs==1)
      ufe_info_print("1 UFE board found.");
    else
      ufe_info_print("%i UFE boards found.", (int) nFebDevs);

    // Check if the description of the system agrees with the list of the connected
    // BM front-end boards.
    if (nFebDevs != ctx->bm_config_doc_["Devices"].size()) {
      ufe_error_print("Device description mismatch.");
      sm->changeState(fsm_state_t::FatalError_s);
      return;
    }

    try {
      ctx->devs_.resize(nFebDevs);
      int xDev =0;
      for (auto &d: ctx->devs_) {
        d.setDevice(ctx->febs_[xDev++]);
        d.init();
        if (!setDeviceDescription(&d, ctx->bm_config_doc_)) {
          sm->changeState(fsm_state_t::FatalError_s);
          return;
        }
      }
    } catch (UFEError &e) {
      cerr << e.getDescription() << endl;
      cerr << e.getLocation() << endl;
      if (e.getSeverity() == UFEError::SERIOUS) {
        sm->changeState(fsm_state_t::Failure_s);
        return;
      } else if (e.getSeverity() == UFEError::FATAL) {
        sm->changeState(fsm_state_t::FatalError_s);
        return;
      }
    }

    // Turn Off the Leds on all boards.
    status = ufe_in_session_on_all_boards_do(&led_off);
    if (status != 0) {
      sm->changeState(fsm_state_t::FatalError_s);
      return;
    }
  }
}

void BMDIdle::whenSameStateDo(Fsm *sm) {
  UFEContext *ctx = UFEContext::instance();
  try {
    // Load the configuration of the system from a Json file.
    // TODO : the Json file must be replaced by a database.
    ctx->loadBMConfig( string(BMDAQ_DIR) + string("/conf/babyMIND-config.json") );

    for (auto &d: ctx->devs_)
      if (!setDeviceDescription(&d, ctx->bm_config_doc_)) {
        sm->changeState(fsm_state_t::FatalError_s);
        return;
      }

  } catch (UFEError &e) {
    cerr << e.getDescription() << endl;
    cerr << e.getLocation() << endl;
    if (e.getSeverity() == UFEError::SERIOUS) {
      sm->changeState(fsm_state_t::Failure_s);
      return;
    } else if (e.getSeverity() == UFEError::FATAL) {
      ufe_init(&ctx->ufec_ctx_);
      sm->changeState(fsm_state_t::FatalError_s);
      return;
    }
  }
}

void BMDIdle::whenLeavingStateDo(Fsm *sm, fsm_state_t new_state) {
  if (new_state == fsm_state_t::Undefined_s) {  // This means that the program is closing now.
    UFEContext *ctx = UFEContext::instance();

    // Close all BM front-end boards.
    for (auto &d: ctx->devs_)
      if (d.isOpen())
        d.close();

    // Turn On the Leds on all boards.
    int status = ufe_in_session_on_all_boards_do(&led_on);
    if (status != 0) {
      sm->changeState(fsm_state_t::FatalError_s);
      return;
    }

    // Free the device list and close ufec session.
    ufe_free_device_list(ctx->febs_, 1);
    ufe_exit(ctx->ufec_ctx_);

    // Update the Json file (configuration of the system).
    // This is mostly in order to store the number of the last run.
    // TODO : the Json file must be replaced by a database.
    ctx->updateBMConfig( string(BMDAQ_DIR) + string("/conf/babyMIND-config.json") );
  }
}

bool BMDIdle::setDeviceDescription(UFEDevice *dev, Json::Value bm_config_doc_) {
  for (auto const &d: bm_config_doc_["Devices"]) {
    if (d["Boards"].size() != dev->boards().size())
      continue;

    vector<int> b_ids;
    for (auto const &b: d["Boards"])
      b_ids.push_back(b["Id"].asInt());

    sort(b_ids.begin(), b_ids.end(), greater<int>());
    if (b_ids == dev->boards()) {
      dev->device_descr_ = d;
      return true;
    }
  }

  ufe_error_print( "no description found for device connecting boards %s",
                   dev->boardsToString().c_str());
  return false;
}

/////////////////////////////////////////////////////////////////////////////////
/*                        BMDStandby                                           */
/////////////////////////////////////////////////////////////////////////////////
std::shared_ptr< PtrFifo<UFEDataContainer*> > BMDStandby::recycled_containers_fifo_(nullptr);
BMDEventBuilder* BMDStandby::rec_proc_ptr_(nullptr);

void BMDStandby::whenEnteringStateDo(Fsm *sm, fsm_state_t old_state) {
  int maxSize(20);
  try {
    if (old_state == fsm_state_t::Idle_s) { //  This means that the system is turning ON.
      UFEContext *ctx = UFEContext::instance();
      // Configurat all BM front-end boards.
      for (auto &dev: ctx->devs_) {
        for (auto const &board: dev.device_descr_["Boards"]) {
          if (!dev.isOpen())
            dev.init();

          int board_id = board["Id"].asInt();
          string board_config_file(UFEAPI_DIR);
          board_config_file += "/conf/";
          if (board.isMember("ConfigurationBinary") && board["ConfigurationBinary"].asString() != "") {
            // Use binary file to configure this board.
            board_config_file += board["ConfigurationBinary"].asString();
            ufe_info_print("configuring board %i from binary file %s", board_id, board_config_file.c_str());
            dev.configFromBinary(board_id, board_config_file);
            dev.setDirectParams(board_id, 0x8280);
          } else if (board.isMember("ConfigurationJosn") && board["ConfigurationJosn"].asString() != "") {
            // Use json file to configure this board. This will set the readout/direct params as well.
            board_config_file += board["ConfigurationJosn"].asString();
            ufe_info_print("configuring board %i from json file %s", board_id, board_config_file.c_str());
            dev.configFromJson(board_id, board_config_file);
          } else {
            ufe_error_print("Unable to retrieve the configuration file for board %i", board_id);
            sm->changeState(fsm_state_t::Failure_s);
            return;
          }
//         cout << board_config_file << endl;

        }
      }

      // Hire one worker to scan the raw data.
      auto scanner_ptr = sm->addWorker<BMDDataScannerWorker>();

      // Hire one worker to record the data.
      auto recorder_ptr = sm->addWorker<BMDEventBuilderWorker>();
      rec_proc_ptr_ = dynamic_cast<BMDEventBuilder*>(recorder_ptr->getProcessor());
//       cerr << rec_proc_ptr_ << endl;

      // Give to the worker some instructions.
      rec_proc_ptr_->recorder().setDataPath( ctx->bm_config_doc_["DataPath"].asString() );
      rec_proc_ptr_->recorder().setRunNum( ctx->bm_config_doc_["RunNumber"].asInt() );
      rec_proc_ptr_->recorder().seMaxFileSize( ctx->bm_config_doc_["UsbParam"]["FileLimit"].asInt() );

      // Configure the FIFOs.
      auto raw_containers_fifo     = scanner_ptr->getInFifoPtr();
      auto scanned_containers_fifo = scanner_ptr->getOutFifoPtr();
      recycled_containers_fifo_    = recorder_ptr->getOutFifoPtr();

      raw_containers_fifo->setMaxSize(maxSize);
      scanned_containers_fifo->setMaxSize(maxSize);
      recycled_containers_fifo_->setMaxSize(maxSize);

      sm->addNode(recycled_containers_fifo_);
      sm->addNode(scanned_containers_fifo);
      sm->addNode(raw_containers_fifo);

      // Hire one readout worker for each device and connect the FIFOs.
      for (auto &dev: ctx->devs_) {
        auto w_ptr = sm->addWorker<BMDReadoutWorker>();
        BMDReadout* ro_proc_ptr = dynamic_cast<BMDReadout*>(w_ptr->getProcessor());
        ro_proc_ptr->setDevice(&dev);
        w_ptr->setInFifoPtr(recycled_containers_fifo_);
        w_ptr->setOutFifoPtr(raw_containers_fifo);
      }

      recorder_ptr->setInFifoPtr(scanned_containers_fifo);

      if (ctx->bm_config_doc_["VerboseLevel"].asInt() > 1) {
        // Enable time stats calculation of all workers.
        sm->enableStats(true);
      } else {
        // Disable time stats calculation of all workers.
        sm->enableStats(false);
      }
    }

    // Provide the workers with empty data containers.
    for (int xDc=0; xDc<maxSize; ++xDc) {
      recycled_containers_fifo_->push( new UFEDataContainer() );
    }

    // Initialize all workers.
    Standby::whenEnteringStateDo(sm, old_state);

  } catch (UFEError e) {
    cerr << e.getDescription() << endl;
    cerr << e.getLocation() << endl;
    if (e.getSeverity() == UFEError::SERIOUS) {
      sm->changeState(fsm_state_t::Failure_s);
      return;
    } else if (e.getSeverity() == UFEError::FATAL) {
      sm->changeState(fsm_state_t::FatalError_s);
      return;
    }
  }
}

void BMDStandby::whenSameStateDo(Fsm *sm) {
  // Print some usefull information about the last run.
  cout << "Last run: " << rec_proc_ptr_->recorder().getRunNum()-1 << endl;
  double run_duration = BMDActive::getDuration();
  if (run_duration < 100)
    cout << "duration: " << BMDActive::getDuration() << " seconds\n";
  else
    cout << "duration: " << BMDActive::getDuration()/60. << " minutes\n";

  cout << "data recorded: " << rec_proc_ptr_->recorder().getDataSize()/(1024.*1024) << " MB\n\n";
}

void BMDStandby::whenLeavingStateDo(Fsm *sm, fsm_state_t new_state) {
  if (new_state == fsm_state_t::Idle_s) { // This means that the system is turning Off.
    // When the system is Off the workers are dismissed.
    // New workers will be hired if the system enters Standby again.
    sm->dismissWorkers();
    sm->dismissNodes();
  }
}


//////////////////////////////////////////////////////////////////////////////////
/*                        BMDActive                                             */
//////////////////////////////////////////////////////////////////////////////////
double BMDActive::run_duration_ = 0;

void BMDActive::whenEnteringStateDo(Fsm *sm, fsm_state_t old_state) {
  // Get the time of the start of the run.
  t0_ = GET_TIME;

  // Send a DATA_READOUT command to all boards in order to enable starts the readout.
  try {
    UFEContext *ctx = UFEContext::instance();
    for (auto &dev: ctx->devs_) {
//       dev.setDirectParams();
      dev.start();
    }
  } catch (UFEError e) {
    cerr << e.getDescription() << endl;
    cerr << e.getLocation() << endl;
    if (e.getSeverity() == UFEError::SERIOUS) {
      sm->changeState(fsm_state_t::Failure_s);
      return;
    } else if (e.getSeverity() == UFEError::FATAL) {
      sm->changeState(fsm_state_t::FatalError_s);
      return;
    }
  }

  // Now start all workers.
  Active::whenEnteringStateDo(sm, old_state);
}

void BMDActive::whenLeavingStateDo(Fsm *sm, fsm_state_t new_state) {
  // Stop all workers.
  Active::whenLeavingStateDo(sm, new_state);

  UFEContext *ctx = UFEContext::instance();

  // Send a DATA_READOUT command to all boards in order to enable stop the readout.
  try {
    for (auto &dev: ctx->devs_) {
      dev.stop();
    }
  } catch (UFEError e) {
    cerr << e.getDescription() << endl;
    cerr << e.getLocation() << endl;
    if (e.getSeverity() == UFEError::SERIOUS) {
      sm->changeState(fsm_state_t::Failure_s);
      return;
    } else if (e.getSeverity() == UFEError::FATAL) {
      sm->changeState(fsm_state_t::FatalError_s);
      return;
    }
  }

  // Get the duration of the run.
  run_duration_ = GET_DURATION(t0_);

  if (ctx->bm_config_doc_["VerboseLevel"].asInt() > 1) {
    // Print some usefull information.
    for (auto &w: sm->getWorkers())
      w->printStats();
  }
}


//////////////////////////////////////////////////////////////////////////////////
/*                        BMDFatalError                                         */
//////////////////////////////////////////////////////////////////////////////////
void BMDFatalError::whenEnteringStateDo(Fsm *sm, fsm_state_t new_state) {
  UFEContext *ctx = UFEContext::instance();

  // Close all BM front-end boards.
  for (auto &d: ctx->devs_)
    if (d.isOpen())
      d.close();

  // Turn Off the Leds on all boards.
  ufe_in_session_on_all_boards_do(&led_off);

  // Free the device list and close ufec session.
  ufe_free_device_list(ctx->febs_, 1);
  ufe_exit(ctx->ufec_ctx_);

  // Update the Json file (configuration of the system).
  // This is mostly in order to store the number of the last run.
  // TODO : the Json file must be replaced by a database.
  ctx->updateBMConfig();
}
