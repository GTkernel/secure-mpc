#include <emp-tool/emp-tool.h>
#include <emp-tool/execution/plain_prot.h>
#include <emp-tool/io/net_io_channel.h>
//#include <emp-sh2pc/emp-sh2pc.h>
#include <src/include.h>

#include <libgen.h>         // dirname
#include <unistd.h>         // readlink
#include <linux/limits.h>   // PATH_MAX

using namespace emp;

int main(int argc, char** argv) {
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    const char *path;
    if (count != -1) {
        path = dirname(result);
    } else {
        error("cant find path\n");
        return 1;
    }
    std::string circuitPath = string(path) + "/noop_circuit.txt";

    emp::setup_plain_prot(true, circuitPath);

    Integer a(32, 0, ALICE);
    Integer b(32, 0, BOB);
    Bit res = a>b;
    bool realres = res.reveal<bool>();

    (void)realres;

    emp::finalize_plain_prot();
}
