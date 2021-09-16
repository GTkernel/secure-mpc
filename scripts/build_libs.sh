#!/bin/bash
scriptpath="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
set -e

# emp-tool has to be first to put common.cmake in /usr/local
dirz="${scriptpath}/../emp-ot/
${scriptpath}/../emp-tool/
${scriptpath}/../emp-ag2pc/
${scriptpath}/../emp-agmpc/
${scriptpath}/../emp-sh2pc/"

echo "$dirz" | while read libdir; do
    cd $libdir
    echo "Running for dir $libdir "
    cmake .
    make -j8
    sudo make install
    cd -
done


