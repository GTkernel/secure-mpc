#!/bin/bash

# Warning destructive operation
# Clone a special version of all emp repos that compile with PVC

read -p "Warning - destructive operation. Continue? [y/n]: " yn
case $yn in
    [Yy]* ) sudo rm -rf emp-m2pc/ emp-ot/ emp-readme/ emp-sh2pc/ emp-tool/ relic/ emp-pvc;;
    [Nn]* ) return;;
    * ) echo "Please answer yes or no." && return;;
esac

git clone https://github.com/emp-toolkit/emp-readme.git
bash ./emp-readme/scripts/install_packages.sh

#bash ./emp-readme/scripts/install_relic.sh
git clone https://github.com/relic-toolkit/relic.git
cd relic
git checkout 99288857
cmake -DALIGN=16 -DARCH=X64 -DARITH=curve2251-sse -DCHECK=off -DFB_POLYN=251 -DFB_METHD="INTEG;INTEG;QUICK;QUICK;QUICK;QUICK;LOWER;SLIDE;QUICK" -DFB_PRECO=on -DFB_SQRTF=off -DEB_METHD="PROJC;LODAH;COMBD;INTER" -DEC_METHD="CHAR2" -DCOMP="-O3 -funroll-loops -fomit-frame-pointer -march=native -msse4.2 -mpclmul" -DTIMER=CYCLE -DWITH="MD;DV;BN;FB;EB;EC" -DWSIZE=64 .
make
sudo make install
cd ..

git clone https://github.com/emp-toolkit/emp-tool.git
cd emp-tool
# see issue https://github.com/emp-toolkit/emp-pvc/issues/7
# and fix https://github.com/emp-toolkit/emp-pvc/commit/63954e9bc5281661a9b910b412650ad8d013aefc
git checkout 50c01ba99e5d257de05ef0e74ce6a0294a9ff471
cmake -DTHREADING=on .
#cmake .
make
sudo make install
cd ..

git clone https://github.com/emp-toolkit/emp-ot.git
cd emp-ot
# see fix https://github.com/emp-toolkit/emp-pvc/commit/63954e9bc5281661a9b910b412650ad8d013aefc
git checkout 15fb731
cmake .
make
sudo make install
cd ..

git clone https://github.com/emp-toolkit/emp-pvc.git
cd emp-pvc
git checkout 9e8b74d # peg to the version released with the paper
cmake .
make
sudo make install
cd ..

#git clone https://github.com/emp-toolkit/emp-ag2pc
#cd emp-ag2pc
#git checkout 163985c8a1637f64a1d1cfc5f75214d6d8ef06da
#cmake .
#make
#sudo make install
#cd ..
