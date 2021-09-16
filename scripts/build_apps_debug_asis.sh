#!/bin/bash
scriptpath="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
set -e

# edit flags in cmake/common.cmake

mkdir -p $scriptpath/../build/
cd $scriptpath/../build/
cmake -DCMAKE_BUILD_TYPE=Debug ..
#cmake -DCMAKE_BUILD_TYPE=Debug -DUSE_RANDOM_DEVICE=ON ..
#cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
make -j8
cd -

