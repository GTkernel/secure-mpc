#!/bin/bash
scriptpath="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
flamegraphpath=$scriptpath/../../FlameGraph
flamescopepath=$scriptpath/../../flamescope
#set -e

numparties=9
#numparties=3


# Kill old running tests
killcontainers() {
    echo "killing containers..."
    for party in $(seq 2 $numparties); do
        sudo docker stop bob${party} && sudo docker rm bob${party} &>/dev/null &
    done
    sudo docker stop alice && sudo docker rm alice &>/dev/null
    killall agmpc_singleatt_9_auction
    wait
    echo "...done"
    sudo tc qdisc del dev lo root &>/dev/null && echo
}

killcontainers

#sudo tc qdisc add dev lo root netem delay 4ms

#for party in $(seq 2 $numparties); do
#    echo "starting docker bob $party"
#    sudo docker run -d --rm \
#        --name bob${party} \
#        --net=host \
#        --cap-add=NET_ADMIN \
#        -v ${scriptpath}/localhostipaddrs.txt:/dex-mpc/build/bin/ipaddrs.txt \
#        registry.choncholas.com/research/dex/dex-mpc:latest \
#        /dex-mpc/build/bin/agmpc_singleatt_${numparties}_auction \
#            /dex-mpc/build/bin/ipaddrs.txt \
#            /dex-mpc/build/bin/agmpc_output.txt \
#            ${party} $((${party} + 2))
#done

for party in $(seq 2 $numparties); do
    echo "starting non-docker bob $party"
    $scriptpath/../build/bin/agmpc_singleatt_${numparties}_auction \
        $scriptpath/localhostipaddrs.txt \
        $scriptpath/../build/bin/agmpc_output.txt \
        ${party} $((${party} + 2)) &>/dev/null &
done




#echo "perfing alice"
#sudo perf record --call-graph dwarf -F 3999 -g \
#    $scriptpath/../build/bin/agmpc_singleatt_${numparties}_auction \
#    $scriptpath/../build/bin/ipAddrs.txt \
#    $scriptpath/../build/bin/agmpc_output.txt \
#    1
#sudo perf script -f --header > out.perf
#sudo chown $USER:$USER out.perf
#cp out.perf $flamescopepath/examples
##$flamegraphpath/stackcollapse-perf.pl out.perf > out.folded
##$flamegraphpath/flamegraph.pl --width 6400 out.folded > flamez.svg
##xdg-open ./flamez.svg
### See https://www.yld.io/blog/cpu-and-i-o-performance-diagnostics-in-node-js for more


#echo "perfing stat'ing alice"
#sudo perf stat -d \
#    $scriptpath/../build/bin/agmpc_singleatt_${numparties}_auction \
#    $scriptpath/../build/bin/ipAddrs.txt \
#    $scriptpath/../build/bin/agmpc_output.txt \
#    1
#sudo perf stat -e 'syscalls:sys_enter_*' -d \
#    $scriptpath/../build/bin/agmpc_singleatt_${numparties}_auction \
#    $scriptpath/../build/bin/ipAddrs.txt \
#    $scriptpath/../build/bin/agmpc_output.txt \
#    1



#echo "starting alice with syscall counters"
#sudo $scriptpath/../syscount -c \
#    $scriptpath/../build/bin/agmpc_singleatt_${numparties}_auction \
#    $scriptpath/../build/bin/ipAddrs.txt \
#    $scriptpath/../build/bin/agmpc_output.txt \
#    1



#echo "starting alice with timers"
#sudo docker run -it --rm \
#    --name alice \
#    --net=host \
#    --env TIME='SeNtInAl,3dbar,bash,walltime,$numpeers,$msdelay,%E
#SeNtInAl,3dbar,bash,kerntime,$numpeers,$msdelay,%S
#SeNtInAl,3dbar,bash,usrtime,$numpeers,$msdelay,%U
#SeNtInAl,3dbar,bash,cpu,$numpeers,$msdelay,%P
#SeNtInAl,3dbar,bash,elapsed,$numpeers,$msdelay,%e
#SeNtInAl,3dbar,bash,maxram,$numpeers,$msdelay,%M
#SeNtInAl,3dbar,bash,majpfaults,$numpeers,$msdelay,%F
#SeNtInAl,3dbar,bash,minpfaults,$numpeers,$msdelay,%R
#SeNtInAl,3dbar,bash,contextsw-invol,$numpeers,$msdelay,%c
#SeNtInAl,3dbar,bash,contextsw-vol,$numpeers,$msdelay,%w
#SeNtInAl,3dbar,bash,sockrx,$numpeers,$msdelay,%r
#SeNtInAl,3dbar,bash,signaled,$numpeers,$msdelay,%k
#SeNtInAl,3dbar,bash,socktx,$numpeers,$msdelay,%s' \
#    -v ${scriptpath}/localhostipaddrs.txt:/dex-mpc/build/bin/ipaddrs.txt \
#    registry.choncholas.com/research/dex/dex-mpc:latest \
#    time \
#        $scriptpath/../build/bin/agmpc_singleatt_${numparties}_auction \
#        $scriptpath/../build/bin/ipAddrs.txt \
#        $scriptpath/../build/bin/agmpc_output.txt \
#        1



#echo "Running docker alice"
#sudo docker run -it --rm \
#    --name agmpc-alice \
#    --cap-add=NET_ADMIN \
#    -v ${scriptpath}/localhostipaddrs.txt:/dex-mpc/build/bin/ipaddrs.txt \
#    registry.choncholas.com/research/dex/dex-mpc:latest \
#    /dex-mpc/build/bin/agmpc_singleatt_${numparties}_auction \
#        /dex-mpc/build/bin/ipAddrs.txt \
#        /dex-mpc/build/bin/agmpc_output.txt \
#        1

#echo "debugging non-docker alice"
#gdb --args $scriptpath/../build/bin/agmpc_singleatt_${numparties}_auction \
#    $scriptpath/localhostipaddrs.txt \
#    $scriptpath/../build/bin/agmpc_output.txt \
#    1
#sudo setcap -r $scriptpath/../build/bin/agmpc_singleatt_${numparties}_auction # have to remove capadmin :(
#sudo valgrind $scriptpath/../build/bin/agmpc_singleatt_${numparties}_auction \
#    $scriptpath/localhostipaddrs.txt \
#    $scriptpath/../build/bin/agmpc_output.txt \
#    1

echo "running non-docker alice"
$scriptpath/../build/bin/agmpc_singleatt_${numparties}_auction \
    $scriptpath/localhostipaddrs.txt \
    $scriptpath/../build/bin/agmpc_output.txt \
    1


killcontainers
