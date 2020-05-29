#include <signal.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//信号处理函数
void recvSignal(int sig)
{
    printf("received signal %d !!!\n",sig);
}

int main(int argc,char** argv)
{
  //给信号注册一个处理函数
  //signal(SIGSEGV, recvSignal);
    //int* s = 0;
    //(*s) = 1;

    char *ptr = "test";        
    strcpy(ptr, "TEST");


 //以上两句用来产生 一个 传说中的段错误
  while(1)
  {
    sleep(1);
    printf("sleep 1 \n");
  }
  return 0;
}
