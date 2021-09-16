#!/bin/bash
scriptpath="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
outputdir=$scriptpath

python3 $scriptpath/../scripts/plotter.py \
    --csvlog "$scriptpath/sharemind_singleatt_results.csv" \
    --graphpath "$outputdir/sharemind_perf.pdf" \
    --only-tags "Sharemind" "RTT to Cloud" \
    --title "Sharemind Runtime Estimation" \
    --xlabel "Number of Auction Participants" \
    --ylabel "Runtime (s)" \
    --color-theme "dracula"

