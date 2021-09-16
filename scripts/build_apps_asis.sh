#!/bin/bash
scriptpath="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
set -e


mkdir -p $scriptpath/../build/
cd $scriptpath/../build/
cmake ..
make -j8
cd -

