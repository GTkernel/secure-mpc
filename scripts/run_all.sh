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
outputdir=results
mkdir -p ${scriptpath}/../$outputdir
rm ${scriptpath}/../$outputdir/*

# Cleanup
killall sh2pc_auction_benchmark &>/dev/null && echo
sudo tc qdisc del dev lo root &>/dev/null && echo


### Scalability tests
echo "scalability test - running"
for delay in 1 10 100; do
    ms=$((delay / 2))
    echo "    testing ${delay}ms"
    sudo tc qdisc add dev lo root netem delay ${ms}ms
    for size in 1 10; do
        echo "    auction size ${size}"
        $scriptpath/../build/bin/sh2pc_auction_benchmark 1 $size 0 > /dev/null &
        $scriptpath/../build/bin/sh2pc_auction_benchmark 0 $size 0 >> $scriptpath/../$outputdir/sh2pc_scalability_${delay}ms_dc0.csv

        $scriptpath/../build/bin/sh2pc_auction_benchmark 1 $size 1 > /dev/null &
        $scriptpath/../build/bin/sh2pc_auction_benchmark 0 $size 1 >> $scriptpath/../$outputdir/sh2pc_scalability_${delay}ms_dc1.csv
    done
    sudo tc qdisc del dev lo root netem delay ${ms}ms
done

echo "scalability test - plotting"
python3 $scriptpath/plotter.py --prefix "sh2pc_scalability_1ms_OT" \
    --csvlog $scriptpath"/../$outputdir/sh2pc_scalability_1ms_dc0.csv" \
        $scriptpath"/../$outputdir/sh2pc_scalability_1ms_dc1.csv" \
    --graphpath $scriptpath"/../$outputdir/sh2pc_scalability_1ms_OT.pdf" \
    --only-tags ot_DC0 ot_DC1 \
    --xlabel "Auction Size" \
    --ylabel "Seconds" \
    --custom-legend-labels "dc0" "dc1"

python3 $scriptpath/plotter.py --prefix "sh2pc_scalability_1ms_Auction" \
    --csvlog $scriptpath"/../$outputdir/sh2pc_scalability_1ms_dc0.csv" \
        $scriptpath"/../$outputdir/sh2pc_scalability_1ms_dc1.csv" \
    --graphpath $scriptpath"/../$outputdir/sh2pc_scalability_1ms_Auction.pdf" \
    --only-tags auction_DC0 auction_DC1 \
    --xlabel "Auction Size" \
    --ylabel "Seconds" \
    --custom-legend-labels "dc0" "dc1"

python3 $scriptpath/plotter.py --prefix "sh2pc_scalability_10ms_OT" \
    --csvlog $scriptpath"/../$outputdir/sh2pc_scalability_10ms_dc0.csv" \
        $scriptpath"/../$outputdir/sh2pc_scalability_10ms_dc1.csv" \
    --graphpath $scriptpath"/../$outputdir/sh2pc_scalability_10ms_OT.pdf" \
    --only-tags ot_DC0 ot_DC1 \
    --xlabel "Auction Size" \
    --ylabel "Seconds" \
    --custom-legend-labels "dc0" "dc1"

python3 $scriptpath/plotter.py --prefix "sh2pc_scalability_10ms_Auction" \
    --csvlog $scriptpath"/../$outputdir/sh2pc_scalability_10ms_dc0.csv" \
        $scriptpath"/../$outputdir/sh2pc_scalability_10ms_dc1.csv" \
    --graphpath $scriptpath"/../$outputdir/sh2pc_scalability_10ms_Auction.pdf" \
    --only-tags auction_DC0 auction_DC1 \
    --xlabel "Auction Size" \
    --ylabel "Seconds" \
    --custom-legend-labels "dc0" "dc1"

python3 $scriptpath/plotter.py --prefix "sh2pc_scalability_100ms_OT" \
    --csvlog $scriptpath"/../$outputdir/sh2pc_scalability_100ms_dc0.csv" \
        $scriptpath"/../$outputdir/sh2pc_scalability_100ms_dc1.csv" \
    --graphpath $scriptpath"/../$outputdir/sh2pc_scalability_100ms_OT.pdf" \
    --only-tags ot_DC0 ot_DC1 \
    --xlabel "Auction Size" \
    --ylabel "Seconds" \
    --custom-legend-labels "dc0" "dc1"

python3 $scriptpath/plotter.py --prefix "sh2pc_scalability_100ms_Auction" \
    --csvlog $scriptpath"/../$outputdir/sh2pc_scalability_100ms_dc0.csv" \
        $scriptpath"/../$outputdir/sh2pc_scalability_100ms_dc1.csv" \
    --graphpath $scriptpath"/../$outputdir/sh2pc_scalability_100ms_Auction.pdf" \
    --only-tags auction_DC0 auction_DC1 \
    --xlabel "Auction Size" \
    --ylabel "Seconds" \
    --custom-legend-labels "dc0" "dc1"




### Number of messages tests
# a simple, synchronous, packet counter - we need async
#sudo tcpdump -i lo -nn port 55555

echo "number of messages test - setup"

## new chain and new rule
sudo iptables -N dexin
sudo iptables -N dexout
sudo iptables -A dexin
sudo iptables -A dexout

## point dex traffic to new rule
sudo iptables -A INPUT -i lo -p tcp --dport 55555 -j dexin
sudo iptables -A OUTPUT -p tcp --dport 55555 -j dexout

echo "number of messages test - running"
for size in 1 10 100 1000; do
    sudo iptables -Z dexin
    sudo iptables -Z dexout

    $scriptpath/../build/bin/sh2pc_auction_benchmark 1 $size 0 > /dev/null &
    $scriptpath/../build/bin/sh2pc_auction_benchmark 0 $size 0 >> $scriptpath/../$outputdir/sh2pc_nom_dc0.csv
    echo "SeNtInAl,counter,bash,packets-in_DC0,$size,$(sudo iptables -nvxL dexin | awk '/all/{print $1}')" >> $scriptpath/../$outputdir/sh2pc_packets-in_dc0.csv
    echo "SeNtInAl,counter,bash,bytes-in_DC0,$size,$(sudo iptables -nvxL dexin | awk '/all/{print $2}')" >> $scriptpath/../$outputdir/sh2pc_bytes-in_dc0.csv
    echo "SeNtInAl,counter,bash,packets-out_DC0,$size,$(sudo iptables -nvxL dexout | awk '/all/{print $1}')" >> $scriptpath/../$outputdir/sh2pc_packets-out_dc0.csv
    echo "SeNtInAl,counter,bash,bytes-out_DC0,$size,$(sudo iptables -nvxL dexout | awk '/all/{print $2}')" >> $scriptpath/../$outputdir/sh2pc_bytes-out_dc0.csv

    sudo iptables -Z dexin
    sudo iptables -Z dexout

    $scriptpath/../build/bin/sh2pc_auction_benchmark 1 $size 1 > /dev/null &
    $scriptpath/../build/bin/sh2pc_auction_benchmark 0 $size 1 >> $scriptpath/../$outputdir/sh2pc_nom_dc1.csv
    echo "SeNtInAl,counter,bash,packets-in_DC1,$size,$(sudo iptables -nvxL dexin | awk '/all/{print $1}')" >> $scriptpath/../$outputdir/sh2pc_packets-in_dc1.csv
    echo "SeNtInAl,counter,bash,bytes-in_DC1,$size,$(sudo iptables -nvxL dexin | awk '/all/{print $2}')" >> $scriptpath/../$outputdir/sh2pc_bytes-in_dc1.csv
    echo "SeNtInAl,counter,bash,packets-out_DC1,$size,$(sudo iptables -nvxL dexout | awk '/all/{print $1}')" >> $scriptpath/../$outputdir/sh2pc_packets-out_dc1.csv
    echo "SeNtInAl,counter,bash,bytes-out_DC1,$size,$(sudo iptables -nvxL dexout | awk '/all/{print $2}')" >> $scriptpath/../$outputdir/sh2pc_bytes-out_dc1.csv
done

# cleanup traffic redirection
sudo iptables -D INPUT -i lo -p tcp --dport 55555 -j dexin
sudo iptables -D OUTPUT -p tcp --dport 55555 -j dexout
sudo iptables -D dexin
sudo iptables -D dexout
sudo iptables -X dexin
sudo iptables -X dexout

echo "number of messages test - plotting"
python3 $scriptpath/plotter.py --prefix "sh2pc_nom_OT" \
    --csvlog $scriptpath"/../$outputdir/sh2pc_nom_dc0.csv" \
        $scriptpath"/../$outputdir/sh2pc_nom_dc1.csv" \
    --graphpath $scriptpath"/../$outputdir/sh2pc_nom_OT.pdf" \
    --only-tags ot_num_messages_send_DC0 ot_num_messages_recv_DC0 \
        ot_num_messages_send_DC1 ot_num_messages_recv_DC1 \
    --xlabel "Auction Size" \
    --ylabel "Number of Messages" \
    --custom-legend-labels "dc0-sent" "dc0-recv" "dc1-sent" "dc1-recv"

python3 $scriptpath/plotter.py --prefix "sh2pc_nom_Auction" \
    --csvlog $scriptpath"/../$outputdir/sh2pc_nom_dc0.csv" \
        $scriptpath"/../$outputdir/sh2pc_nom_dc1.csv" \
    --graphpath $scriptpath"/../$outputdir/sh2pc_nom_Auction.pdf" \
    --only-tags auction_num_messages_send_DC0 auction_num_messages_recv_DC0 \
        auction_num_messages_send_DC1 auction_num_messages_recv_DC1 \
    --xlabel "Auction Size" \
    --ylabel "Number of Messages" \
    --custom-legend-labels "dc0-sent" "dc0-recv" "dc1-sent" "dc1-recv"

python3 $scriptpath/plotter.py --prefix "sh2pc_total" \
    --csvlog $scriptpath"/../$outputdir/sh2pc_packets-in_dc0.csv" \
        $scriptpath"/../$outputdir/sh2pc_packets-in_dc1.csv" \
    --graphpath $scriptpath"/../$outputdir/sh2pc_total_packets_in.pdf" \
    --only-tags packets-in_DC0 packets-in_DC1 \
    --xlabel "Auction Size" \
    --ylabel "Packets In" \
    --custom-legend-labels "dc0" "dc1"

python3 $scriptpath/plotter.py --prefix "sh2pc_total" \
    --csvlog $scriptpath"/../$outputdir/sh2pc_bytes-in_dc0.csv" \
        $scriptpath"/../$outputdir/sh2pc_bytes-in_dc1.csv" \
    --graphpath $scriptpath"/../$outputdir/sh2pc_total_bytes_in.pdf" \
    --only-tags bytes-in_DC0 bytes-in_DC1 \
    --xlabel "Auction Size" \
    --ylabel "Bytes In" \
    --custom-legend-labels "dc0" "dc1"

python3 $scriptpath/plotter.py --prefix "sh2pc_total" \
    --csvlog $scriptpath"/../$outputdir/sh2pc_packets-out_dc0.csv" \
        $scriptpath"/../$outputdir/sh2pc_packets-out_dc1.csv" \
    --graphpath $scriptpath"/../$outputdir/sh2pc_total_packets_out.pdf" \
    --only-tags packets-out_DC0 packets-out_DC1 \
    --xlabel "Auction Size" \
    --ylabel "Packets Out" \
    --custom-legend-labels "dc0" "dc1"

python3 $scriptpath/plotter.py --prefix "sh2pc_total" \
    --csvlog $scriptpath"/../$outputdir/sh2pc_bytes-out_dc0.csv" \
        $scriptpath"/../$outputdir/sh2pc_bytes-out_dc1.csv" \
    --graphpath $scriptpath"/../$outputdir/sh2pc_total_bytes_out.pdf" \
    --only-tags bytes-out_DC0 bytes-out_DC1 \
    --xlabel "Auction Size" \
    --ylabel "Bytes Out" \
    --custom-legend-labels "dc0" "dc1"

