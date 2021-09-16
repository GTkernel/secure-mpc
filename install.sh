#!/bin/bash
scriptpath="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd $scriptpath

rm ${scriptpath}/emp-ag2pc/CMakeCache.txt \
    ${scriptpath}/emp-agmpc/CMakeCache.txt \
    ${scriptpath}/emp-ot/CMakeCache.txt \
    ${scriptpath}/emp-readme/CMakeCache.txt \
    ${scriptpath}/emp-sh2pc/CMakeCache.txt \
    ${scriptpath}/emp-tool/CMakeCache.txt \
    ${scriptpath}/build/CMakeCache.txt

wget --no-check-certificate https://raw.githubusercontent.com/emp-toolkit/emp-readme/master/scripts/install.py -O install.py
python3 install.py -install -tool -ot -sh2pc -agmpc -ag2pc
rm install.py

# Apply patch to emp-tool repo for instrumentation
cd ${scriptpath}/emp-tool
git apply ../emp-tool_bandwidth_instrumentation.patch
make
sudo make install

# to create patch:
# cd emp-tool/
# git diff > ../emp-tool_bandwidth_instrumentation.patch


