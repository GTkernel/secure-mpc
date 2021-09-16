#!/bin/bash
scriptpath="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
set -e

# Edit flags in emp-tool/cmake/common.cmake for libs

# emp-tool has to be first to put common.cmake in /usr/local
dirz="${scriptpath}/../emp-tool/
${scriptpath}/../emp-ag2pc/
${scriptpath}/../emp-agmpc/
${scriptpath}/../emp-ot/
${scriptpath}/../emp-sh2pc/"

echo "$dirz" | while read libdir; do
    cd $libdir
    echo "Running for dir $libdir "
    cmake -DCMAKE_BUILD_TYPE=Debug .
    #cmake -DCMAKE_BUILD_TYPE=Debug -DUSE_RANDOM_DEVICE=ON
    #cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo
    make -j8
    sudo make install
    cd -
done


