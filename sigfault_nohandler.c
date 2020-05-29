#include <iostream>
#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void handler(int sig) {
  void *array[10];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}

class foo
{
public:
    foo() { foo1(); }

private:
    void foo1() { foo2(); }
    void foo2() { foo3(); }
    void foo3() { foo4(); }
    void foo4() { crash(); }
    void crash() { char * p = NULL; *p = 0; }
};

int main(int argc, char ** argv)
{
    // Setup signal handler for SIGSEGV
    
    //signal(SIGSEGV, handler);
    
    foo * f = new foo();
    return 0;
}
