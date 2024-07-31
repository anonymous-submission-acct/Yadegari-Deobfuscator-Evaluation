# double_loop_branch_insertion.c

This program computes twice the value of an input integer provided as a command-line argument.
It is obfuscated with non-deterministic control flows that maintain the same semantics as the original program.

```c
int main(int argc, char **argv)
{
  int nondeterm = atoi(argv[1]);
  int output = 0;

  for (int i = 0; i < nondeterm; i++)
  {
    if (nondeterm > 100)
    {
      output += 2;
    }
    else
    {
      output += 2;
    }
  }

  return output;
}

//
// Below is the deobfuscated ground-truth version of double_loop_branch_insertion.c:
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

We generated a trace from the obfuscated program using Intel PIN and exported it to `double_loop_branch_insertion.trace`.
Given an integer input of 4, the expected output is 8.
We then ran Yadegari's tool on the trace and obtained the simplified trace in `double_loop_branch_insertion.simplified`.

### Output Analysis

Firstly, Yadegari's tool does not eliminate the cmp instruction introduced by obfuscation on lines 21, 26, 31, and 36 in the simplified trace.

```asm
// these instructions should be removed
FTCF +c:      596: [0x08048445] cmp dword [0xbfc7e17c], 0x64                                                               EAX:00000000 ECX:00000000 EDX:00000000 EBX:00000000 ESP:bfc7e170 EBP:bfc7e188 ESI:b505c000 EDI:b505c000  R: 0xbfc7e17c--0xbfc7e17f; W: 
FTCF      597: [0x08048449] jle 0x8048451      
FTCF +c:      603: [0x08048445] cmp dword [0xbfc7e17c], 0x64                                                               EAX:00000001 ECX:00000000 EDX:00000000 EBX:00000000 ESP:bfc7e170 EBP:bfc7e188 ESI:b505c000 EDI:b505c000  R: 0xbfc7e17c--0xbfc7e17f; W: 
FTCF      604: [0x08048449] jle 0x8048451                                                                              EAX:00000001 ECX:00000000 EDX:00000000 EBX:00000000 ESP:bfc7e170 EBP:bfc7e188 ESI:b505c000 EDI:b505c000  R: W: 
...
```

Secondly, the tool changes the increment of the loop counter (`i++`) into loading constants to `%eax`.

```asm
+c:      593: [0x08048459] mov eax, 0x0  
// ...
+c:      600: [0x08048459] mov eax, 0x1
// ...
+c:      607: [0x08048459] mov eax, 0x2
// ...
+c:      614: [0x08048459] mov eax, 0x3
// ...
+c:      621: [0x08048459] mov eax, 0x4
```

These changes alter the semantics of the input program, making it difficult to recover a semantically equivalent program from the simplified trace.

### Incorrect Removal of Updates to `output`

In the loop body, the input program iteratively adds 2 to the local variable output, which can be repeatedly seen in the extracted trace output:
```asm
-1	25761	unknown             	08048451	                                        	83 45 ec 02                                       	add dword ptr [ebp-0x14], 0x2
```

However, these instructions are incorrectly removed by the tool from the simplified trace:

```asm
// input: extracted trace
-1	25761	unknown             	08048449	                                        	7e 06                                             	jle 0x8048451                                     		EAX=00000000, ECX=00000000, EDX=00000000, EBX=00000000, ESP=bfc7e170, EBP=bfc7e188, ESI=b505c000, EDI=b505c000
// outputs = outputs + 2
-1	25761	unknown             	08048451	                                        	83 45 ec 02                                       	add dword ptr [ebp-0x14], 0x2                     		EAX=00000000, ECX=00000000, EDX=00000000, EBX=00000000, ESP=bfc7e170, EBP=bfc7e188, ESI=b505c000, EDI=b505c000
// i++
-1	25761	unknown             	08048455	                                        	83 45 f0 01                                       	add dword ptr [ebp-0x10], 0x1                     		EAX=00000000, ECX=00000000, EDX=00000000, EBX=00000000, ESP=bfc7e170, EBP=bfc7e188, ESI=b505c000, EDI=b505c000

// output: simplified trace
FTCF      597: [0x08048449] jle 0x8048451                                                                              EAX:00000000 ECX:00000000 EDX:00000000 EBX:00000000 ESP:bfc7e170 EBP:bfc7e188 ESI:b505c000 EDI:b505c000  R: W: 
// missing update: outputs = outputs + 2
// i++ (incorrectly changed to loading constants to %eax)
+c:      600: [0x08048459] mov eax, 0x1                                                                               EAX:00000000 ECX:00000000 EDX:00000000 EBX:00000000 ESP:bfc7e170 EBP:bfc7e188 ESI:b505c000 EDI:b505c000  R: W: 
```

Finally, the program should load the final value of output into the return register `%eax` before returning from the `main` function. This can be seen in the extracted input trace:

```
// input trace
 804845f:	7c e4                	jl     8048445 <main+0x3a>
 8048461:	8b 45 ec             	mov    -0x14(%ebp),%eax
```

However, these instructions are also incorrectly removed by the tool from the simplified trace:

```
// Incorrect simplified trace output
FT     623: [0x0804845f] jl 0x8048445                                                                               EAX:00000004 ECX:00000000 EDX:00000000 EBX:00000000 ESP:bfc7e170 EBP:bfc7e188 ESI:b505c000 EDI:b505c000  R: W: 
// no mov instruction to load the final value of output into %eax
// however, the value of EAX is changed to 8, which means the trace is Incorrect
+d:      628: [0x0804846b] add esp, 0x4                                                                               EAX:00000008 ECX:bfc7e1a0 EDX:00000000 EBX:00000000 ESP:bfc7e19c EBP:00000000 ESI:b505c000 EDI:b505c000  R: W: 
```
