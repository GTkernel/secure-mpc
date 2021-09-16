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
outputdir=results-agmpc
mkdir -p ${scriptpath}/../$outputdir

# Cleanup
sudo tc qdisc del dev lo root &>/dev/null && echo


# Multi-attribute auction
killall agmpc_multiatt_auction &>/dev/null && echo
${binpath}/agmpc_multiatt_circuit_generator 2 2 3
echo "Number of AND gates: $(grep -c "AND" agmpc_multiatt_auction_circuit.txt)"
echo "Number of XOR gates: $(grep -c "XOR" agmpc_multiatt_auction_circuit.txt)"
echo ""

$binpath/agmpc_multiatt_auction 3 12345 $ms > /dev/null &
$binpath/agmpc_multiatt_auction 2 12345 $ms > /dev/null &
sudo perf stat $binpath/agmpc_multiatt_auction 1 12345 $ms #>> $scriptpath/../$outputdir/sh2pc_latency_dc0.csv
echo ""
