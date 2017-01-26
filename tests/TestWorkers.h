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

#ifndef WORKERS_TEST_HPP_
#define WORKERS_TEST_HPP_

// CppUnit
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

// bm-daq
#include "BMDWorkers.h"

class TestWorkers : public CppUnit::TestFixture {
 public:
  void setUp();
  void tearDown();

 protected:
  void TestEventBuilderInitClose();
  void TestEBProcessCase1();
  void TestEBProcessCase2();

 private:


  CPPUNIT_TEST_SUITE( TestWorkers );
  CPPUNIT_TEST( TestEventBuilderInitClose );
  CPPUNIT_TEST( TestEBProcessCase1 );
  CPPUNIT_TEST( TestEBProcessCase2 );
//   CPPUNIT_TEST(  );
//   CPPUNIT_TEST(  );
//   CPPUNIT_TEST(  );
  CPPUNIT_TEST_SUITE_END();
};

#endif
