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
 *  \file    BMDStateMachine.h
 *  \brief   File containing declarations of the Baby MIND DAQ Finite-state machine.
 *  \author  Yordan Karadzhov
 *  \date    Nov 2016
 */

#ifndef UFE_DAQSM_H
#define UFE_DAQSM_H 1

// Mic11Api
#include "State.h"
#include "Fifo.h"

// libufecpp
#include "UFEDevice.h"

// bm-daq
#include "BMDWorkers.h"


/** @struct BMDIdle
 *  Idle stat of the Baby MIND DAQ Finite-state machine.
 */
struct BMDIdle : public Idle {

  /** \brief User-defined action, to be executed when the state machine
   * enters this state.
   */
  void whenEnteringStateDo(Fsm *sm, fsm_state_t old_state) final;

  /** \brief User-defined action, to be executed when the state machine
   *  stays in this state.
   */
  void whenSameStateDo(Fsm *sm) final;

  /** \brief User-defined action, to be executed when the state machine
   * leaves this state.
   */
  void whenLeavingStateDo(Fsm *sm, fsm_state_t new_state) final;

private:
  /** \brief Searches for the description of the device in a Json file
   *  containing the configuration of the system.
   */
  bool setDeviceDescription(UFEDevice *dev, Json::Value bm_config_doc_);
};



/** @struct BMDStandby
 *  Standby stat of the Baby MIND DAQ Finite-state machine.
 */
struct BMDStandby : public Standby {

  /** \brief User-defined action, to be executed when the state machine
   * enters this state.
   */
  void whenEnteringStateDo(Fsm *sm, fsm_state_t old_state) final;

  /** \brief User-defined action, to be executed when the state machine
   *  stays in this state.
   */
  void whenSameStateDo(Fsm *sm) final;

  /** \brief User-defined action, to be executed when the state machine
   * leaves this state.
   */
  void whenLeavingStateDo(Fsm *sm, fsm_state_t new_state) final;

private:
  static double data_size_;

  static std::shared_ptr< PtrFifo<UFEDataContainer*> > recycled_containers_fifo_;
  static BMDEventBuilder *rec_proc_ptr_;
};



/** @struct BMDActive
 *  Active stat of the Baby MIND DAQ Finite-state machine.
 */
struct BMDActive : public Active {

  /** \brief User-defined action, to be executed when the state machine
   * enters this state.
   */
  void whenEnteringStateDo(Fsm *sm, fsm_state_t old_state) final;

  /** \brief User-defined action, to be executed when the state machine
   *  stays in this state.
   */
  void whenSameStateDo(Fsm *sm) final {}

  /** \brief User-defined action, to be executed when the state machine
   * leaves this state.
   */
  void whenLeavingStateDo(Fsm *sm, fsm_state_t new_state) final;

  /** \brief Returns the duration of the run in seconds. */
  static double getDuration() {return run_duration_;}

private:
  hd_time t0_;
  static double run_duration_;
};



/** @struct BMDFailure
 *  Failure stat of the Baby MIND DAQ Finite-state machine.
 */
struct BMDFailure : public Failure {

  /** \brief User-defined action, to be executed when the state machine
   * enters this state.
   */
  void whenLeavingStateDo(Fsm *sm, fsm_state_t new_state) final;
};



/** @struct BMDFatalError
 *  Fatal Error stat of the Baby MIND DAQ Finite-state machine.
 */
struct BMDFatalError : public FatalError {

  /** \brief User-defined action, to be executed when the state machine
   * leaves this state.
   */
  void whenEnteringStateDo(Fsm *sm, fsm_state_t new_state) final;
};


/** Implement user-defined State list. */
#define stateMap(STATE) \
  STATE( USER_IDLE(BMDIdle) ) \
  STATE( USER_FAILURE( BMDFailure ) ) \
  STATE( USER_STANDBY(BMDStandby) ) \
  STATE( USER_ACTIVE(BMDActive) ) \
  STATE( USER_FATALERROR(BMDFatalError) )

/** Implement user-defined State Factory. */
IMPLEMENT_STATE_FACTORY(stateMap, UFEStateFactory)

#endif

