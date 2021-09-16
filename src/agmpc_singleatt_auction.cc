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

const static int nP = 3;

static const int ALICE_NUM_WIRES = 5*32;
static const int BOB_NUM_WIRES_EACH = 1*32;

static const string outputDir = "/tmp/";

#define ADD_INT_TO_BOOLARR(NUM_, INDEX_, BOOL_ARR_) do { \
    int tmp = INDEX_ + 32; \
    int tmpNum = NUM_; \
    while (tmpNum > 0) { \
        BOOL_ARR_[INDEX_] = tmpNum & 1; \
        tmpNum = tmpNum >> 1; \
        INDEX_ ++; \
    } \
    INDEX_ = tmp; \
} while (0)

#define GET_INT_FROM_BOOLARR(NUM_, INDEX_, BOOL_ARR_) do { \
    int res = 0; \
    for (int macroi = INDEX_; macroi < INDEX_+32; macroi++) { \
        int lol = BOOL_ARR_[macroi]; \
        res |= lol << macroi; \
    } \
    NUM_ = res; \
    INDEX_ += 32; \
} while (0)

int party, bobInput, msLogger=0;

void resetCounters(NetIOMP<nP> * ios[2]) {
    for(int i = 1; i <= nP; ++i)for(int j = 1; j <= nP; ++j)if(i < j){
        int toPrint=0;
        if(i == party) {
            toPrint=j;
        } else if(j == party) {
            toPrint=i;
        }

        if (toPrint) {
            ios[0]->ios[toPrint]->num_tx = 0;
            ios[0]->ios[toPrint]->num_rx = 0;
            ios[0]->ios[toPrint]->size_tx = 0;
            ios[0]->ios[toPrint]->size_rx = 0;

            ios[0]->ios2[toPrint]->num_tx = 0;
            ios[0]->ios2[toPrint]->num_rx = 0;
            ios[0]->ios2[toPrint]->size_tx = 0;
            ios[0]->ios2[toPrint]->size_rx = 0;

            ios[1]->ios[toPrint]->num_tx = 0;
            ios[1]->ios[toPrint]->num_rx = 0;
            ios[1]->ios[toPrint]->size_tx = 0;
            ios[1]->ios[toPrint]->size_rx = 0;

            ios[1]->ios2[toPrint]->num_tx = 0;
            ios[1]->ios2[toPrint]->num_rx = 0;
            ios[1]->ios2[toPrint]->size_tx = 0;
            ios[1]->ios2[toPrint]->size_rx = 0;

            ios[0]->ios[toPrint]->num_flush = 0;
            ios[0]->ios2[toPrint]->num_flush = 0;
            ios[1]->ios[toPrint]->num_flush = 0;
            ios[1]->ios2[toPrint]->num_flush = 0;
        }
    }
}

