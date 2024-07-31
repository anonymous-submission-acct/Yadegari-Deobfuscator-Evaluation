# double_loop_deadcode.c

This program computes twice the value of an input integer provided as a command-line argument.
It is obfuscated with deadcode insertion. Specifically, the condition `!(i < *ptr)` is always
false: `ptr` points to the variable `nondeterm`, so `*ptr` is equal to `nondeterm`.
When `!(i < *ptr)` is evaluated inside the loop body, we have `i < nondeterm`, thus `!(i < *ptr)`
is always false. As a result, the true branch is not reachable.

```c
int main(int argc, char **argv)
{
  int nondeterm = atoi(argv[1]);
  int output = 0;

  int *ptr = &nondeterm;

  for (int i = 0; i < nondeterm; i++)
  {
    if (!(i < *ptr))
    {
      output += 100;
    }
    else
    {
      output += 2;
    }
  }

  return output;
}

//
// Below is the deobfuscated ground-truth version of double_loop_deadcode.c:
//
int main(int argc, char **argv)
{
  int nondeterm = atoi(argv[1]);
  int output = 0;

  for (int i = 0; i < nondeterm; i++)
  {
    output += 2;
  }

  return output;
}
```

## Experimental Results with Yadegari et al.

We generated a trace from the obfuscated program using Intel PIN and exported it to `double_loop_deadcode.trace`.
Given an integer input of 4, the expected output is 8.
We then ran Yadegari's tool on the trace and obtained the simplified trace in `double_loop_deadcode.simplified`.


### Yadegari's tool does not remove obfuscation-introduced instructions

Yadegari's tool does not eliminate mov and cmp instructions introduced by obfuscation
on lines 20, 21, 26, 27, 32, and 33 in the simplified trace. These instructions load the value
of `*ptr` and compare it with `i`. The tool should have removed these instructions since the
condition `!(i < *ptr)` is always false.

```asm
// line 20-21: mov *ptr to %eax, and compare to i
FTCF +c:      604: [0x080484bf] mov eax, [0xbfcd697c]                                                                      EAX:bfcd697c ECX:00000000 EDX:00000000 EBX:00000000 ESP:bfcd6960 EBP:bfcd6998 ESI:b4f8d000 EDI:b4f8d000  R: 0xbfcd697c--0xbfcd697f; W: 
FTCF +c: +m:      605: [0x080484c1] cmp eax, [ebp-0x14]                                                                        EAX:00000004 ECX:00000000 EDX:00000000 EBX:00000000 ESP:bfcd6960 EBP:bfcd6998 ESI:b4f8d000 EDI:b4f8d000  R: W: 
...
// line 26-27: mov *ptr to %eax, and compare to i
FTCF +c:      613: [0x080484bf] mov eax, [0xbfcd697c]                                                                      EAX:bfcd697c ECX:00000000 EDX:00000000 EBX:00000000 ESP:bfcd6960 EBP:bfcd6998 ESI:b4f8d000 EDI:b4f8d000  R: 0xbfcd697c--0xbfcd697f; W: 
FTCF +c: +m:      614: [0x080484c1] cmp eax, [ebp-0x14]                                                                        EAX:00000004 ECX:00000000 EDX:00000000 EBX:00000000 ESP:bfcd6960 EBP:bfcd6998 ESI:b4f8d000 EDI:b4f8d000  R: W: 
...
```


### Yadegari's tool incorrectly removes updates to `output` and `i`

The tool completely gets rid of the increment of the loop counter (`i++`).
It also removes the addition of 2 to the `output` variable in the loop body.

These changes alter the semantics of the input program, making it difficult
to recover a semantically equivalent program from the simplified trace.


In the loop body, the input program iteratively executes `output += 2` and `i++`,
which can be repeatedly seen in the extracted trace output:
```asm
// output += 2;
-1	26889	unknown             	080484cc	                                        	83 45 e8 02                                       	add dword ptr [ebp-0x18], 0x2                     		EAX=00000004, ECX=00000000, EDX=00000000, EBX=00000000, ESP=bfcd6960, EBP=bfcd6998, ESI=b4f8d000, EDI=b4f8d000

// i++;
-1	26889	unknown             	080484d0	                                        	83 45 ec 01                                       	add dword ptr [ebp-0x14], 0x1                     		EAX=00000004, ECX=00000000, EDX=00000000, EBX=00000000, ESP=bfcd6960, EBP=bfcd6998, ESI=b4f8d000, EDI=b4f8d000
```

However, these instructions are incorrectly removed by the tool from the simplified trace:

