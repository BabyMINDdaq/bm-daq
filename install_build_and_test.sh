#!/bin/bash

# This file is part of BabyMINDdaq software package. This software
# package is designed for internal use for the Baby MIND detector
# collaboration and is tailored for this use primarily.
# 
# BabyMINDdaq is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# BabyMINDdaq is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with BabyMINDdaq. If not, see <http://www.gnu.org/licenses/>.

export BMDAQ_INSTALL=OK
export BMDAQ=$PWD

source install_third_party.sh

if [ $BMDAQ_INSTALL = "ERROR" ]; then
  echo
  echo "Failed to install third party libraries!"
  echo
  exit
fi

cd $BMDAQ/../
BMDAQ_ROOT=$PWD
BMDAQ_PATH=$BMDAQ_ROOT/bm-daq/bin/:$BMDAQ_ROOT/bm-unpack/bin/:$BMDAQ_ROOT/libufec/bin/:$BMDAQ_ROOT/libufecpp/bin/:
cd -
echo "export PATH="$BMDAQ_PATH:'$'"PATH" >bm-daq-env.sh

./build_and_test.sh


if [ $BMDAQ_INSTALL = "OK" ]; then


  source $BMDAQ/bm-daq-env.sh

fi