void printCounters(NetIOMP<nP> * ios[2], string prefix) {

    for(int i = 1; i <= nP; ++i)for(int j = 1; j <= nP; ++j)if(i < j){
        int toPrint=0;
        if(i == party) {
            toPrint=j;
        } else if(j == party) {
            toPrint=i;
        }

        if (toPrint) {
            MSG("outbound to party %d\n",toPrint);
            MSG("SeNtInAl,3dprojectx,%s,%s%s%d,%d,%d,%d\n", __FUNCTION__, prefix.c_str(),"-io0-numtx-party",j, nP, msLogger, ios[0]->ios[toPrint]->num_tx);
            MSG("SeNtInAl,3dprojectx,%s,%s%s%d,%d,%d,%d\n", __FUNCTION__, prefix.c_str(),"-io0-numrx-party",j, nP, msLogger, ios[0]->ios[toPrint]->num_rx);
            MSG("SeNtInAl,3dprojectx,%s,%s%s%d,%d,%d,%d\n", __FUNCTION__, prefix.c_str(),"-io0-sizetx-party",j, nP, msLogger, ios[0]->ios[toPrint]->size_tx);
            MSG("SeNtInAl,3dprojectx,%s,%s%s%d,%d,%d,%d\n", __FUNCTION__, prefix.c_str(),"-io0-sizerx-party",j, nP, msLogger, ios[0]->ios[toPrint]->size_rx);
            MSG("inbound from party %d\n",toPrint);
            MSG("SeNtInAl,3dprojectx,%s,%s%s%d,%d,%d,%d\n", __FUNCTION__, prefix.c_str(),"-io1-numtx-party",j, nP, msLogger, ios[0]->ios2[toPrint]->num_tx);
            MSG("SeNtInAl,3dprojectx,%s,%s%s%d,%d,%d,%d\n", __FUNCTION__, prefix.c_str(),"-io1-numrx-party",j, nP, msLogger, ios[0]->ios2[toPrint]->num_rx);
            MSG("SeNtInAl,3dprojectx,%s,%s%s%d,%d,%d,%d\n", __FUNCTION__, prefix.c_str(),"-io1-sizetx-party",j, nP, msLogger, ios[0]->ios2[toPrint]->size_tx);
            MSG("SeNtInAl,3dprojectx,%s,%s%s%d,%d,%d,%d\n", __FUNCTION__, prefix.c_str(),"-io1-sizerx-party",j, nP, msLogger, ios[0]->ios2[toPrint]->size_rx);

            MSG("abit outbound to party %d\n",toPrint);
            MSG("SeNtInAl,3dprojectx,%s,%s%s%d,%d,%d,%d\n", __FUNCTION__, prefix.c_str(),"-io0-abit-numtx-party",j, nP, msLogger, ios[1]->ios[toPrint]->num_tx);
            MSG("SeNtInAl,3dprojectx,%s,%s%s%d,%d,%d,%d\n", __FUNCTION__, prefix.c_str(),"-io0-abit-numrx-party",j, nP, msLogger, ios[1]->ios[toPrint]->num_rx);
            MSG("SeNtInAl,3dprojectx,%s,%s%s%d,%d,%d,%d\n", __FUNCTION__, prefix.c_str(),"-io0-abit-sizetx-party",j, nP, msLogger, ios[1]->ios[toPrint]->size_tx);
            MSG("SeNtInAl,3dprojectx,%s,%s%s%d,%d,%d,%d\n", __FUNCTION__, prefix.c_str(),"-io0-abit-sizerx-party",j, nP, msLogger, ios[1]->ios[toPrint]->size_rx);
            MSG("abit inbound from party %d\n",toPrint);
            MSG("SeNtInAl,3dprojectx,%s,%s%s%d,%d,%d,%d\n", __FUNCTION__, prefix.c_str(),"-io1-abit-numtx-party",j, nP, msLogger, ios[1]->ios2[toPrint]->num_tx);
            MSG("SeNtInAl,3dprojectx,%s,%s%s%d,%d,%d,%d\n", __FUNCTION__, prefix.c_str(),"-io1-abit-numrx-party",j, nP, msLogger, ios[1]->ios2[toPrint]->num_rx);
            MSG("SeNtInAl,3dprojectx,%s,%s%s%d,%d,%d,%d\n", __FUNCTION__, prefix.c_str(),"-io1-abit-sizetx-party",j, nP, msLogger, ios[1]->ios2[toPrint]->size_tx);
            MSG("SeNtInAl,3dprojectx,%s,%s%s%d,%d,%d,%d\n", __FUNCTION__, prefix.c_str(),"-io1-abit-sizerx-party",j, nP, msLogger, ios[1]->ios2[toPrint]->size_rx);

            MSG("total interaction with party %d\n",toPrint);
            MSG("SeNtInAl,3dprojectx,%s,%s%s%d,%d,%d,%d\n", __FUNCTION__, prefix.c_str(),"-numtx-party",j, nP, msLogger, ios[0]->ios[toPrint]->num_tx +
                                                                                                                    ios[0]->ios2[toPrint]->num_tx +
                                                                                                                    ios[1]->ios[toPrint]->num_tx +
                                                                                                                    ios[1]->ios2[toPrint]->num_tx);
            MSG("SeNtInAl,3dprojectx,%s,%s%s%d,%d,%d,%d\n", __FUNCTION__, prefix.c_str(),"-numrx-party",j, nP, msLogger, ios[0]->ios[toPrint]->num_rx +
                                                                                                                    ios[0]->ios2[toPrint]->num_rx +
                                                                                                                    ios[1]->ios[toPrint]->num_rx +
                                                                                                                    ios[1]->ios2[toPrint]->num_rx);
            MSG("SeNtInAl,3dprojectx,%s,%s%s%d,%d,%d,%d\n", __FUNCTION__, prefix.c_str(),"-sizetx-party",j, nP,msLogger, ios[0]->ios[toPrint]->size_tx +
                                                                                                                    ios[0]->ios2[toPrint]->size_tx +
                                                                                                                    ios[1]->ios[toPrint]->size_tx +
                                                                                                                    ios[1]->ios2[toPrint]->size_tx);
            MSG("SeNtInAl,3dprojectx,%s,%s%s%d,%d,%d,%d\n", __FUNCTION__, prefix.c_str(),"-sizerx-party",j, nP,msLogger, ios[0]->ios[toPrint]->size_rx +
                                                                                                                    ios[0]->ios2[toPrint]->size_rx +
                                                                                                                    ios[1]->ios[toPrint]->size_rx +
                                                                                                                    ios[1]->ios2[toPrint]->size_rx);

            MSG("flushes so far for party %d\n",toPrint);
            MSG("SeNtInAl,3dprojectx,%s,%s%s%d,%d,%d,%d\n", __FUNCTION__, prefix.c_str(),"-io0-flushes-party",j, nP, msLogger, ios[0]->ios[toPrint]->num_flush);
            MSG("SeNtInAl,3dprojectx,%s,%s%s%d,%d,%d,%d\n", __FUNCTION__, prefix.c_str(),"-io1-flushes-party",j, nP, msLogger, ios[0]->ios2[toPrint]->num_flush);
            MSG("SeNtInAl,3dprojectx,%s,%s%s%d,%d,%d,%d\n", __FUNCTION__, prefix.c_str(),"-io0-abit-flushes-party",j, nP, msLogger, ios[1]->ios[toPrint]->num_flush);
            MSG("SeNtInAl,3dprojectx,%s,%s%s%d,%d,%d,%d\n", __FUNCTION__, prefix.c_str(),"-io1-abit-flushes-party",j, nP, msLogger, ios[1]->ios2[toPrint]->num_flush);
        }
    }
}

