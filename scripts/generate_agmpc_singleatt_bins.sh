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

### Pregenerate all circuits and binaries
for numParties in $(seq 2 10); do
#for numParties in 9; do
    # first build circuit
    time ${scriptpath}/../build/bin/agmpc_singleatt_circuit_generator $((numParties-1))
    
    circuitPath="${binpath}/agmpc_singleatt_${numParties}_circuit.txt"
    echo "Number of AND gates: $(grep -c "AND" $circuitPath)"
    echo "Number of XOR gates: $(grep -c "XOR" $circuitPath)"

    # recompile code
    sed -i "s/const static int nP = [0-9]\+;/const static int nP = ${numParties};/" $scriptpath/../src/agmpc_singleatt_auction.cc
    $scriptpath/../scripts/build_apps_asis.sh
    #$scriptpath/../scripts/build_apps_debug_asis.sh

    # rename output binary
    mv ${binpath}/agmpc_singleatt_auction ${binpath}/agmpc_singleatt_$((numParties))_auction
    sudo setcap 'CAP_NET_ADMIN+eip' ${binpath}/agmpc_singleatt_$((numParties))_auction
done

