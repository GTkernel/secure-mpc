#include <emp-tool/emp-tool.h>
#include <emp-tool/circuits/integer.h>
#include <emp-tool/io/net_io_channel.h>
#include <emp-ag2pc/emp-ag2pc.h>
#include <emp-ag2pc/fpre.h>
#include <src/include.h>
#include <iostream>
#include <emp-ot/emp-ot.h>

using namespace emp;
using namespace std;

int main(int argc, char** argv) {
    NetIO* io;
    int p = atoi(argv[1]);

    if (p == 0) {
        //TIC(setup);
        std::cout << "ALICE starting netio" << "\n";
        io = new NetIO(nullptr, 44444);
        io->set_nodelay();
        std::cout << "ALICE netio done" << "\n";
        //setup_semi_honest(io, ALICE);
        std::cout << "ALICE setup done" << "\n";
        //TOC(setup);


        string file = "A2B23.txt";
        BristolFormat cf(file.c_str());
	    auto t1 = clock_start();
	    C2PC twopc(io, ALICE, &cf);
	    io->flush();
	    cout << "one time:\t"<<"ALICE"<<"\t" <<time_from(t1)<<endl;



        cout <<"herea\n";
        //cout << "one time:\t"<<"ALICE"<<"\t" <<time_from(t1)<<endl;
        /*
        t1 = clock_start();
        twopc.function_independent();
        io->flush();
        cout << "inde:\t"<<"ALICE"<<"\t"<<time_from(t1)<<endl;

        t1 = clock_start();
        twopc.function_dependent();
        io->flush();
        cout << "dep:\t"<<"ALICE"<<"\t"<<time_from(t1)<<endl;
        */
        auto insize = cf.n1+cf.n2;
        bool *in = new bool[insize];
        bool * out = new bool[cf.n3];
        memset(in, false, insize);
        memset(out, false, cf.n3);
        //t1 = clock_start();
        CLOCK(ONLINE);
        TIC(ONLINE);
        twopc.online(in, out);
        TOC(ONLINE);
        //cout << "online:\t"<<"ALICE"<<"\t"<<time_from(t1)<<endl;
        delete[] in;
        delete[] out;
    } else {
        std::cout << "BOB starting netio" << "\n";
        io = new NetIO("127.0.0.1", 44444);
        io->set_nodelay();
        std::cout << "BOB netio done" << "\n";
        //setup_semi_honest(io, BOB);
        std::cout << "BOB setup done" << "\n";

        string file = "A2B23.txt";
        BristolFormat cf(file.c_str());
	    auto t1 = clock_start();
	    C2PC twopc(io, BOB, &cf);
	    io->flush();
	    cout << "one time:\t"<<"BOB"<<"\t" <<time_from(t1)<<endl;
        /*
	    t1 = clock_start();
	    twopc.function_independent();
	    io->flush();
	    cout << "inde:\t"<<"BOB"<<"\t"<<time_from(t1)<<endl;

	    t1 = clock_start();
	    twopc.function_dependent();
	    io->flush();
	    cout << "dep:\t"<<"BOB"<<"\t"<<time_from(t1)<<endl;
        */
        auto insize = cf.n1+cf.n2;
	    bool *in = new bool[insize];
	    bool * out = new bool[cf.n3];
	    memset(in, false, insize);
	    memset(out, false, cf.n3);
        cout << "here?\n";
	    t1 = clock_start();
	    twopc.online(in, out);
	    cout << "online:\t"<<"BOB"<<"\t"<<time_from(t1)<<endl;
		string res = "";
		for(int i = 0; i < cf.n3; ++i)
			res += (out[i]?"1":"0");
		cout << res <<endl;
	    delete[] in;
	    delete[] out;
    }
}
