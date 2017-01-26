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

#ifndef UFE_DAQSM_H
#define UFE_DAQSM_H 1

// C++

// Mic11Api
#include "State.h"
#include "Fifo.h"

#include "UFEDevice.h"
#include "BMDWorkers.h"

struct BMDIdle : public Idle {
  void whenEnteringStateDo(Fsm *sm, fsm_state_t old_state) final;
  void whenSameStateDo(Fsm *sm) final;
  void whenLeavingStateDo(Fsm *sm, fsm_state_t new_state) final;

private:
  bool setDeviceDescription(UFEDevice *dev, Json::Value bm_config_doc_);
};


struct BMDFatalError : public FatalError {
  void whenEnteringStateDo(Fsm *sm, fsm_state_t new_state) final;
};


struct BMDStandby : public Standby {
  void whenEnteringStateDo(Fsm *sm, fsm_state_t old_state) final;
  void whenSameStateDo(Fsm *sm) final;
  void whenLeavingStateDo(Fsm *sm, fsm_state_t new_state) final;

private:
  static std::shared_ptr< PtrFifo<UFEDataContainer*> > recycled_containers_fifo_;
  static BMDEventBuilder *rec_proc_ptr_;
};


struct BMDActive : public Active {
  void whenEnteringStateDo(Fsm *sm, fsm_state_t old_state) final;
  void whenSameStateDo(Fsm *sm) final {}
  void whenLeavingStateDo(Fsm *sm, fsm_state_t new_state) final;

  static double getDuration() {return run_duration_;}

private:
  hd_time t0_;
  static double run_duration_;
};


#define stateMap(STATE) \
  STATE( USER_IDLE(BMDIdle) ) \
  STATE( DEFAULT_FAILURE ) \
  STATE( USER_STANDBY(BMDStandby) ) \
  STATE( USER_ACTIVE(BMDActive) ) \
  STATE( USER_FATALERROR(BMDFatalError) )

IMPLEMENT_STATE_FACTORY(stateMap, UFEStateFactory)

#endif

