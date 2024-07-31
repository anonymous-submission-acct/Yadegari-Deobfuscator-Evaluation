# double_loop_flattened.c

This program computes twice the value of an input integer provided as a command-line argument.
It is obfuscated with control-flow flattening.

```c
#include <stdlib.h>

int main(int argc, char **argv)
{
  int c = 0;
  int nondeterm = 0, output = 0, i = 0;

  while (1)
  {
    if (c == 0)
    {
      nondeterm = atoi(argv[1]);
      c = 1;
    }
    else if (c == 1)
    {
      output = 0;
      c = 2;
    }
    else if (c == 2)
    {
      i = 0;
      c = 3;
    }
    else if (c == 3)
    {
      if (i < nondeterm)
      {
        c = 4;
      }
      else
      {
        c = 7;
      }
    }
    else if (c == 7)
    {
      return output;
    }
    else if (c == 4)
    {
      output += 2;
      c = 5;
    }
    else if (c == 5)
    {
      i++;
      c = 6;
    }
    else if (c == 6)
    {
      c = 3;
    }
  }

  return -1;
}

//
// Below is the deobfuscated ground-truth version of double_loop_flattened.c:
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

We generated a trace from the obfuscated program using Intel PIN and exported it to `double_loop_flattened.trace`.
Given an integer input of 4, the expected output is 8.
We then ran Yadegari's tool on the trace and obtained the simplified trace in `double_loop_flattened.simplified`.

### Yadegari's tool incorrectly removes updates to `output` and `i`

The tool removes the addition of 2 to the `output` variable in the loop body.
It also changes the increment of the loop counter (`i++`) into loading constants to `%eax`.

In the loop body, the input program iteratively executes `output += 2` and `i++`,
which can be repeatedly seen in the extracted trace output:
```asm
// output += 2;
-1	26897	unknown             	080484be	                                        	83 45 f0 02                                       	add dword ptr [ebp-0x10], 0x2                     		EAX=00000001, ECX=00000000, EDX=00000000, EBX=bf9041f0, ESP=bf9041c0, EBP=bf9041d8, ESI=b4f8f000, EDI=b4f8f000

// i++;
-1	26897	unknown             	080484d4	                                        	83 45 f4 01                                       	add dword ptr [ebp-0xc], 0x1                      		EAX=00000001, ECX=00000000, EDX=00000000, EBX=bf9041f0, ESP=bf9041c0, EBP=bf9041d8, ESI=b4f8f000, EDI=b4f8f000
```

However, these instructions are incorrectly removed by the tool from the simplified trace:

```asm
//
// input: extracted trace
//
// output += 2;
-1	26897	unknown             	080484be	                                        	83 45 f0 02                                       	add dword ptr [ebp-0x10], 0x2                     		EAX=00000001, ECX=00000000, EDX=00000000, EBX=bf9041f0, ESP=bf9041c0, EBP=bf9041d8, ESI=b4f8f000, EDI=b4f8f000
...
// i++;
-1	26897	unknown             	080484d4	                                        	83 45 f4 01                                       	add dword ptr [ebp-0xc], 0x1                      		EAX=00000001, ECX=00000000, EDX=00000000, EBX=bf9041f0, ESP=bf9041c0, EBP=bf9041d8, ESI=b4f8f000, EDI=b4f8f000
...

