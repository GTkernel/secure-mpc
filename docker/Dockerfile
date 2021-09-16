FROM ubuntu:18.04

WORKDIR /dex-mpc

SHELL ["/bin/bash", "-c"]

# Extras for testing
RUN apt-get update && apt-get install -y \
    time \
    linux-tools-generic \
    linux-tools-5.4.0-54-generic \
    libcap2-bin \
    --no-install-recommends \
    && rm -rf /var/lib/apt/lists/*

COPY ./ .

RUN rm -rf ./build/* && \
    ./scripts/install_deps.sh && \
    ./install.sh && \
    ./scripts/build_libs.sh && \
    ./scripts/build_apps_asis.sh && \
    ./scripts/generate_agmpc_singleatt_bins.sh && \
    ./scripts/generate_plain_singleatt_bins.sh

#ENTRYPOINT ["./sh2pc_auction_benchmark"]
#CMD ["node", "app.js"]

