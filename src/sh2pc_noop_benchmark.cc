#include <emp-tool/emp-tool.h>
#include <emp-tool/io/net_io_channel.h>
#include <emp-sh2pc/emp-sh2pc.h>
#include <src/include.h>

using namespace emp;

#define NUM_ITERS 10000

int main(int argc, char** argv) {
    NetIO* io;
    int p = atoi(argv[1]);
    int ms_delay = atoi(argv[2]);
    int depConfig = atoi(argv[3]); // 0=Bob and Alice share data; 1=Bob has all data

    auto test = depConfig ? "BOB" : "ALICE";
    std::cout << "depConfig will choose: " << test << "\n";

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



        // measure OT
        io->num_tx = 0;
        io->num_rx = 0;
        //WALL_TIC(ot_DC0);
        static time_point<high_resolution_clock> ticotps_DC0 = high_resolution_clock::now();
        for (int i = 0; i < NUM_ITERS; i++) {
            Integer t(32, 0, BOB);
        }
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
        MSG("SeNtInAl,xy,%s,%s,%d,%g\n", __FUNCTION__, "otps_DC0", ms_delay, NUM_ITERS/seconds);



        // measure comparisons
        Integer a(32, 4, ALICE);
        Integer b(32, 0, BOB); // calls ot->send_cot() on SHOTExtension
        io->num_tx = 0;
        io->num_rx = 0;
        //WALL_TIC(auction_DC0);
        static time_point<high_resolution_clock> ticcomparisonps_DC0 = high_resolution_clock::now();

        for (int i = 0; i < NUM_ITERS; i++) {
            Bit res = a>b;
            (void) res;
        }

        //WALL_TOC_CSV_XY(auction_DC0, ms_delay);
        time_point<high_resolution_clock> toccomparisonps_DC0 = high_resolution_clock::now();
        auto secondss = std::chrono::duration_cast<std::chrono::microseconds>(toccomparisonps_DC0 - ticcomparisonps_DC0).count() / 1000000.0;
        MSG("SeNtInAl,xy,%s,%s,%d,%g\n", __FUNCTION__, "comparisonps_DC0", ms_delay, NUM_ITERS/secondss);
        MSG("SeNtInAl,xy,%s,%s,%d,%d\n", __FUNCTION__, "auction_num_messages_send_DC0", ms_delay, io->num_tx);
        MSG("SeNtInAl,xy,%s,%s,%d,%d\n", __FUNCTION__, "auction_num_messages_recv_DC0", ms_delay, io->num_rx);



        // measure together
        static time_point<high_resolution_clock> tictotalps_DC0 = high_resolution_clock::now();
        for (int i = 0; i < NUM_ITERS; i++) {
            Integer c(32, 4, ALICE);
            Integer d(32, 0, BOB); // calls ot->send_cot() on SHOTExtension
            Bit res = c>d;
            bool realres = res.reveal<bool>();
            (void) realres;
        }
        time_point<high_resolution_clock> toctotalps_DC0 = high_resolution_clock::now();
        auto secondsss = std::chrono::duration_cast<std::chrono::microseconds>(toctotalps_DC0 - tictotalps_DC0).count() / 1000000.0;
        MSG("SeNtInAl,xy,%s,%s,%d,%g\n", __FUNCTION__, "totalps_DC0", ms_delay, NUM_ITERS/secondsss);




    } else {
        // Generator = Alice
        std::cout << "ALICE netio making connection to Bob" << "\n";
        io = new NetIO("192.168.2.6", 55555);
        std::cout << "ALICE netio done" << "\n";
        setup_semi_honest(io, ALICE);
        std::cout << "ALICE setup done" << "\n";

        // warm up
        Integer t(32, 0, BOB);

        // measure OT
        for (int i = 0; i < NUM_ITERS; i++) {
            Integer t(32, 0, BOB);
        }

        // measure comparisons
        Integer a(32, 0, ALICE);
        Integer b(32, 5, BOB);
        for (int i = 0; i < NUM_ITERS; i++) {
            Bit res = a>b;
            bool realres = res.reveal<bool>();
            (void) realres;
        }


        // measure together
        for (int i = 0; i < NUM_ITERS; i++) {
            Integer c(32, 0, ALICE);
            Integer d(32, 5, BOB);
            Bit res = c>d;
            bool realres = res.reveal<bool>();
            (void) realres;
        }

    }
}
