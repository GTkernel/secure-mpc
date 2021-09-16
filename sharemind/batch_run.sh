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

#PATH_TO_AUCTION=/home/sharemind/Sharemind-SDK/demos/basic/hellosecrec.sc
PATH_TO_AUCTION=/media/sf_SharedFolder/sharemind_compare.cs

### Basic test
# wipe file
echo "" > singlerun.txt
/usr/local/sharemind/bin/sm_compile_and_run.sh $PATH_TO_AUCTION | sed -n '4,4p' >> singlerun.txt


#### Scalability tests
## wipe file
#echo "" > scalability.txt
#
#echo "scalability test - running"
#for i in {1..1000}
#do
#    /usr/local/sharemind/bin/sm_compile_and_run.sh $PATH_TO_AUCTION | sed -n '4,4p' >> scalability.txt
#done




#### Latency tests
## remove any old latency
#sudo tc qdisc del dev lo root &>/dev/null && echo
#
## wipe file
#echo "" > latency.txt
#
#echo "latency test - running"
#for i in {1..100..10}; do
#    ms=$((i / 2))
#    echo "    testing ${i}ms"
#    sudo tc qdisc add dev lo root netem delay ${ms}ms
#    sleep 1
#    /usr/local/sharemind/bin/sm_compile_and_run.sh $PATH_TO_AUCTION | sed -n '4,4p' >> latency.txt
#    sudo tc qdisc del dev lo root netem delay ${ms}ms
#done