//
// output: simplified trace
//
+c:      623: [0x08048493] mov eax, 0x0                                                                               EAX:00000004 ECX:00000000 EDX:00000000 EBX:bf9041f0 ESP:bf9041c0 EBP:bf9041d8 ESI:b4f8f000 EDI:b4f8f000  R: W: 
FT+c:      624: [0x08048496] cmp eax, [0xbf9041c4]                                                                      EAX:00000000 ECX:00000000 EDX:00000000 EBX:bf9041f0 ESP:bf9041c0 EBP:bf9041d8 ESI:b4f8f000 EDI:b4f8f000  R: 0xbf9041c4--0xbf9041c7; W: 
FT     625: [0x08048499] jge 0x80484a4                                                                              EAX:00000000 ECX:00000000 EDX:00000000 EBX:bf9041f0 ESP:bf9041c0 EBP:bf9041d8 ESI:b4f8f000 EDI:b4f8f000  R: W: 
+c:      686: [0x08048493] mov eax, 0x1                                                                               EAX:00000000 ECX:00000000 EDX:00000000 EBX:bf9041f0 ESP:bf9041c0 EBP:bf9041d8 ESI:b4f8f000 EDI:b4f8f000  R: W: 
FT+c:      687: [0x08048496] cmp eax, [0xbf9041c4]                                                                      EAX:00000001 ECX:00000000 EDX:00000000 EBX:bf9041f0 ESP:bf9041c0 EBP:bf9041d8 ESI:b4f8f000 EDI:b4f8f000  R: 0xbf9041c4--0xbf9041c7; W: 
FT     688: [0x08048499] jge 0x80484a4                                                                              EAX:00000001 ECX:00000000 EDX:00000000 EBX:bf9041f0 ESP:bf9041c0 EBP:bf9041d8 ESI:b4f8f000 EDI:b4f8f000  R: W: 
+c:      749: [0x08048493] mov eax, 0x2                                                                               EAX:00000001 ECX:00000000 EDX:00000000 EBX:bf9041f0 ESP:bf9041c0 EBP:bf9041d8 ESI:b4f8f000 EDI:b4f8f000  R: W: 
FT+c:      750: [0x08048496] cmp eax, [0xbf9041c4]                                                                      EAX:00000002 ECX:00000000 EDX:00000000 EBX:bf9041f0 ESP:bf9041c0 EBP:bf9041d8 ESI:b4f8f000 EDI:b4f8f000  R: 0xbf9041c4--0xbf9041c7; W: 
FT     751: [0x08048499] jge 0x80484a4                                                                              EAX:00000002 ECX:00000000 EDX:00000000 EBX:bf9041f0 ESP:bf9041c0 EBP:bf9041d8 ESI:b4f8f000 EDI:b4f8f000  R: W: 
+c:      812: [0x08048493] mov eax, 0x3                                                                               EAX:00000002 ECX:00000000 EDX:00000000 EBX:bf9041f0 ESP:bf9041c0 EBP:bf9041d8 ESI:b4f8f000 EDI:b4f8f000  R: W: 
FT+c:      813: [0x08048496] cmp eax, [0xbf9041c4]                                                                      EAX:00000003 ECX:00000000 EDX:00000000 EBX:bf9041f0 ESP:bf9041c0 EBP:bf9041d8 ESI:b4f8f000 EDI:b4f8f000  R: 0xbf9041c4--0xbf9041c7; W: 
FT     814: [0x08048499] jge 0x80484a4                                                                              EAX:00000003 ECX:00000000 EDX:00000000 EBX:bf9041f0 ESP:bf9041c0 EBP:bf9041d8 ESI:b4f8f000 EDI:b4f8f000  R: W: 
+c:      875: [0x08048493] mov eax, 0x4                                                                               EAX:00000003 ECX:00000000 EDX:00000000 EBX:bf9041f0 ESP:bf9041c0 EBP:bf9041d8 ESI:b4f8f000 EDI:b4f8f000  R: W: 
FT+c:      876: [0x08048496] cmp eax, [0xbf9041c4]                                                                      EAX:00000004 ECX:00000000 EDX:00000000 EBX:bf9041f0 ESP:bf9041c0 EBP:bf9041d8 ESI:b4f8f000 EDI:b4f8f000  R: 0xbf9041c4--0xbf9041c7; W: 
FT     877: [0x08048499] jge 0x80484a4
// there is no update to output in the simplified trace
// in fact, the simplified trace can be translated as the following code snippet:
// i = 1; end loop if i >= nondeterm;
// i = 2; end loop if i >= nondeterm;
// ...
// i = 4; end loop if i >= nondeterm;
// This is not equivalent to the original trace input.
```

These changes alter the semantics of the input program, making it difficult
to recover a semantically equivalent program from the simplified trace.


### Yadegari's tool incorrectly removes return value loading

Our program should load the final value of output into the return register `%eax`
before returning from the `main` function. This can be seen in the extracted input
trace:

```
// input trace
-1	26897	unknown             	080484b3	                                        	8b 45 f0                                          	mov eax, dword ptr [ebp-0x10]                     		EAX=00000004, ECX=00000000, EDX=00000000, EBX=bf9041f0, ESP=bf9041c0, EBP=bf9041d8, ESI=b4f8f000, EDI=b4f8f000
...
-1	26897	unknown             	08048503	                                        	c3                                                	ret                                               		EAX=00000008, ECX=bf9041f0, EDX=00000000, EBX=00000000, ESP=bf9041ec, EBP=00000000, ESI=b4f8f000, EDI=b4f8f000
```

However, these instructions are also incorrectly removed by the tool from the simplified trace:

```
FT     877: [0x08048499] jge 0x80484a4                                                                              EAX:00000004 ECX:00000000 EDX:00000000 EBX:bf9041f0 ESP:bf9041c0 EBP:bf9041d8 ESI:b4f8f000 EDI:b4f8f000  R: W: 
// no mov instruction to load thefinal value of output into %eax
// however, the value of EAX is changed to 8, which means the simplified trace is incorrect
+d:      897: [0x08048503] add esp, 0x4                                                                               EAX:00000008 ECX:bf9041f0 EDX:00000000 EBX:00000000 ESP:bf9041ec EBP:00000000 ESI:b4f8f000 EDI:b4f8f000  R: W: 
```

### Yadegari's tool removes obfuscation-introduced control-flow instructions

The simplified trace does not contain the obfuscation-introduced control-flow instructions.
However, as shown above, the simplified trace does not contain ground-truth instructions,
which are necessary to recover the original program.
