// including the plain protocol execution
#include <emp-tool/execution/plain_prot.h>

// browse the `circuits` subdirectory for different pre-written circuits.
#include <emp-tool/circuits/bit.h>
#include <emp-tool/circuits/circuit_file.h>

#include <iostream>
using namespace emp;
using namespace std;
static const char* FILENAME = "test.txt";

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

Integer auction(std::vector<Attribute> lookingFor, std::vector<Resource> db) {
    Integer globalMinCost(32, INT_MAX, PUBLIC);
    //Integer minId(32, INT_MAX, PUBLIC);

    // For each available resource
    for (auto rec = db.begin(); rec != db.end(); rec++) {

        Integer totalCost(32, 0, ALICE);
        Integer numAttributesSatisfied(32, 0, ALICE);

        // For each attribute of the available resource
        for (auto att = rec->attributes.begin(); att != rec->attributes.end(); att++) {
            // For each attribute requested
            for (auto neededAtt = lookingFor.begin(); neededAtt != lookingFor.end(); neededAtt++) {

                Bit sameUnit = neededAtt->unit == att->unit;
                Bit enoughAvailable = neededAtt->amount <= att->amount;
                //std::cout << sameUnit.reveal() << "\n";
                //std::cout << enoughAvailable.reveal() << "\n";

                Bit kosher = sameUnit & enoughAvailable;
                Integer thisCost = neededAtt->amount * att->price;

                totalCost = totalCost.If(kosher, totalCost + thisCost);
                numAttributesSatisfied = numAttributesSatisfied.If(kosher, numAttributesSatisfied + Integer(32, 1, PUBLIC));
                //std::cout << thisCost.reveal<int>() << "\n";
                //std::cout << totalCost.reveal<int>() << "\n";
            }
        }

        Bit isMin = totalCost < globalMinCost;
        Bit allAttributesSatisfied = numAttributesSatisfied >= Integer(32, lookingFor.size(), PUBLIC);
        globalMinCost = globalMinCost.If(allAttributesSatisfied & isMin, totalCost);
        //std::cout << globalMinCost.reveal<int>() << "\n";
    }

    return globalMinCost;
}

int main() {
    // First, I'll show how to write a circuit out to a file. The high level
    // idea is that we will write our circuit as a highly stylized C++ program
    // in the context of an EMP "protocol execution". This protocol execution
    // will handle the details of constructing the circuit netlist for us.

    // We'll set up a so-called "plain execution", which will allow us to compile
    // our program to a circuit netlist file. I'll write the circuit out to
    // `text.txt`.
    emp::setup_plain_prot(true, FILENAME);
    /*
    // Next, I'll set up a simple MPC where Alice and Bob AND their private bits.
    // Reveals tells emp what should be considered in the output of the circuit.
    emp::Bit a_bit = { false, emp::ALICE };
    emp::Bit b_bit = { false, emp::BOB };
    emp::Bit res_bit = a_bit & b_bit;
    res_bit.reveal<bool>();

    */

    std::vector<Attribute> aliceLookingFor = {
            { Integer(32, Unit::CPU, ALICE), Integer(32, 0, ALICE), Integer(32, 0, ALICE), }, // Alice wants 1 CPU
            { Integer(32, Unit::RAM, ALICE), Integer(32, 0, ALICE), Integer(32, 0, ALICE), }, // and 1 RAM
        };

    std::vector<Resource> db = {
            {
                Integer(32, 0, BOB), //id
                { //attributes
                    { Integer(32, 0, BOB), Integer(32, 0, BOB), Integer(32, 0, BOB), },
                    { Integer(32, 0, BOB), Integer(32, 0, BOB), Integer(32, 0, BOB), },
                    { Integer(32, 0, BOB), Integer(32, 0, BOB), Integer(32, 0, BOB), },
                }
            },
            {
                Integer(32, 0, BOB), //id
                { //attributes
                    { Integer(32, 0, BOB), Integer(32, 0, BOB), Integer(32, 0, BOB), },
                    { Integer(32, 0, BOB), Integer(32, 0, BOB), Integer(32, 0, BOB), },
                    { Integer(32, 0, BOB), Integer(32, 0, BOB), Integer(32, 0, BOB), },
                }
            },
        };

    auto result = auction(aliceLookingFor,db);
    result.reveal<int>();

    // Close the protocol execution.
    emp::finalize_plain_prot();
}
