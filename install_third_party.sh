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

BMDAQ=$PWD

BMDAQ_INSTALL=OK

cd ..

# git clone git@github.com:BabyMINDdaq/libufec.git
# 
# git clone git@github.com:BabyMINDdaq/libufecpp.git
# 
# git clone git@github.com:yordan-karadzhov/mic11api.git
# 
# git clone git@github.com:BabyMINDdaq/bm-unpack.git

wget https://github.com/BabyMINDdaq/libufec/archive/master.zip
unzip master.zip
rm master.zip

wget https://github.com/BabyMINDdaq/libufecpp/archive/master.zip
unzip master.zip
rm master.zip

wget https://github.com/yordan-karadzhov/mic11api/archive/master.zip
unzip master.zip
rm master.zip

wget https://github.com/BabyMINDdaq/bm-unpack/archive/master.zip
unzip master.zip
rm master.zip


mv libufec-master libufec
cd libufec
source build_and_test.sh
if [ $BMDAQ_INSTALL = "ERROR" ]; then
  exit
fi

cd $BMDAQ/../
mv libufecpp-master libufecpp
cd libufecpp
source build_and_test.sh
if [ $BMDAQ_INSTALL = "ERROR" ]; then
  exit
fi

cd $BMDAQ/../
mv mic11api-master mic11api
cd mic11api
source build_and_test.sh
if [ $MIC11_INSTALL = "ERROR" ]; then
  BMDAQ_INSTALL=ERROR
  exit
fi

cd $BMDAQ/../
mv bm-unpack-master bm-unpack
cd bm-unpack/build/
cmake ..
make
if [ $BMDAQ_INSTALL = "ERROR" ]; then
  exit
fi

cd $BMDAQ