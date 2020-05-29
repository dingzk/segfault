#### 自定义segv handler和添加backtrace()

> ./a.out 2>&1 | grep -o '0x[0-9a-z].*'|addr2line -Cfpi -e a.out

> 加-g, 情况就会好更多了，显示源文件名和行号对大型程序，也许用-g不太合适，文件太大加-rdynamic

```c++
[root@08829897f7f3 core_dump]# g++ sigfault.c 
[root@08829897f7f3 core_dump]# ./a.out 
Error: signal 11:
./a.out[0x400989]
/lib64/libc.so.6(+0x363b0)[0x7fcd4b8063b0]
./a.out[0x400b16]
./a.out[0x400b00]
./a.out[0x400ae6]
./a.out[0x400acc]
./a.out[0x400ab2]
./a.out[0x400a98]
./a.out[0x400a04]
/lib64/libc.so.6(__libc_start_main+0xf5)[0x7fcd4b7f2505]

[root@08829897f7f3 core_dump]# ./a.out 2>&1 | grep -o '0x[0-9a-z].*'|addr2line -Cfpi -e a.out
handler(int) at ??:?
?? ??:0
foo::crash() at ??:?
foo::foo4() at ??:?
foo::foo3() at ??:?
foo::foo2() at ??:?
foo::foo1() at ??:?
foo::foo() at ??:?
main at ??:?
?? ??:0

```

### dmesg + objdump | nm | ldd

```c++
[root@08829897f7f3 core_dump]# g++ sigfault_nohandler.c
[root@08829897f7f3 core_dump]# ./a.out
Segmentation fault (core dumped)
[root@08829897f7f3 core_dump]# dmesg |tail
[1140705.060706] a.out[3286]: segfault at 0 ip 0000000000400ab8 sp 00007ffcdbcefcc0 error 6 in a.out[400000+1000]

[root@08829897f7f3 core_dump]# objdump -d a.out |grep -in -A 10 -B 10 "400ab8"
308-  400aa2:	c9                   	leaveq
309-  400aa3:	c3                   	retq
310-
311-0000000000400aa4 <_ZN3foo5crashEv>:
312-  400aa4:	55                   	push   %rbp
313-  400aa5:	48 89 e5             	mov    %rsp,%rbp
314-  400aa8:	48 89 7d e8          	mov    %rdi,-0x18(%rbp)
315-  400aac:	48 c7 45 f8 00 00 00 	movq   $0x0,-0x8(%rbp)
316-  400ab3:	00
317-  400ab4:	48 8b 45 f8          	mov    -0x8(%rbp),%rax
318:  400ab8:	c6 00 00             	movb   $0x0,(%rax)
319-  400abb:	5d                   	pop    %rbp
320-  400abc:	c3                   	retq
321-  400abd:	0f 1f 00             	nopl   (%rax)
322-
323-0000000000400ac0 <__libc_csu_init>:
324-  400ac0:	41 57                	push   %r15
325-  400ac2:	41 89 ff             	mov    %edi,%r15d
326-  400ac5:	41 56                	push   %r14
327-  400ac7:	49 89 f6             	mov    %rsi,%r14
328-  400aca:	41 55                	push   %r13


```

###  LD_PRELOAD方式,同理加-rdynamic会生成函数名 

```c++

[root@08829897f7f3 core_dump]# locate libSegFault.so
/usr/lib64/libSegFault.so
[root@08829897f7f3 core_dump]# LD_PRELOAD=/usr/lib64/libSegFault.so ./a.out
*** Segmentation fault

Backtrace:
./a.out[0x400ab8]
./a.out[0x400aa2]
./a.out[0x400a88]
./a.out[0x400a6e]
./a.out[0x400a54]
./a.out[0x400a3a]
./a.out[0x4009a5]
/lib64/libc.so.6(__libc_start_main+0xf5)[0x7f15b0e8f505]
./a.out[0x400859]

[root@08829897f7f3 core_dump]# LD_PRELOAD=/usr/lib64/libSegFault.so ./a.out 2>&1 |grep -A 10 'Backtrace' | c++filt
Backtrace:
./a.out[0x400ab8]
./a.out[0x400aa2]
./a.out[0x400a88]
./a.out[0x400a6e]
./a.out[0x400a54]
./a.out[0x400a3a]
./a.out[0x4009a5]
/lib64/libc.so.6(__libc_start_main+0xf5)[0x7fd18dc58505]
./a.out[0x400859]

[root@08829897f7f3 core_dump]# g++ sigfault_nohandler.c -rdynamic
[root@08829897f7f3 core_dump]# LD_PRELOAD=/usr/lib64/libSegFault.so ./a.out 2>&1 |grep -A 10 'Backtrace'
Backtrace:
./a.out(_ZN3foo5crashEv+0x14)[0x400e18]
./a.out(_ZN3foo4foo4Ev+0x18)[0x400e02]
./a.out(_ZN3foo4foo3Ev+0x18)[0x400de8]
./a.out(_ZN3foo4foo2Ev+0x18)[0x400dce]
./a.out(_ZN3foo4foo1Ev+0x18)[0x400db4]
./a.out(_ZN3fooC1Ev+0x18)[0x400d9a]
./a.out(main+0x27)[0x400d05]
/lib64/libc.so.6(__libc_start_main+0xf5)[0x7f9090e5b505]
./a.out[0x400bb9]

[root@08829897f7f3 core_dump]# LD_PRELOAD=/usr/lib64/libSegFault.so ./a.out 2>&1 |grep -A 10 'Backtrace' | c++filt
Backtrace:
./a.out(foo::crash()+0x14)[0x400e18]
./a.out(foo::foo4()+0x18)[0x400e02]
./a.out(foo::foo3()+0x18)[0x400de8]
./a.out(foo::foo2()+0x18)[0x400dce]
./a.out(foo::foo1()+0x18)[0x400db4]
./a.out(foo::foo()+0x18)[0x400d9a]
./a.out(main+0x27)[0x400d05]
/lib64/libc.so.6(__libc_start_main+0xf5)[0x7fb37bc2f505]
./a.out[0x400bb9]


```

### ref
- https://stackoverflow.com/questions/77005/how-to-automatically-generate-a-stacktrace-when-my-program-crashes
- https://zhuanlan.zhihu.com/p/37571803
- https://www.boost.org/doc/libs/1_66_0/doc/html/stacktrace/getting_started.html#stacktrace.getting_started.handle_terminates_aborts_and_seg
- http://www.docin.com/p-105923877.html
- https://en.wikipedia.org/wiki/Segmentation_fault
