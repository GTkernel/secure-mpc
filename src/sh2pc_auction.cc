#include <emp-tool/emp-tool.h>
#include <emp-tool/io/net_io_channel.h>
#include <emp-sh2pc/emp-sh2pc.h>
#include <src/include.h>

using namespace emp;

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

int main(int argc, char** argv) {
    NetIO* io;
    int p = atoi(argv[1]);
    int db_size = atoi(argv[2]);

    if (p == 0) {
        std::cout << "ALICE netio done" << "\n";
        io = new NetIO(nullptr, 55555);
        std::cout << "ALICE netio done" << "\n";
        setup_semi_honest(io, ALICE);
        std::cout << "ALICE setup done" << "\n";

        // Alice wants to find 1 CPU
        std::vector<Attribute> aliceLookingFor = {
            { Integer(32, Unit::CPU, ALICE), Integer(32, 0, ALICE), Integer(32, 1, ALICE), }, // Alice wants 1 CPU
            { Integer(32, Unit::RAM, ALICE), Integer(32, 0, ALICE), Integer(32, 1, ALICE), }, // and 1 RAM
        };

        // Bob has the DB of resources
        std::vector<Resource> db = {};

        // this is warm up for Bob, first OT takes more messages to set things up
        auto warmup = Bit(false, BOB);
        (void) warmup;

        for (int i=0; i<db_size; i++) {

            db.push_back({ // Alice doesn't know the exact entries in Bob's DB
                Integer(32, 0, BOB),
                {
                    { Integer(32, 0, BOB), Integer(32, 0, BOB), Integer(32, 0, BOB), },
                    { Integer(32, 0, BOB), Integer(32, 0, BOB), Integer(32, 0, BOB), },
                    { Integer(32, 0, BOB), Integer(32, 0, BOB), Integer(32, 0, BOB), },
                }
            });

            auto res = auction(aliceLookingFor,db);
            res.reveal<int>();

            std::cout << "Auction result: " << res.reveal<int>() << "\n";
        }
        std::cout << "done\n";

    } else {
        std::cout << "BOB starting netio" << "\n";
        io = new NetIO("127.0.0.1", 55555);
        std::cout << "BOB netio done" << "\n";
        setup_semi_honest(io, BOB);
        std::cout << "BOB setup done" << "\n";

        // Bob doesn't know what Alice wants, but know the number of attributes
        std::vector<Attribute> aliceLookingFor = {
            { Integer(32, 0, ALICE), Integer(32, 0, ALICE), Integer(32, 0, ALICE), },
            { Integer(32, 0, ALICE), Integer(32, 0, ALICE), Integer(32, 0, ALICE), },
        };

        // this is warm up for Bob, first OT takes more messages to set things up
        auto warmup = Bit(false, BOB);
        (void) warmup;

        // Bob has DB of resources
        std::vector<Resource> db = {};

        for (int i=0; i<db_size; i++) {
            db.push_back({
                Integer(32, 1000+i, BOB), //resource id 1001 has the following attributes...
                {
                    { Integer(32, Unit::CPU, BOB), Integer(32, 3, BOB), Integer(32, 1, BOB), }, // 1 CPUs for 3$ each
                    { Integer(32, Unit::RAM, BOB), Integer(32, 2, BOB), Integer(32, 5, BOB), }, // 5 RAMs for 2$ each
                    { Integer(32, Unit::GPU, BOB), Integer(32, 1, BOB), Integer(32, 10, BOB), }, // 10 GPUs for 1$ each
                }
            });
            auto res= auction(aliceLookingFor,db);
            res.reveal<int>();
            std::cout << "Auction result: " << res.reveal<int>() << "\n";
        }
        std::cout << "done\n";
    }
}
