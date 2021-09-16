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

static const int INTS_PER_ATTRIBUTE = 3;
static const int ALICE_NUM_ATTRIBUTES = 2;
static const int ALICE_NUM_WIRES = (8*32) + (ALICE_NUM_ATTRIBUTES*INTS_PER_ATTRIBUTE*32);
static const int BOB_NUM_ATTRIBUTES = 3;
static const int BOB_NUM_WIRES_EACH = 32 + (BOB_NUM_ATTRIBUTES*INTS_PER_ATTRIBUTE*32);

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

//// credit: https://github.com/emp-toolkit/emp-agmpc/compare/master...mc2-project:master
//const static int num_parties = NUM_PARTY_FOR_RUNNING;
//
//// This function parses the circuit input file to determine
//// which input wires belong to which party
//std::pair<uint64_t, uint64_t> parse_circuit_input(uint64_t party_id,
//                                                  std::string filename,
//                                                  std::vector<std::pair<uint64_t, uint64_t> > &input_wires) {
//
//  std::ifstream infile(filename.c_str());
//  uint64_t num_inputs = 0, num_outputs = 0;
//  infile >> num_inputs >> num_outputs;
//  uint64_t party = 0, begin = 0, end = 0;
//  while (infile >> party >> begin >> end) {
//    if (party + 1 != party_id) {
//      continue;
//    }
//
//    input_wires.push_back(std::make_pair(begin, end));
//  }
//
//  return std::make_pair(num_inputs, num_outputs);
//}
//
//
//// This function reads the input from "filename" and sets the data in "input"
//// Each party sets a value for every input wire
//// If a party does not provide real input for a wire, the value is set to be 0
//void create_circuit_input(bool *input, std::vector<std::pair<uint64_t, uint64_t> > &input_wires, std::string filename) {
//  uint8_t buffer[1];
//  std::ifstream infile;
//  infile.open(filename.c_str());
//  for (size_t i = 0; i < input_wires.size(); i++) {
//    uint64_t offset = input_wires.at(i).first;
//    uint64_t length = input_wires.at(i).second + 1 - offset;
//
//    uint64_t counter = length - 1;
//    for (size_t j = 0; j < length / 8; j++) {
//      infile.read((char *) buffer, 1);
//      for (size_t k = 0; k < 8; k++) {
//        size_t shift = 7 - k;
//        input[counter] = ((buffer[0] & (1 << shift)) > 0);
//        counter -= 1;
//      }
//    }
//
//    if (length % 8 > 0) {
//      infile.read((char *) buffer, 1);
//      for (size_t k = 0; k < length % 8; k++) {
//        size_t shift = 7 - k;
//        input[counter] = ((buffer[0] & (1 << shift)) > 0);
//        counter -= 1;
//      }
//    }
//  }
//  infile.close();
//}





