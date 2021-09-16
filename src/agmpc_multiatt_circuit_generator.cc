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

typedef enum Unit {
    CPU=1,
    RAM=2,
    GPU=3,
} Unit;

typedef struct Attribute {
    Integer unit;
    Integer price;
    Integer amount;
} Attribute;

typedef struct Resource {
    Integer id;
    //Integer geohash;
    // more identity info
    std::vector<Attribute> attributes;
} Resource;


int main(int argc, char** argv) {
    if (argc != 4)
        error("Usage: agmpc_circuit_generator <Num Alice Attributes> <Num Bobs> <Num Bob Attributes>");
    int numAliceAtt = atoi (argv[1]);
    int numBobs = atoi (argv[2]);
    int numBobAtt = atoi (argv[3]);

    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    const char *path;
    if (count != -1) {
        path = dirname(result);
    } else {
        error("cant find path\n");
        return 1;
    }
    //std::string circuitPath = string(path) + "/agmpc_multiatt_" + to_string(numBobs+1) + "_circuit.txt";
    std::string circuitPath = string(path) + "/agmpc_multiatt_circuit.txt";


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
    Integer globalMinID(32, 0, ALICE);
    Integer globalMinCost(32, INT_MAX, ALICE);
    Integer secondPrice(32, INT_MAX, ALICE);
    Integer numAttributes(32, 2, ALICE);
    Integer garbledZero(32, 0, ALICE);
    Integer garbledOne(32, 1, ALICE);
    Integer totalCost(32, 0, ALICE);
    Integer numAttributesSatisfied(32, 0, ALICE);

    std::vector<Attribute> aliceLookingFor = {};
    for (int i=0; i<numAliceAtt; i++) {
        aliceLookingFor.push_back(
            { Integer(32, Unit::CPU, ALICE), Integer(32, 0, ALICE), Integer(32, 0, ALICE), }
        );
    }

    std::vector<Resource> db = {};
    for (int i=0; i<numBobs; i++) {
        Resource r = { Integer(32, 0, BOB), //id
                {} //attributes
        };
        for (int j=0; j<numBobAtt; j++) {
            r.attributes.push_back(
                { Integer(32, 0, BOB), Integer(32, 0, BOB), Integer(32, 0, BOB), }
            );
        }
        db.push_back(r);
    }

    // For each available resource
    for (auto rec = db.begin(); rec != db.end(); rec++) {

        Integer totalCost = garbledZero;
        Integer numAttributesSatisfied = garbledZero;

        // For each attribute of the available resource
        for (auto att = rec->attributes.begin(); att != rec->attributes.end(); att++) {
            // For each attribute requested
            for (auto neededAtt = aliceLookingFor.begin(); neededAtt != aliceLookingFor.end(); neededAtt++) {

                Bit sameUnit = neededAtt->unit == att->unit;
                Bit enoughAvailable = neededAtt->amount <= att->amount;
                //std::cout << sameUnit.reveal() << "\n";
                //std::cout << enoughAvailable.reveal() << "\n";

                Bit kosher = sameUnit & enoughAvailable;
                Integer thisCost = neededAtt->amount * att->price;

                totalCost = totalCost.If(kosher, totalCost + thisCost);
                numAttributesSatisfied = numAttributesSatisfied.If(kosher, numAttributesSatisfied + garbledOne);
                //std::cout << thisCost.reveal<int>() << "\n";
                //std::cout << totalCost.reveal<int>() << "\n";
            }
        }

        Bit isMin = totalCost < globalMinCost;
        Bit allAttributesSatisfied = numAttributesSatisfied >= numAttributes;
        Bit lowestFound = allAttributesSatisfied & isMin;
        secondPrice = secondPrice.If(lowestFound, globalMinCost);
        globalMinCost = globalMinCost.If(lowestFound, totalCost);
        globalMinID = globalMinID.If(lowestFound, rec->id);
        //std::cout << globalMinCost.reveal<int>() << "\n";
    }

    secondPrice = secondPrice.If(secondPrice > globalMinCost, globalMinCost);

    globalMinID.reveal<int>();
    secondPrice.reveal<int>();

    // Close the protocol execution.
    emp::finalize_plain_prot();
}