```asm
//
// input: extracted trace
//
-1	26889	unknown             	080484c4	                                        	7f 06                                             	jnle 0x80484cc                                    		EAX=00000004, ECX=00000000, EDX=00000000, EBX=00000000, ESP=bfcd6960, EBP=bfcd6998, ESI=b4f8d000, EDI=b4f8d000
// output += 2;
-1	26889	unknown             	080484cc	                                        	83 45 e8 02                                       	add dword ptr [ebp-0x18], 0x2                     		EAX=00000004, ECX=00000000, EDX=00000000, EBX=00000000, ESP=bfcd6960, EBP=bfcd6998, ESI=b4f8d000, EDI=b4f8d000
// i++;
-1	26889	unknown             	080484d0	                                        	83 45 ec 01                                       	add dword ptr [ebp-0x14], 0x1                     		EAX=00000004, ECX=00000000, EDX=00000000, EBX=00000000, ESP=bfcd6960, EBP=bfcd6998, ESI=b4f8d000, EDI=b4f8d000// output: simplified trace

//
// output: simplified trace
//
// the first instruction is the first instruction in the trace above
// note that their address is the same (0x080484c4)
FTCF      615: [0x080484c4] jg 0x80484cc                                                                               EAX:00000004 ECX:00000000 EDX:00000000 EBX:00000000 ESP:bfcd6960 EBP:bfcd6998 ESI:b4f8d000 EDI:b4f8d000  R: W: 
// (deadcode) loads the value of *ptr into %eax and compares it with i
FT+c:      618: [0x080484d4] mov eax, [0xbfcd697c]                                                                      EAX:00000004 ECX:00000000 EDX:00000000 EBX:00000000 ESP:bfcd6960 EBP:bfcd6998 ESI:b4f8d000 EDI:b4f8d000  R: 0xbfcd697c--0xbfcd697f; W: 
FT+c: +m:      619: [0x080484d7] cmp [ebp-0x14], eax                                                                        EAX:00000004 ECX:00000000 EDX:00000000 EBX:00000000 ESP:bfcd6960 EBP:bfcd6998 ESI:b4f8d000 EDI:b4f8d000  R: W: 
FT     620: [0x080484da] jl 0x80484bc                                                                               EAX:00000004 ECX:00000000 EDX:00000000 EBX:00000000 ESP:bfcd6960 EBP:bfcd6998 ESI:b4f8d000 EDI:b4f8d000  R: W: 
FTCF +c:      622: [0x080484bf] mov eax, [0xbfcd697c]                                                                      EAX:bfcd697c ECX:00000000 EDX:00000000 EBX:00000000 ESP:bfcd6960 EBP:bfcd6998 ESI:b4f8d000 EDI:b4f8d000  R: 0xbfcd697c--0xbfcd697f; W: 
FTCF +c: +m:      623: [0x080484c1] cmp eax, [ebp-0x14]                                                                        EAX:00000004 ECX:00000000 EDX:00000000 EBX:00000000 ESP:bfcd6960 EBP:bfcd6998 ESI:b4f8d000 EDI:b4f8d000  R: W: 
// there is no update to output and i in the simplified trace
```

### Yadegari's tool incorrectly removes return value loading

Our program should load the final value of output into the return register `%eax`
before returning from the `main` function. This can be seen in the extracted input
trace:

```
// input trace
-1	26889	unknown             	080484da	                                        	7c e0                                             	jl 0x80484bc                                      		EAX=00000004, ECX=00000000, EDX=00000000, EBX=00000000, ESP=bfcd6960, EBP=bfcd6998, ESI=b4f8d000, EDI=b4f8d000
// move the final value of output into %eax
-1	26889	unknown             	080484dc	                                        	8b 45 e8                                          	mov eax, dword ptr [ebp-0x18]                     		EAX=00000004, ECX=00000000, EDX=00000000, EBX=00000000, ESP=bfcd6960, EBP=bfcd6998, ESI=b4f8d000, EDI=b4f8d000
```

However, these instructions are also incorrectly removed by the tool from the simplified trace:

```
FT     638: [0x080484da] jl 0x804 84bc                                                                               EAX:00000004 ECX:00000000 EDX:00000000 EBX:00000000 ESP:bfcd6960 EBP:bfcd6998 ESI:b4f8d000 EDI:b4f8d000  R: W: 
// no mov instruction to load the final value of output into %eax
// however, the value of EAX is changed to 8, which means the simplified trace is incorrect
+d:      646: [0x080484f7] add esp, 0x4                                                                               EAX:00000008 ECX:bfcd69b0 EDX:00000000 EBX:00000000 ESP:bfcd69ac EBP:00000000 ESI:b4f8d000 EDI:b4f8d000  R: W: 
```
