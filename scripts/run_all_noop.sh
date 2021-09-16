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
set -e
outputdir=results-noop
mkdir -p ${scriptpath}/../$outputdir

# Cleanup
#rm ${scriptpath}/../$outputdir/* && echo "cleaned results dir"
killall sh2pc_noop_benchmark &>/dev/null && echo
killall sh2pc_noop_circuit_benchmark &>/dev/null && echo
sudo tc qdisc del dev lo root &>/dev/null && echo


# Count number of gates in the circuit
${scriptpath}/../build/bin/sh2pc_noop_circuit_generator
echo "Number of AND gates: $(grep -c "AND" noop_circuit.txt)"
echo "Number of XOR gates: $(grep -c "XOR" noop_circuit.txt)"
echo ""


### Latency tests
#echo "latency test - running"
#for ms in 0 2 4 6 8 10 30; do # even numbers only
#    echo "    testing ${ms}ms"
#    delay=$((ms / 2))
#    sudo tc qdisc add dev lo root netem delay ${delay}ms
#
#    $scriptpath/../build/bin/sh2pc_noop_benchmark 1 $ms 0 > /dev/null &
#    $scriptpath/../build/bin/sh2pc_noop_benchmark 0 $ms 0 >> $scriptpath/../$outputdir/sh2pc_latency_dc0.csv
#
#    sudo tc qdisc del dev lo root netem delay ${delay}ms
#done
#
#
#echo "latency test - plotting latency vs throughput"
#python3 $scriptpath/plotter.py --prefix "sh2pc_latency" \
#    --csvlog $scriptpath"/../$outputdir/sh2pc_latency_dc0.csv" \
#    --graphpath $scriptpath"/../$outputdir/sh2pc_latency_noop_ot.pdf" \
#    --only-tags "otps_DC0" \
#    --xlabel "ms between Alice and Bob" \
#    --ylabel "32 bit int OTs per second" \
#    --custom-legend-labels "ot"
#
#python3 $scriptpath/plotter.py --prefix "sh2pc_latency" \
#    --csvlog $scriptpath"/../$outputdir/sh2pc_latency_dc0.csv" \
#    --graphpath $scriptpath"/../$outputdir/sh2pc_latency_noop_total.pdf" \
#    --only-tags "comparisonps_DC0" \
#    --xlabel "ms between Alice and Bob" \
#    --ylabel "32 bit int comparisons per second" \
#    --custom-legend-labels "auction"
#
#python3 $scriptpath/plotter.py --prefix "sh2pc_latency" \
#    --csvlog $scriptpath"/../$outputdir/sh2pc_latency_dc0.csv" \
#    --graphpath $scriptpath"/../$outputdir/sh2pc_latency_noop_persec.pdf" \
#    --only-tags "totalps_DC0" \
#    --xlabel "ms between Alice and Bob" \
#    --ylabel "32 bit int millionaires per second" \
#    --custom-legend-labels "total"
#
#
#echo "latency test - plotting latency vs num messages"
#python3 $scriptpath/plotter.py --prefix "sh2pc_latency" \
#    --csvlog $scriptpath"/../$outputdir/sh2pc_latency_dc0.csv" \
#    --graphpath $scriptpath"/../$outputdir/sh2pc_latency_noop_ot_num_messages.pdf" \
#    --only-tags "ot_num_messages_send_DC0" "ot_num_messages_recv_DC0" \
#    --xlabel "ms between Alice and Bob" \
#    --ylabel "number of messages for OT" \
#    --custom-legend-labels "sent" "recvd"

# Could plot more about auction_num_messages<sent/recv>

# Could also measure bytes sent and recv'd

# Could plot bytes/message





### Latency tests with circuit file
# WARNING - these results are very misleading because
# there is no OT happening - just circuit execution.
echo "circuit file latency test - running"
echo "" > $scriptpath/../$outputdir/sh2pc_latencycf_dc0.csv
for ms in 0 2 4 6 8 10 30; do # even numbers only
    echo "    testing ${ms}ms"
    delay=$((ms / 2))
    sudo tc qdisc add dev lo root netem delay ${delay}ms

    $scriptpath/../build/bin/sh2pc_noop_circuit_benchmark 1 $ms 0 > /dev/null &
    $scriptpath/../build/bin/sh2pc_noop_circuit_benchmark 0 $ms 0 >> $scriptpath/../$outputdir/sh2pc_latencycf_dc0.csv

    sudo tc qdisc del dev lo root netem delay ${delay}ms
done

python3 $scriptpath/plotter.py --prefix "sh2pc_latency_circuit" \
    --csvlog $scriptpath"/../$outputdir/sh2pc_latencycf_dc0.csv" \
    --graphpath $scriptpath"/../$outputdir/sh2pc_latency_noop.pdf" \
    --only-tags "circuitps_DC0" \
    --xlabel "THIS IS MISLEADING - ms between Alice and Bob" \
    --ylabel "32 bit int millionaires per second" \
    --custom-legend-labels "total_circuit"
