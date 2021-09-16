#!/bin/bash
if [ "${BASH_SOURCE[0]}" -ef "$0" ]; then
    # good - we're executing
    echo -ne ""
else
    echo "execute this script, do not source it!"
    return
fi

# Setup
scriptpath="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
binpath="${scriptpath}/../build/bin/"
set -e
outputdir=results-plain
mkdir -p ${scriptpath}/../$outputdir


### Pregenerate some circuits and binaries
for numParties in $(seq 2 10); do
#for numParties in 3; do
    # recompile code
    sed -i "s/const static int nP = [0-9]\+;/const static int nP = ${numParties};/" $scriptpath/../src/plain_singleatt_auction.cc
    $scriptpath/../scripts/build_apps_asis.sh

    # rename output binary
    mv ${binpath}/plain_singleatt_auction ${binpath}/plain_singleatt_$((numParties))_auction
done
