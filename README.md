# NOTE
This repo is a work in progress. Please contact [James Choncholas](james.choncholas.com) for more information.

# Building and Running EMP

The auction can run in 3 security models, semi-honest 2pc, publicly verifiable covert, and authenticated garbling 2pc.

Specifically for semihonest 2pc there are a few different applications.
- sh2pc\_auction is a usable application that computes an auction for resources.
- sh2pc\_auction\_benchmark is a tool to run the auction repeatedly with increasing size and performance measurements.
- sh2pc\_auction\_benchmark\_dc2 is just like benchmark except the evaluator (bob) has ALL the data.


## Flame Graphs
1. install FlameGraph repo and point path in docker/run.sh

## sh2pc installation instructions:

1.  Install necessary dependencies
    ```bash
    > ./install.sh
    ```

1.  Build and run the sh2pc tests
    ```bash
    > ./scripts/build_all.sh
    > ./scripts/run_all.sh
    ```


## pvc installation instructions:

1. First install normal dependancies.
    ```bash
    > ./install.sh
    ```
1. Build a circuit file with circuit generator program.
    ```bash
    > mkdir build
    > cd build
    > cmake ..
    > make
    > ./bin/gen_auction
    ```

1. Install special pvc dependancies.
    ```bash
    > ./install_pvc.sh
    ```

1. Copy the circuit to PVC location
    ```bash
    > cp to pvc test dir with special name (aes or something)
    ```

1. Run the pvc test but with the new circuit
    ```bash
    > cd emp-pvc/test
    > run the test
    ```

## AG installation instructions:
Copy the following two lines into emp-agmpc/CMakeLists.txt right above the test cases

```
install(DIRECTORY emp-agmpc DESTINATION include)
install(DIRECTORY cmake/ DESTINATION cmake)
```

Run make install from that directory
