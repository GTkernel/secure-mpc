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
    //Integer id;
    //Integer geohash;
    // more identity info
    std::vector<Attribute> attributes;
} Resource;

Integer auction(std::vector<Attribute> lookingFor, std::vector<Resource> db) {
    Integer globalMinCost(32, INT_MAX, PUBLIC);
    //Integer minId(32, INT_MAX, PUBLIC);

    // For each available resource
    for (auto rec = db.begin(); rec != db.end(); rec++) {

        Integer totalCost(32, 0, BOB);
        Integer numAttributesSatisfied(32, 0, BOB);

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
    int depConfig = atoi(argv[3]); // 0=Bob and Alice share data; 1=Bob has all data

    auto test = depConfig ? "BOB" : "ALICE";
    std::cout << "depConfig will choose: " << test << "\n";

    // Evaluator = Bob
    if (p == 0) {
        WALL_CLOCK(setup);
        WALL_TIC(setup);
        std::cout << "BOB netio done" << "\n";
        io = new NetIO(nullptr, 55555);
        std::cout << "BOB netio done" << "\n";
        setup_semi_honest(io, BOB);
        std::cout << "BOB setup done" << "\n";
        WALL_TOC_CSV_BOX(setup, setup);

        // compute a full auction (including OT) for auction of size db_size
        WALL_CLOCK(ot_DC0);
        WALL_CLOCK(ot_DC1);
        io->num_tx = 0;
        io->num_rx = 0;
        if (depConfig) {
            WALL_TIC(ot_DC1);
        } else {
            WALL_TIC(ot_DC0);
        }
        std::vector<Attribute> bobLookingFor = {
            { Integer(32, 0, BOB), Integer(32, 0, BOB), Integer(32, 0, BOB), },
        };

        // Alice has the DB of resources
        std::vector<Resource> db = {};

        for (int j=0; j<db_size; j++) {
            db.push_back({ // Bob doesn't know the exact entries in Alice's DB
                {
                    { Integer(32, 0, depConfig ? BOB:ALICE), Integer(32, 0, depConfig ? BOB:ALICE), Integer(32, 0, depConfig ? BOB:ALICE), },
                }
            });
        }
        // sync after ot to measure true runtime?
        //uint8_t tosend=0x77;
        //io->send_data(&tosend, 1);
        //uint8_t torecv;
        //io->recv_data(&torecv, 1);
        //assert(torecv == 0x77);
        if (depConfig) {
            MSG("SeNtInAl,xy,%s,%s,%d,%d\n", __FUNCTION__, "ot_num_messages_send_DC1", db_size, io->num_tx);
            MSG("SeNtInAl,xy,%s,%s,%d,%d\n", __FUNCTION__, "ot_num_messages_recv_DC1", db_size, io->num_rx);
            WALL_TOC_CSV_XY(ot_DC1, db_size);
        } else {
            MSG("SeNtInAl,xy,%s,%s,%d,%d\n", __FUNCTION__, "ot_num_messages_send_DC0", db_size, io->num_tx);
            MSG("SeNtInAl,xy,%s,%s,%d,%d\n", __FUNCTION__, "ot_num_messages_recv_DC0", db_size, io->num_rx);
            WALL_TOC_CSV_XY(ot_DC0, db_size);
        }


        WALL_CLOCK(auction_DC0);
        WALL_CLOCK(auction_DC1);
        io->num_tx = 0;
        io->num_rx = 0;
        if (depConfig) {
            WALL_TIC(auction_DC1);
        } else {
            WALL_TIC(auction_DC0);
        }
        auto res = auction(bobLookingFor,db);
        int realres = res.reveal<int>();
        if (depConfig) {
            WALL_TOC_CSV_XY(auction_DC1, db_size);
            MSG("SeNtInAl,xy,%s,%s,%d,%d\n", __FUNCTION__, "auction_num_messages_send_DC1", db_size, io->num_tx);
            MSG("SeNtInAl,xy,%s,%s,%d,%d\n", __FUNCTION__, "auction_num_messages_recv_DC1", db_size, io->num_rx);
        } else {
            WALL_TOC_CSV_XY(auction_DC0, db_size);
            MSG("SeNtInAl,xy,%s,%s,%d,%d\n", __FUNCTION__, "auction_num_messages_send_DC0", db_size, io->num_tx);
            MSG("SeNtInAl,xy,%s,%s,%d,%d\n", __FUNCTION__, "auction_num_messages_recv_DC0", db_size, io->num_rx);
        }

        std::cout << "Auction result: " << realres << "\n";

    } else {
        // Generator = Alice
        std::cout << "ALICE netio making connection to Bob" << "\n";
        io = new NetIO("192.168.2.6", 55555);
        std::cout << "ALICE netio done" << "\n";
        setup_semi_honest(io, ALICE);
        std::cout << "ALICE setup done" << "\n";

        // compute a full auction (including OT) for auction of size db_size
        // Alice doesn't know what Bob wants, but knows the number of attributes
        std::vector<Attribute> bobLookingFor = {
            { Integer(32, Unit::CPU, BOB), Integer(32, 0, BOB), Integer(32, 1, BOB), },
        };

        // sync after ot to measure true runtime?
        //uint8_t torecv;
        //io->recv_data(&torecv, 1);
        //assert(torecv == 0x77);
        //uint8_t tosend=0x77;
        //io->send_data(&tosend, 1);

        // Alice has DB of resources
        std::vector<Resource> db = {};

        for (int j=0; j<db_size; j++) {
            db.push_back({
                {
                    { Integer(32, Unit::CPU, depConfig ? BOB:ALICE), Integer(32, 3, depConfig ? BOB:ALICE), Integer(32, 1, depConfig ? BOB:ALICE), },
                }
            });
        }
        auto res= auction(bobLookingFor,db);
        int realres = res.reveal<int>();
        std::cout << "Auction result: " << realres << "\n";

    }
}