int party, port;
void bench_once(NetIOMP<nP> * ios[2], ThreadPool * pool, string filename) {
    if(party == 1)cout <<"CIRCUIT:\t"<<filename<<endl;
    //string file = circuit_file_location+"/"+filename;
    BristolFormat cf(filename.c_str());

    auto start = clock_start();
    CMPC<nP>* mpc = new CMPC<nP>(ios, pool, party, &cf);
    ios[0]->flush();
    ios[1]->flush();
    double t2 = time_from(start);
//  ios[0]->sync();
//  ios[1]->sync();
    if(party == 1)cout <<"Setup:\t"<<party<<"\t"<< t2 <<"\n"<<flush;

    start = clock_start();
    mpc->function_independent();
    ios[0]->flush();
    ios[1]->flush();
    t2 = time_from(start);
    if(party == 1)cout <<"FUNC_IND:\t"<<party<<"\t"<<t2<<" \n"<<flush;

    start = clock_start();
    mpc->function_dependent();
    ios[0]->flush();
    ios[1]->flush();
    t2 = time_from(start);
    if(party == 1)cout <<"FUNC_DEP:\t"<<party<<"\t"<<t2<<" \n"<<flush;

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
        int globalMinID = 0;
        int globalMinCost = INT_MAX;
        int secondPrice = INT_MAX;
        int numAttributes = 2;
        int garbledZero = 0;
        int garbledOne = 1;
        int totalCost = 0;
        int numAttributesSatisfied = 0;

        ADD_INT_TO_BOOLARR(globalMinID, i, in);
        ADD_INT_TO_BOOLARR(globalMinCost, i, in);
        ADD_INT_TO_BOOLARR(secondPrice, i, in);
        ADD_INT_TO_BOOLARR(numAttributes, i, in);
        ADD_INT_TO_BOOLARR(garbledZero, i, in);
        ADD_INT_TO_BOOLARR(garbledOne, i, in);
        ADD_INT_TO_BOOLARR(totalCost, i, in);
        ADD_INT_TO_BOOLARR(numAttributesSatisfied, i, in);

        // Now add "looking for" attributes
        //for (int a = 0; a < ALICE_NUM_ATTRIBUTES; a++) {
        //    for (int e = 0; e < INTS_PER_ATTRIBUTE; e++) {
        //        ADD_INT_TO_BOOLARR(1, i, in);
        //    }
        //}
        ADD_INT_TO_BOOLARR(1, i, in);
        ADD_INT_TO_BOOLARR(0, i, in);
        ADD_INT_TO_BOOLARR(1, i, in); // 1 CPU

        ADD_INT_TO_BOOLARR(2, i, in);
        ADD_INT_TO_BOOLARR(0, i, in);
        ADD_INT_TO_BOOLARR(1, i, in); // 1 RAM
        assert(i == partyEnd[party]);

        //cout << "p1 - first number= " << std::bitset<32>(numAttributes) << endl;
        //for (int i = partyStart[party]+32; i < partyStart[party] + 64; i++) {
        //    cout << "p1 - in[" << i << "] = " << in[i] << std::endl;
        //}

    } else {
        int i = partyStart[party];

        // Now add resources and attributes
        //for (int a = 0; a < BOB_NUM_ATTRIBUTES; a++) {
        //    ADD_INT_TO_BOOLARR(ID, i, in);
        //    for (int e = 0; e < INTS_PER_ATTRIBUTE; e++) {
        //        ADD_INT_TO_BOOLARR(1, i, in);
        //    }
        //}
        ADD_INT_TO_BOOLARR(67+party, i, in); //id

        ADD_INT_TO_BOOLARR(1, i, in);
        ADD_INT_TO_BOOLARR(1+party, i, in);
        ADD_INT_TO_BOOLARR(1, i, in); // 1 RAM for $3/4

        ADD_INT_TO_BOOLARR(2, i, in);
        ADD_INT_TO_BOOLARR(2+party, i, in);
        ADD_INT_TO_BOOLARR(1, i, in); // 1 CPU for $2/3

        ADD_INT_TO_BOOLARR(3, i, in);
        ADD_INT_TO_BOOLARR(3+party, i, in);
        ADD_INT_TO_BOOLARR(1, i, in); // 1 GPU for $3/4

        assert(i == partyEnd[party]);
    }


    start = clock_start();
    mpc->online(in, out, partyStart, partyEnd, true);
    ios[0]->flush();
    ios[1]->flush();
    t2 = time_from(start);
    //uint64_t band2 = io.count();
    //if(party == 1)cout <<"bandwidth\t"<<party<<"\t"<<band2<<endl;

    if(party == 1)cout <<"ONLINE:\t"<<party<<"\t"<<t2<<" \n"<<flush;

    int i=0;
    int globalMinID=0;
    int secondPrice=0;
    GET_INT_FROM_BOOLARR(globalMinID, i, out);
    GET_INT_FROM_BOOLARR(secondPrice, i, out);
    assert(i==cf.n3);

    cout << "globalMinID = " << globalMinID << std::endl;
    cout << "secondPrice = " << secondPrice << std::endl;


    delete[] in;
    delete[] out;
    delete[] partyStart;
    delete[] partyEnd;
    delete mpc;
}

int main(int argc, char** argv) {
    parse_party_and_port(argv, &party, &port);
    if(party > nP)return 0;
    NetIOMP<nP> io(party, port);
#ifdef LOCALHOST
    NetIOMP<nP> io2(party, port+2*(nP+1)*(nP+1)+1);
#else
    NetIOMP<nP> io2(party, port+2*(nP+1));
#endif
    NetIOMP<nP> *ios[2] = {&io, &io2};
    ThreadPool pool(2*(nP-1)+2);

    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    const char *path;
    if (count != -1) {
        path = dirname(result);
    } else {
        error("cant find my own path\n");
        return 1;
    }
    std::string circuitPath = string(path) + "/agmpc_multiatt_circuit.txt";
    bench_once(ios, &pool, circuitPath);
    //bench_once(ios, &pool, "noop_circuit.txt");
    //bench_once(ios, &pool, "./emp-tool/emp-tool/circuits/files/sha-1.txt");
    return 0;
}
