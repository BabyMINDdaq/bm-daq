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
 *  \file    BMDDataHeaders.h
 *  \brief   File containing declarations of the Baby MIND DAQ event-building
 *  headers.
 *  \author  Yordan Karadzhov
 *  \date    Dec 2016
 */
#ifndef BMD_DATA_HEADERS_H
#define BMD_DATA_HEADERS_H 1

// C++
#include <ctime>

/** \struct BMDDataFragmenHeader
 *  Header for the Fragmen Memoru bank.
 */
struct BMDDataFragmenHeader {
  /** The size of the data. */
  size_t size_;     // Make sure that the size_ is declared before any other data member.
};

/** \struct BMDEventHeader
 *  Header for the Event Memoru bank.
 */
struct BMDEventHeader {
  /** The size of the data. */
  size_t size_;     // Make sure that the size_ is declared before any other data member.

  /** Time stamp of the event. */
  std::time_t  time_;

  /** Id number of the event. */
  uint32_t     event_id_;

  /** Type of the event. */
  uint8_t      type_;

  /** If Super even, the number of subevents. */
  uint8_t      n_subevents_;

  /** If normal even, the number of fragments. */
  uint16_t     n_fragments_;

  /** \brief Returns True, if this is a Super event. */
  bool isSuperEvent() {
    return static_cast<bool>(this->type_ & 0x80);
  }

  /** \brief Returns the type of the event. */
  unsigned int  eventType() {
    return static_cast<int>(this->type_ & 0x7f);
  }
};

#endif


