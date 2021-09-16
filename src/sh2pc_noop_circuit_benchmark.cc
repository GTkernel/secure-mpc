#include <emp-tool/emp-tool.h>
#include <emp-tool/io/net_io_channel.h>
#include <emp-sh2pc/emp-sh2pc.h>
#include <src/include.h>

#include <libgen.h>         // dirname
#include <unistd.h>         // readlink
#include <linux/limits.h>   // PATH_MAX

using namespace emp;

#define NUM_ITERS 10000


// WARNING - these results are very misleading because
// there is no OT happening - just circuit execution.

int main(int argc, char** argv) {
    NetIO* io;
    int p = atoi(argv[1]);
    int ms_delay = atoi(argv[2]);
    int depConfig = atoi(argv[3]); // 0=Bob and Alice share data; 1=Bob has all data

    auto test = depConfig ? "BOB" : "ALICE";
    std::cout << "depConfig will choose: " << test << "\n";

    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    const char *path;
    if (count != -1) {
        path = dirname(result);
    } else {
        error("cant find my own path\n");
        return 1;
    }
    std::string circuitPath = string(path) + "/noop_circuit.txt";
    BristolFormat cf(circuitPath.c_str());

    block *ain = new block[cf.n1];
    block *bin = new block[cf.n2];
    block *out = new block[cf.n3];
    printf("sizes: %d - %d - %d\n", cf.n1, cf.n2, cf.n3);

    PRG prg;
    prg.random_block(ain, cf.n1);
    prg.random_block(bin, cf.n2);

    // Evaluator = Bob
    if (p == 0) {
        WALL_CLOCK(setup);
        WALL_CLOCK(ot_DC0);
        WALL_CLOCK(ot_DC1);
        WALL_CLOCK(auction_DC0);
        WALL_CLOCK(auction_DC1);

        WALL_TIC(setup);
        std::cout << "BOB netio done" << "\n";
        io = new NetIO(nullptr, 55555);
        std::cout << "BOB netio done" << "\n";
        setup_semi_honest(io, BOB);
        std::cout << "BOB setup done" << "\n";
        WALL_TOC_CSV_BOX(setup, setup);

        // warm up
        Integer t(32, 0, BOB);

        HalfGateEva<NetIO>::circ_exec = new HalfGateEva<NetIO>(io);

        io->num_tx = 0;
        io->num_rx = 0;
        //WALL_TIC(ot_DC0);
        static time_point<high_resolution_clock> ticotps_DC0 = high_resolution_clock::now();
        for(int i = 0; i < NUM_ITERS; ++i)
            cf.compute(ain, bin, out);
        MSG("SeNtInAl,xy,%s,%s,%d,%d\n", __FUNCTION__, "ot_num_messages_send_DC0", ms_delay, io->num_tx);
        MSG("SeNtInAl,xy,%s,%s,%d,%d\n", __FUNCTION__, "ot_num_messages_recv_DC0", ms_delay, io->num_rx);
        // sync after ot to measure true runtime?
        //uint8_t tosend=0x77;
        //io->send_data(&tosend, 1);
        //uint8_t torecv;
        //io->recv_data(&torecv, 1);
        //assert(torecv == 0x77);
        //WALL_TOC_CSV_XY(ot_DC0, ms_delay);
        time_point<high_resolution_clock> tocotps_DC0 = high_resolution_clock::now();
        auto seconds = std::chrono::duration_cast<std::chrono::microseconds>(tocotps_DC0 - ticotps_DC0).count() / 1000000.0;
        MSG("SeNtInAl,xy,%s,%s,%d,%g\n", __FUNCTION__, "circuitps_DC0", ms_delay, NUM_ITERS/seconds);


    } else {
        // Generator = Alice
        std::cout << "ALICE netio making connection to Bob" << "\n";
        io = new NetIO("127.0.0.1", 55555);
        std::cout << "ALICE netio done" << "\n";
        setup_semi_honest(io, ALICE);
        std::cout << "ALICE setup done" << "\n";

        // warm up
        Integer t(32, 0, BOB);

        HalfGateGen<NetIO>::circ_exec = new HalfGateGen<NetIO>(io);

        for(int i = 0; i < NUM_ITERS; ++i) {
            cf.compute(ain, bin, out);
        }
    }
}
