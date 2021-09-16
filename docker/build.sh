#!/bin/bash
scriptpath="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd $scriptpath/..

sudo docker build \
    -f docker/Dockerfile \
    -t reg.choncholas.com/research/dex/dex-mpc:latest \
    .

sudo docker push reg.choncholas.com/research/dex/dex-mpc:latest
