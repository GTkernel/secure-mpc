#include <emp-tool/emp-tool.h>
//#include <emp-tool/circuits/integer.h>
//#include <emp-tool/io/net_io_channel.h>
#include <emp-agmpc/emp-agmpc.h>
//#include <emp-ag2pc/fpre.h>
#include <src/include.h>
#include <iostream>
#include <emp-ot/emp-ot.h>

#include <libgen.h>         // dirname
#include <unistd.h>         // readlink
#include <linux/limits.h>   // PATH_MAX

using namespace emp;
using namespace std;

#define APP_DEBUG 0

const static int nP = 5;

static const string outputDir = "/tmp/";

int party, bobInput;
void bench_once(NetIOMP<nP> * ios[2], ThreadPool * pool, string outputfp) {

    int lowest = INT_MAX;
    int secondPrice = INT_MAX;
    int globalMinIndex = INT_MAX;

    if(party != 1) {
        int myBid = bobInput;
        int myId = party;
        ios[0]->send_data(1, &myBid, sizeof(int));
        ios[0]->send_data(1, &myId, sizeof(int));
    } else {
        for (int i = 2; i <= nP; ++i) {
            int bid;
            int bidId;
            ios[0]->recv_data(i, &bid, sizeof(int));
            ios[0]->recv_data(i, &bidId, sizeof(int));
            if (bid < lowest) {
                secondPrice = lowest;
                lowest = bid;
                globalMinIndex = bidId;
            } else if (bid < secondPrice) {
                secondPrice = bid;
            }
        }
    }

    // broadcast output
    if(party != 1) {
        ios[0]->recv_data(1, &secondPrice, sizeof(int));
        ios[0]->recv_data(1, &globalMinIndex, sizeof(int));
    } else {
        for (int i = 2; i <= nP; ++i) {
            ios[0]->send_data(i, &secondPrice, sizeof(int));
            ios[0]->send_data(i, &globalMinIndex, sizeof(int));
        }
    }

    cout << "secondPrice = " << secondPrice << std::endl;
    cout << "globalMinIdex = " << globalMinIndex << std::endl;

    ofstream outputfs;
    outputfs.open (outputfp, ios::trunc | ios::out);
    outputfs << to_string(globalMinIndex) << " " << to_string(secondPrice) << "\n";
    outputfs.close();
}


int main(int argc, char** argv) {
    if (argc < 4 || argc > 6) {
        error("Usage: plain_singleatt_auction <ipaddr filepath> <output filepath> <party index> [bob_bid] [ms_logging]\n");
    }
    std::string ipFilePath = string(argv[1]);
    std::string outputFilePath = string(argv[2]);
    party = atoi(argv[3]);
    if (party > nP) {
        cout << "party out of range\n";
        return 1;
    }
    if (party > 1 && argc <= 4) {
        cout << "bob needs to supply input bid\n";
        return 1;
    }

    if (argc >= 5) {
        bobInput = atoi(argv[4]);
        if (argc >= 6) {
            //msLogger = atoi(argv[5]);
        }
    }

    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    const char *path;
    if (count != -1) {
        path = dirname(result);
    } else {
        error("cant find my own path\n");
        return 1;
    }

    ofstream outputfs;
    outputfs.open(outputFilePath, ios::trunc | ios::out);
    outputfs.close();

    ThreadPool pool(2*(nP-1)+2);
    NetIOMP<nP> io(party, ipFilePath, 0, &pool);
    NetIOMP<nP> io2(party, ipFilePath, 2*(nP+1), &pool);
    NetIOMP<nP> *ios[2] = {&io, &io2};

    static time_point<high_resolution_clock> tic = high_resolution_clock::now();
    bench_once(ios, &pool, outputFilePath);
    time_point<high_resolution_clock> toc = high_resolution_clock::now();
    auto seconds = std::chrono::duration_cast<std::chrono::microseconds>(toc - tic).count() / 1000000.0;
    MSG("SeNtInAl,xy,%s,%s,%d,%g\n", __FUNCTION__, "e2e_plain", nP, seconds);
    return 0;
}
