#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include <unwind.h>
#include <dlfcn.h>
#include <cxxabi.h>
//#include <ucontext.h>

//-fvisibility=hidden

#define BT_BUFFER_SIZE 64

char *getTimeString(void)
{
    time_t tm;
    time(&tm);
    return ctime(&tm);
}

#if 0
#include <execinfo.h>

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
#endif

struct BacktraceState
{
    void **current;
    void **end;
};

static _Unwind_Reason_Code unwindCallback(struct _Unwind_Context *context, void *arg)
{
    BacktraceState *state = static_cast<BacktraceState*>(arg);
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

size_t captureBacktrace(void **buffer, size_t max)
{
    BacktraceState state = {buffer, buffer + max};
    _Unwind_Backtrace(unwindCallback, &state);

    return state.current - buffer;
}

void dump_backtrace(std::ostream &os, void **buffer, size_t count)
{
    os << "=================================================================\n"
        << ">>> " << getTimeString() << "\n";

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

static void signal_handler(int signum, siginfo_t *sigInfo, void *ctx)
{
    /* Take appropriate actions for signal delivery */
    if (signum == SIGSEGV) {
        printf("signal SIGSEGV catching\n");

#if defined(__arm__)
        struct ucontext *uc = (struct ucontext *)ctx;
        unsigned long pc;
        pc = uc->uc_mcontext.arm_pc;
        printf("pc->: %#lx\n", pc);
        printf("lr->: %#lx\n", uc->uc_mcontext.arm_lr);
        printf("sp->: %#lx\n", uc->uc_mcontext.arm_sp);
        printf("ip->: %#lx\n", uc->uc_mcontext.arm_ip);
        printf("fp->: %#lx\n", uc->uc_mcontext.arm_fp);

        unsigned long *oldpc = (unsigned long *)uc->uc_mcontext.arm_fp;
        int i, max;
        max = (uc->uc_mcontext.arm_fp - uc->uc_mcontext.arm_sp) / sizeof(long);
        for (i = 0; i <= max; i++) {
            printf(" #%2d->: %#lx\n", i, *oldpc--);
        }

#elif defined(__aarch64__)
        struct ucontext *uc = (struct ucontext *)ctx;
        unsigned long pc;
        pc = uc->uc_mcontext.pc;
        printf("pc->: %#lx\n", pc);
#else
        void* buffer[BT_BUFFER_SIZE];
        std::ostringstream oss;
        dump_backtrace(oss, buffer, captureBacktrace(buffer, BT_BUFFER_SIZE));
        printf("%s", oss.str().c_str());
#endif

        printf("signal SIGSEGV handled\n");
        exit(EXIT_FAILURE);
    }
}

void signal_init()
{
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags     = SA_SIGINFO;
    sa.sa_sigaction = signal_handler;

    if (sigaction(SIGSEGV, &sa, NULL) == -1) {
        perror("sigaction");
    }
}

void f3() {
    *(int *)0 = 0;
    printf("%s->%d\n", __func__, __LINE__);
}

void f2() {
    f3();
    printf("%s->%d\n", __func__, __LINE__);
}

void f1() {
    f2();
    printf("%s->%d\n", __func__, __LINE__);
}

int main(int argc, char const *argv[])
{
#if defined(__linux__) || defined(__APPLE__)
    signal_init();
#endif

    f1();
    printf("%s->%d\n", __func__, __LINE__);

    return 0;
}
