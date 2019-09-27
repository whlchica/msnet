#include "parseargs.h"
#include <cassert>
#include <iostream>
#include <stdio.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>
#include <vector>

int system_core_dump()
{
#define CORE_SIZE 1024 * 1024 * 500
    do {
        struct rlimit rlmt;
        if (getrlimit(RLIMIT_CORE, &rlmt) == -1) {
            break;
        }
        printf("Before set rlimit CORE dump current is: %d, max is: %d\n", ( int )rlmt.rlim_cur, ( int )rlmt.rlim_max);
        rlmt.rlim_cur = ( rlim_t )CORE_SIZE;
        rlmt.rlim_max = ( rlim_t )CORE_SIZE;
        if (setrlimit(RLIMIT_CORE, &rlmt) == -1 || getrlimit(RLIMIT_CORE, &rlmt) == -1) {
            break;
        }
        printf("After set rlimit CORE dump current is: %d, max is: %d\n", ( int )rlmt.rlim_cur, ( int )rlmt.rlim_max);
        return 0;
    } while (0);
#undef CORE_SIZE
    return -1;
}

int main(int argc, char* const argv[])
{
    parse_args(argc, argv);
    return 0;
}
