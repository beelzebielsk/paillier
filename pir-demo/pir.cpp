#include <unistd.h>
    // using fork;
    // using sleep;
#include <sys/types.h>
    // for pid_t type
#include <iostream>
    using std::cout;
    using std::endl;

int
main() {
    int identity = 0;
    int numForks = 3;
    pid_t pid;
    for (; identity < numForks; identity++) {
        pid = fork();
        if (pid == 0) break;
    }
    if (identity == 0 && !pid) {
        cout << "Executing client." << endl;
        execl("./pir_client", "./pir_client", (char*)NULL);
    } else if (identity == 1) {
        sleep(10);
        cout << "Executing 1st server." << endl;
        execl("./pir_server", "./pir_server", "0", (char*)NULL);
    } else if (identity == 2) {
        sleep(10);
        cout << "Executing 2nd server." << endl;
        execl("./pir_server", "./pir_server", "1", (char*)NULL);
    }
    return 0;
}
