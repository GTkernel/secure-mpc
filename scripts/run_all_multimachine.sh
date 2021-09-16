#!/bin/bash
if [ "${BASH_SOURCE[0]}" -ef "$0" ]; then
    # good - we're executing
    echo -ne ""
else
    echo "execute this script, do not source it!"
    return
fi

scriptpath="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
set -e

if [ "$1" = "ALICE" ]; then
    echo "Running Alice"
elif [ "$1" = "BOB" ]; then
    echo "Running Bob"
else
    echo "Usage: $0 <ALICE|BOB>"
    exit 1
fi

### Scalability tests
# Kill old running tests
killall sh2pc_auction_benchmark &>/dev/null && echo

echo "scalability test - running"
if [ "$1" = "ALICE" ]; then
    $scriptpath/../build/bin/sh2pc_auction_benchmark 0 10 > $scriptpath/../results/sh2pc_scalability.csv

    echo "scalability test - plotting"
    python3 $scriptpath/plotter.py --prefix "sh2pc_scalability" \
        --csvlog $scriptpath"/../results/sh2pc_scalability.csv" \
        --graphpath $scriptpath"/../results/sh2pc_scalability_multimachine.pdf" \
        --only-tags ot auction \
        --xlabel "Auction Size (number of attributes)"
else
    $HOME/dex-mpc/build/bin/sh2pc_auction_benchmark 1 10
fi
echo -e "scalability test - done\n\n"



### Latency tests

if [ "$1" = "ALICE" ]; then
    # remove any old latency
    sudo tc qdisc del dev wlp0s20f3 root &>/dev/null && 1

    # wipe file
    echo "" > $scriptpath/../results/sh2pc_latency.csv

    echo "latency test - running"
    for i in {1..1000..100}; do
        #ms=$((i / 2)) if running on same machine, latency is added both on send and rx
        ms=$i # if running on different machines, only inject latency on one side
        echo "    testing ${i}ms"
        sudo tc qdisc add dev wlp0s20f3 root netem delay ${ms}ms
        sleep 1 # wait for latency to stick?
        $scriptpath/../build/bin/sh2pc_auction_benchmark 0 1 >> $scriptpath/../results/sh2pc_latency.csv
        sudo tc qdisc del dev wlp0s20f3 root netem delay ${ms}ms
    done

    echo "latency test - plotting"
    python3 $scriptpath/plotter.py --prefix "sh2pc_latency" \
        --csvlog $scriptpath"/../results/sh2pc_latency.csv" \
        --graphpath $scriptpath"/../results/sh2pc_latency_100ms_multimachine.pdf" \
        --only-tags ot auction \
        --xlabel "Latency (100ms)"
else

    echo "latency test - running"
    for i in {1..1000..100}; do
        $scriptpath/../build/bin/sh2pc_auction_benchmark 1 1
    done
fi
echo "latency test - done"


### Number of messages tests
if [ "$1" = "ALICE" ]; then
    echo "number of messages test - running"
    $scriptpath/../build/bin/sh2pc_auction_benchmark 0 10 > $scriptpath/../results/sh2pc_nom.csv

    echo "number of messages test - plotting"
    python3 $scriptpath/plotter.py --prefix "sh2pc_nom" \
        --csvlog $scriptpath"/../results/sh2pc_nom.csv" \
        --graphpath $scriptpath"/../results/sh2pc_nom_multimachine.pdf" \
        --only-tags ot_num_messages_send ot_num_messages_recv auction_num_messages_send auction_num_messages_recv \
        --xlabel "Auction Size (number of attributes)"
else
    $scriptpath/../build/bin/sh2pc_auction_benchmark 1 10
fi
