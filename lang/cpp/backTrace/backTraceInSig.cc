#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include <execinfo.h>
#include <unwind.h>
#include <dlfcn.h>
#include <cxxabi.h>

//-fvisibility=hidden

#define BT_BUFFER_SIZE 64

void dump_backTrace1(void)
{
    int i, nptrs;
    void *buffer[BT_BUFFER_SIZE];
    char **strings;

    nptrs = backtrace(buffer, BT_BUFFER_SIZE);
    printf("backtrace() returned %d addresses\n", nptrs);

    /* The call backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO)
     *  would produce similar output to the following: */

    strings = backtrace_symbols(buffer, nptrs);
    if (strings == NULL) {
        perror("backtrace_symbols");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < nptrs; i++) {
        printf("%d:%s\n", i, strings[i]);
    }

    free(strings);
}

struct BacktraceState
{
    void** current;
    void** end;
};

static _Unwind_Reason_Code unwindCallback(struct _Unwind_Context* context, void* arg)
{
    BacktraceState* state = static_cast<BacktraceState*>(arg);
    uintptr_t pc = _Unwind_GetIP(context);
    if (pc) {
        if (state->current == state->end) {
            return _URC_END_OF_STACK;
        } else {
            *state->current++ = reinterpret_cast<void*>(pc);
        }
    }
    return _URC_NO_REASON;
}

size_t captureBacktrace(void** buffer, size_t max)
{
    BacktraceState state = {buffer, buffer + max};
    _Unwind_Backtrace(unwindCallback, &state);

    return state.current - buffer;
}

void dump_backtrace(std::ostream& os, void** buffer, size_t count)
{
    for (size_t idx = 0; idx < count; ++idx) {
        const void* addr = buffer[idx];
        const char* symbol = "";

        Dl_info info;
        if (dladdr(addr, &info) && info.dli_sname) {
            symbol = info.dli_sname;
        }

        os << "  #" << std::setw(2) << idx << ": " << addr << "  " << symbol << "\n";
    }
}

static void signal_handler(int signum)
{
    /* Take appropriate actions for signal delivery */
    if (signum == SIGSEGV) {
        printf("signal SIGSEGV catching\n");

        void* buffer[BT_BUFFER_SIZE];
        std::ostringstream oss;

        dump_backtrace(oss, buffer, captureBacktrace(buffer, BT_BUFFER_SIZE));

        printf("%s", oss.str().c_str());

        printf("signal SIGSEGV handled\n");
        exit(EXIT_FAILURE);
    }
}

void signal_init()
{
    struct sigaction sa;

    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGSEGV, &sa, NULL) == -1) {
        perror("sigaction");
    }
}

void f3()
{
    *(int *)0 = 0;
}

void f2()
{
    f3();
}

void f1()
{
    f2();
}

int main(int argc, char const *argv[])
{
    signal_init();

    f1();

    return 0;
}