void bench_once(NetIOMP<nP> * ios[2], ThreadPool * pool, string cffp, string outputfp) {
    if(party == 1)cout <<"CIRCUIT:\t"<<cffp<<endl;
    //string file = circuit_file_location+"/"+cffp;
    BristolFormat cf(cffp.c_str());

    resetCounters(ios);
    auto start = clock_start();
    CMPC<nP>* mpc = new CMPC<nP>(ios, pool, party, &cf);
    ios[0]->flush();
    ios[1]->flush();
    double t2 = time_from(start);
//  ios[0]->sync();
//  ios[1]->sync();
    if(party == 1) MSG("SeNtInAl,3dbar,%s,%s,%d,%d,%.0f\n", __FUNCTION__, "mpc_setup_us", nP, msLogger, t2);
    printCounters(ios, "setup");


    resetCounters(ios);
    start = clock_start();
    mpc->function_independent();
    ios[0]->flush();
    ios[1]->flush();
    t2 = time_from(start);
    if(party == 1) MSG("SeNtInAl,3dbar,%s,%s,%d,%d,%.0f\n", __FUNCTION__, "mpc_indep_us", nP, msLogger, t2);
    printCounters(ios, "indep");

    resetCounters(ios);
    start = clock_start();
    mpc->function_dependent();
    ios[0]->flush();
    ios[1]->flush();
    t2 = time_from(start);
    if(party == 1) MSG("SeNtInAl,3dbar,%s,%s,%d,%d,%.0f\n", __FUNCTION__, "mpc_depen_us", nP, msLogger, t2);
    printCounters(ios, "depen");


    bool *in = new bool[cf.n1+cf.n2];
    memset(in, false, cf.n1+cf.n2);

    bool *out = new bool[cf.n3];
    memset(out, false, cf.n3);

    int *partyStart = new int[nP+1];
    int *partyEnd = new int[nP+1];
    partyStart[0] = 0; // unused
    partyEnd[0] = 0; // unused

    partyStart[1] = 0;
    partyEnd[1] = partyStart[1]+ALICE_NUM_WIRES;
    for (int i=2; i <= nP; i++) {
        partyStart[i] = partyEnd[i-1];
        partyEnd[i] = partyStart[i] + BOB_NUM_WIRES_EACH;
    }

#if APP_DEBUG
    for (int i = 0; i <= nP; i++) {
        cout << "partyStart[" << i << "] = " << partyStart[i];// << std::endl;
        cout << "\tpartyEnd[" << i << "] = " << partyEnd[i] << std::endl;
    }
#endif

    assert(partyEnd[nP] == cf.n1+cf.n2);

    if (party==1) {
        int i = partyStart[party];

        // Set up first 8 constants
        int globalMinIndex = 0;
        int globalMinCost = INT_MAX;
        int secondPrice = INT_MAX;
        int curIndex = 2;
        int one = 1;
        ADD_INT_TO_BOOLARR(globalMinIndex, i, in);
        ADD_INT_TO_BOOLARR(globalMinCost, i, in);
        ADD_INT_TO_BOOLARR(secondPrice, i, in);
        ADD_INT_TO_BOOLARR(curIndex, i, in);
        ADD_INT_TO_BOOLARR(one, i, in);
        assert(i == partyEnd[party]);

        //cout << "p1 - first number= " << std::bitset<32>(numAttributes) << endl;
        //for (int i = partyStart[party]+32; i < partyStart[party] + 64; i++) {
        //    cout << "p1 - in[" << i << "] = " << in[i] << std::endl;
        //}

    } else {
        int i = partyStart[party];
        ADD_INT_TO_BOOLARR(bobInput, i, in); //price
        assert(i == partyEnd[party]);
    }


    resetCounters(ios);
    start = clock_start();
    mpc->online(in, out, partyStart, partyEnd, true);

    int i=0;
    int globalMinIndex=-1;
    int secondPrice=-1;

    // party 1 reconstructs first
    if (party == 1) {
        GET_INT_FROM_BOOLARR(globalMinIndex, i, out);
        GET_INT_FROM_BOOLARR(secondPrice, i, out);
        assert(i==cf.n3);

        cout << "globalMinIndex = " << globalMinIndex << std::endl;
        cout << "secondPrice = " << secondPrice << std::endl;

        mpc->broadcast_output(out, globalMinIndex);
    } else {
        if (mpc->broadcast_output(out, -1)) { // we won winning bid
            GET_INT_FROM_BOOLARR(globalMinIndex, i, out);
            GET_INT_FROM_BOOLARR(secondPrice, i, out);
            assert(i==cf.n3);

            cout << "globalMinIndex = " << globalMinIndex << std::endl;
            cout << "secondPrice = " << secondPrice << std::endl;
        } else {
            cout << "we lost" << std::endl;
        }
    }
    ios[0]->flush();
    ios[1]->flush();
    t2 = time_from(start);
    //uint64_t band2 = io.count();
    //if(party == 1)cout <<"bandwidth\t"<<party<<"\t"<<band2<<endl;
    if(party == 1) MSG("SeNtInAl,3dbar,%s,%s,%d,%d,%.0f\n", __FUNCTION__, "mpc_online_us", nP, msLogger, t2);
    printCounters(ios, "online");

    int globalTotalFlushes=0;
    // Print GLOBAL counters
    for(int i = 1; i <= nP; ++i)for(int j = 1; j <= nP; ++j)if(i < j){
        int toPrint=0;
        if(i == party) {
            toPrint=j;
        } else if(j == party) {
            toPrint=i;
        }

        if (toPrint) {
            MSG("outbound to party %d\n",toPrint);
            MSG("SeNtInAl,3dprojectx,%s,%s%d,%d,%d,%d\n", __FUNCTION__, "total-numtx-party",toPrint, nP, msLogger, ios[0]->ios[toPrint]->global_num_tx);
            MSG("SeNtInAl,3dprojectx,%s,%s%d,%d,%d,%d\n", __FUNCTION__, "total-numrx-party",toPrint, nP, msLogger, ios[0]->ios[toPrint]->global_num_rx);
            MSG("SeNtInAl,3dprojectx,%s,%s%d,%d,%d,%d\n", __FUNCTION__, "total-sizetx-party",toPrint, nP, msLogger, ios[0]->ios[toPrint]->global_size_tx);
            MSG("SeNtInAl,3dprojectx,%s,%s%d,%d,%d,%d\n", __FUNCTION__, "total-sizerx-party",toPrint, nP, msLogger, ios[0]->ios[toPrint]->global_size_rx);
            MSG("inbound from party %d\n",toPrint);
            MSG("SeNtInAl,3dprojectx,%s,%s%d,%d,%d,%d\n", __FUNCTION__, "total-numtx-party",toPrint, nP, msLogger, ios[0]->ios2[toPrint]->global_num_tx);
            MSG("SeNtInAl,3dprojectx,%s,%s%d,%d,%d,%d\n", __FUNCTION__, "total-numrx-party",toPrint, nP, msLogger, ios[0]->ios2[toPrint]->global_num_rx);
            MSG("SeNtInAl,3dprojectx,%s,%s%d,%d,%d,%d\n", __FUNCTION__, "total-sizetx-party",toPrint, nP, msLogger, ios[0]->ios2[toPrint]->global_size_tx);
            MSG("SeNtInAl,3dprojectx,%s,%s%d,%d,%d,%d\n", __FUNCTION__, "total-sizerx-party",toPrint, nP, msLogger, ios[0]->ios2[toPrint]->global_size_rx);

            MSG("abit outbound to party %d\n",toPrint);
            MSG("SeNtInAl,3dprojectx,%s,%s%d,%d,%d,%d\n", __FUNCTION__, "total-abit-numtx-party",toPrint, nP, msLogger, ios[1]->ios[toPrint]->global_num_tx);
            MSG("SeNtInAl,3dprojectx,%s,%s%d,%d,%d,%d\n", __FUNCTION__, "total-abit-numrx-party",toPrint, nP, msLogger, ios[1]->ios[toPrint]->global_num_rx);
            MSG("SeNtInAl,3dprojectx,%s,%s%d,%d,%d,%d\n", __FUNCTION__, "total-abit-sizetx-party",toPrint, nP, msLogger, ios[1]->ios[toPrint]->global_size_tx);
            MSG("SeNtInAl,3dprojectx,%s,%s%d,%d,%d,%d\n", __FUNCTION__, "total-abit-sizerx-party",toPrint, nP, msLogger, ios[1]->ios[toPrint]->global_size_rx);
            MSG("abit inbound from party %d\n",toPrint);
            MSG("SeNtInAl,3dprojectx,%s,%s%d,%d,%d,%d\n", __FUNCTION__, "total-abit-numtx-party",toPrint, nP, msLogger, ios[1]->ios2[toPrint]->global_num_tx);
            MSG("SeNtInAl,3dprojectx,%s,%s%d,%d,%d,%d\n", __FUNCTION__, "total-abit-numrx-party",toPrint, nP, msLogger, ios[1]->ios2[toPrint]->global_num_rx);
            MSG("SeNtInAl,3dprojectx,%s,%s%d,%d,%d,%d\n", __FUNCTION__, "total-abit-sizetx-party",toPrint, nP, msLogger, ios[1]->ios2[toPrint]->global_size_tx);
            MSG("SeNtInAl,3dprojectx,%s,%s%d,%d,%d,%d\n", __FUNCTION__, "total-abit-sizerx-party",toPrint, nP, msLogger, ios[1]->ios2[toPrint]->global_size_rx);

            MSG("total interaction with party %d\n",toPrint);
            MSG("SeNtInAl,3dprojectx,%s,%s%d,%d,%d,%d\n", __FUNCTION__, "global-total-numtx-party",toPrint, nP, msLogger,  ios[0]->ios[toPrint]->global_num_tx +
                                                                                                             ios[0]->ios2[toPrint]->global_num_tx +
                                                                                                             ios[1]->ios[toPrint]->global_num_tx +
                                                                                                             ios[1]->ios2[toPrint]->global_num_tx);
            MSG("SeNtInAl,3dprojectx,%s,%s%d,%d,%d,%d\n", __FUNCTION__, "global-total-numrx-party",toPrint, nP, msLogger,  ios[0]->ios[toPrint]->global_num_rx +
                                                                                                             ios[0]->ios2[toPrint]->global_num_rx +
                                                                                                             ios[1]->ios[toPrint]->global_num_rx +
                                                                                                             ios[1]->ios2[toPrint]->global_num_rx);
            MSG("SeNtInAl,3dprojectx,%s,%s%d,%d,%d,%d\n", __FUNCTION__, "global-total-sizetx-party",toPrint, nP, msLogger, ios[0]->ios[toPrint]->global_size_tx +
                                                                                                             ios[0]->ios2[toPrint]->global_size_tx +
                                                                                                             ios[1]->ios[toPrint]->global_size_tx +
                                                                                                             ios[1]->ios2[toPrint]->global_size_tx);
            MSG("SeNtInAl,3dprojectx,%s,%s%d,%d,%d,%d\n", __FUNCTION__, "global-total-sizerx-party",toPrint, nP, msLogger, ios[0]->ios[toPrint]->global_size_rx +
                                                                                                       ios[0]->ios2[toPrint]->global_size_rx +
                                                                                                       ios[1]->ios[toPrint]->global_size_rx +
                                                                                                       ios[1]->ios2[toPrint]->global_size_rx);

            MSG("total flushes for party %d\n",toPrint);
            MSG("SeNtInAl,3dprojectx,%s,%s%d,%d,%d,%d\n", __FUNCTION__, "total-io0-flushes-party",toPrint, nP, msLogger, ios[0]->ios[toPrint]->global_num_flush);
            MSG("SeNtInAl,3dprojectx,%s,%s%d,%d,%d,%d\n", __FUNCTION__, "total-io1-flushes-party",toPrint, nP, msLogger, ios[0]->ios2[toPrint]->global_num_flush);
            MSG("SeNtInAl,3dprojectx,%s,%s%d,%d,%d,%d\n", __FUNCTION__, "total-io0-abit-flushes-party",toPrint, nP, msLogger, ios[1]->ios[toPrint]->global_num_flush);
            MSG("SeNtInAl,3dprojectx,%s,%s%d,%d,%d,%d\n", __FUNCTION__, "total-io1-abit-flushes-party",toPrint, nP, msLogger, ios[1]->ios2[toPrint]->global_num_flush);
            globalTotalFlushes+=ios[0]->ios[toPrint]->global_num_flush;
            globalTotalFlushes+=ios[0]->ios2[toPrint]->global_num_flush;
            globalTotalFlushes+=ios[1]->ios[toPrint]->global_num_flush;
            globalTotalFlushes+=ios[1]->ios2[toPrint]->global_num_flush;
        }
    }

    MSG("SeNtInAl,3dbar,%s,%s,%d,%d,%d\n", __FUNCTION__, "global-total-flushes", nP, msLogger, globalTotalFlushes);

    ofstream outputfs;
    outputfs.open (outputfp, ios::trunc | ios::out);
    outputfs << to_string(globalMinIndex) << " " << to_string(secondPrice) << "\n";
    outputfs.flush();
    outputfs.close();

    delete[] in;
    delete[] out;
    delete[] partyStart;
    delete[] partyEnd;
    delete mpc;
}

int main(int argc, char** argv) {
    if (argc < 4 || argc > 6) {
        error("Usage: agmpc_singleatt_auction <ipaddr filepath> <output filepath> <party index> [bob_bid] [ms_logging]\n");
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
            msLogger = atoi(argv[5]);
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
    std::string circuitPath = string(path) + "/agmpc_singleatt_" + to_string(nP) + "_circuit.txt";

    ofstream outputfs;
    outputfs.open(outputFilePath, ios::trunc | ios::out);
    outputfs.close();

    ThreadPool pool(2*(nP-1)+2);
    NetIOMP<nP> io(party, ipFilePath, 0, &pool);
    NetIOMP<nP> io2(party, ipFilePath, 2*(nP+1), &pool);
    NetIOMP<nP> *ios[2] = {&io, &io2};

    auto start = clock_start();
    bench_once(ios, &pool, circuitPath, outputFilePath);
    double t2 = time_from(start);

    MSG("SeNtInAl,3dbar,%s,%s,%d,%d,%.0f\n", __FUNCTION__, "e2e-mpc", nP, msLogger, t2);
    return 0;
}
