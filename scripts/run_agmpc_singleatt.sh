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
killall agmpc_multiatt_auction &>/dev/null && echo



# Generate bins
#${scriptpath}/generate_agmpc_singleatt_bins.sh



### Make ipaddrs file for localhost testing
cat <<EOT > ${binpath}/ipAddrs.txt
127.0.0.1:3000
127.0.0.1:3100
127.0.0.1:3200
127.0.0.1:3300
127.0.0.1:3400
127.0.0.1:3500
127.0.0.1:3600
127.0.0.1:3700
127.0.0.1:3800
127.0.0.1:3900
127.0.0.1:4000
127.0.0.1:4100
127.0.0.1:4200
127.0.0.1:4300
127.0.0.1:4400
127.0.0.1:4500
127.0.0.1:4600
127.0.0.1:4700
127.0.0.1:4800
127.0.0.1:4900
EOT




### Simple Test
#for party in 2 3; do
#    echo "starting $party"
#    $binpath/agmpc_singleatt_3_auction $binpath/ipAddrs.txt $binpath/agmpc_output.txt ${party} $((${party} + 2)) > /dev/null &
#done
#$binpath/agmpc_singleatt_3_auction $binpath/ipAddrs.txt $binpath/agmpc_output.txt 1


sudo tc qdisc del dev lo root || echo no delay found
killall agmpc_singleatt* || echo all agmpc processes exited successfully


### Single Attribute Auction - Results
killall agmpc_singleatt* &>/dev/null && echo
rm ${scriptpath}/../$outputdir/output.log && echo -e "cleaned results dir\n\n\n"

for numParties in $(seq 2 9); do
    # set the p2p latency
    #for ms in 0 4 8 12 16 20; do
        delay=$((ms / 2))
        sudo tc qdisc add dev lo root netem delay ${delay}ms || echo skipping 0 ms latency

        # run all parties
        for party in $(seq 2 $numParties); do
            echo "starting $party"
            $binpath/agmpc_singleatt_$((numParties))_auction $binpath/ipAddrs.txt $binpath/agmpc_output.txt ${party} $((${party} + 2)) > /dev/null &
        done
        echo "Writing run to result directory"
        $binpath/agmpc_singleatt_$((numParties))_auction $binpath/ipAddrs.txt $binpath/agmpc_output.txt 1 >> $scriptpath/../$outputdir/output.log
        sudo tc qdisc del dev lo root netem delay ${delay}ms
    #done

done

echo -e "\n\n\nplotting numParties vs flushes"
# global flushes
python3 $scriptpath/plotter.py \
    --csvlog $scriptpath"/../$outputdir/${ms}output.log" \
    --graphpath $scriptpath"/../$outputdir/global-total-flushes.pdf" \
    --only-tags "global-total-flushes" \
    --custom-legend-labels "AGMPC" \
    --title "Global Total Flushes" \
    --xlabel "Number of Parties" \
    --ylabel "Flushes" \
    --color-theme "dracula" \
    --projection 0

# flushes by protocol phase
python3 $scriptpath/plotter.py \
    --csvlog $scriptpath"/../$outputdir/${ms}output.log" \
    --graphpath $scriptpath"/../$outputdir/global-total-flushes.pdf" \
    --only-tags "global-total-flushes" \
    --custom-legend-labels "AGMPC" \
    --title "Global Total Flushes" \
    --xlabel "Number of Parties" \
    --ylabel "Flushes" \
    --color-theme "dracula" \
    --projection 0

# setup flushes
#python3 $scriptpath/plotter.py \
#    --csvlog $scriptpath"/../$outputdir/${ms}output.log" \
#    --graphpath $scriptpath"/../$outputdir/setup-io0-flushes.pdf" \
#    --only-tags \
#        "setup-io0-flushes-party2" "setup-io0-flushes-party3" \
#        "setup-io0-flushes-party4" "setup-io0-flushes-party5" \
#        "setup-io0-flushes-party6" "setup-io0-flushes-party7" \
#        "setup-io0-flushes-party8" "setup-io0-flushes-party9" \
#    --custom-legend-labels \
#        "Party 2" "Party 3" \
#        "Party 4" "Party 5" \
#        "Party 6" "Party 7" \
#        "Party 8" "Party 9" \
#    --title "Flushes in Setup Phase" \
#    --xlabel "Number of Parties" \
#    --ylabel "Flushes" \
#    --color-theme "dracula" \
#    --projection 0 \
#    --show


echo -e "\n\n\nplotting runtime"
python3 $scriptpath/plotter.py \
    --csvlog $scriptpath"/../$outputdir/${ms}output.log" \
    --graphpath $scriptpath"/../$outputdir/runtime.pdf" \
    --only-tags "e2e-mpc" \
    --custom-legend-labels "AGMPC" \
    --title "Runtime" \
    --xlabel "Number of Parties" \
    --ylabel "Runtime (us)" \
    --color-theme "dracula" \
    --projection 0
