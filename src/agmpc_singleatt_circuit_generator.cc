// including the plain protocol execution
#include <emp-tool/execution/plain_prot.h>

// browse the `circuits` subdirectory for different pre-written circuits.
#include <emp-tool/circuits/bit.h>
#include <emp-tool/circuits/circuit_file.h>

#include <iostream>

#include <libgen.h>         // dirname
#include <unistd.h>         // readlink
#include <linux/limits.h>   // PATH_MAX

using namespace emp;
using namespace std;

typedef struct Resource {
    Integer price;
} Resource;


int main(int argc, char** argv) {
    if (argc != 2)
        error("Usage: agmpc_circuit_generator <Num Bobs>\n");
    int numBobs = atoi (argv[1]);

    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    const char *path;
    if (count != -1) {
        path = dirname(result);
    } else {
        error("cant find path\n");
        return 1;
    }
    std::string circuitPath = string(path) + "/agmpc_singleatt_" + to_string(numBobs+1) + "_circuit.txt";

    // First, I'll show how to write a circuit out to a file. The high level
    // idea is that we will write our circuit as a highly stylized C++ program
    // in the context of an EMP "protocol execution". This protocol execution
    // will handle the details of constructing the circuit netlist for us.

    // We'll set up a so-called "plain execution", which will allow us to compile
    // our program to a circuit netlist file. I'll write the circuit out to
    // `text.txt`.
    emp::setup_plain_prot(true, circuitPath.c_str());

    /*
    // Next, I'll set up a simple MPC where Alice and Bob AND their private bits.
    // Reveals tells emp what should be considered in the output of the circuit.
    emp::Bit p_bit = { true, emp::PUBLIC };
    emp::Bit a_bit = { false, emp::ALICE };
    emp::Bit b_bit = { false, emp::BOB };
    emp::Bit res_bit = a_bit & b_bit & p_bit;
    res_bit.reveal<bool>();
    */


    /*
    Integer a = { 32, 0, ALICE };
    Integer b = { 32, 0, BOB };
    Bit res_bit = a > b;
    res_bit.reveal<bool>();
    */


    // no public wires in agmpc, alice must supply
    Integer globalMinIndex(32, 0, ALICE);
    Integer globalMinCost(32, INT_MAX, ALICE);
    Integer secondPrice(32, INT_MAX, ALICE);
    Integer curIndex(32, 2, ALICE); // 0=none, 1=alice, 2=first bob
    Integer one(32, 1, ALICE);

    std::vector<Resource> db = {};
    for (int i=0; i<numBobs; i++) {
        Resource r = {
            Integer(32, 0, BOB) // price
        };
        db.push_back(r);
    }

    // For each available resource
    for (auto rec = db.begin(); rec != db.end(); rec++) {
        Bit isMin = rec->price < globalMinCost;
        secondPrice = secondPrice.If(isMin, globalMinCost);
        globalMinCost = globalMinCost.If(isMin, rec->price);
        globalMinIndex = globalMinIndex.If(isMin, curIndex);

        Bit isSecondMin = (!isMin) & (rec->price < secondPrice);
        secondPrice = secondPrice.If(isSecondMin, rec->price);
        curIndex = curIndex + one;
    }

    globalMinIndex.reveal<int>();
    secondPrice.reveal<int>();

    // Close the protocol execution.
    emp::finalize_plain_prot();
}
