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

# Cleanup
sudo tc qdisc del dev lo root &>/dev/null && echo
killall plain_singleatt* &>/dev/null && echo


$scriptpath/generate_plain_singleatt_bins.sh



### Make ipaddrs file for localhost testing
cat <<EOT > ${binpath}/ipaddrs.txt
127.0.0.1:10000
127.0.0.1:11000
127.0.0.1:12000
127.0.0.1:13000
127.0.0.1:14000
127.0.0.1:15000
127.0.0.1:16000
127.0.0.1:17000
127.0.0.1:18000
127.0.0.1:19000
127.0.0.1:20000
127.0.0.1:21000
127.0.0.1:22000
127.0.0.1:23000
127.0.0.1:24000
127.0.0.1:25000
127.0.0.1:26000
127.0.0.1:27000
127.0.0.1:28000
127.0.0.1:29000
EOT



### Simple Test
for party in 2 3; do
    echo "starting $party"
    $binpath/plain_singleatt_3_auction ${party} $((${party} + 2)) > /dev/null &
done
$scriptpath/../build/bin/plain_singleatt_3_auction 1



### Sinlge Attribute Auction - Results
killall plain_singleatt* &>/dev/null && echo
rm ${scriptpath}/../$outputdir/* && echo -e "cleaned results dir\n\n\n"

for numParties in 3 5 10 15 20; do
    # set the p2p latency
    for ms in 0 2 4 6 8 10; do
        delay=$((ms / 2))
        sudo tc qdisc add dev lo root netem delay ${delay}ms

        # run all parties
        for party in $(seq 2 $numParties); do
            echo "starting $party"
            $binpath/plain_singleatt_$((numParties))_auction ${party} 12345 > /dev/null &
        done
        $binpath/plain_singleatt_$((numParties))_auction 1 12345 >> $scriptpath/../$outputdir/${ms}mslatency.csv
        sudo tc qdisc del dev lo root netem delay ${delay}ms
    done

done

# plot
for ms in 0 2 4 6 8 10; do
    echo "plotting numParties vs runtime"
    python3 $scriptpath/plotter.py --prefix "${ms}ms p2p" \
        --csvlog $scriptpath"/../$outputdir/${ms}mslatency.csv" \
        --graphpath $scriptpath"/../$outputdir/plain_${ms}mslatency.pdf" \
        --only-tags "end-to-end" \
        --xlabel "Number of Parties" \
        --ylabel "runtime (s)"
done

