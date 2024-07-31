# parity.c

This program computes the parity of an integer provided as a command-line argument.
It is obfuscated with non-deterministic control flows that maintain the same semantics as the original program.

```
int foobar(int in)
{
  return in * 2 + 3;
}

int main(int argc, char **argv)
{
  int nondeterm = atoi(argv[1]);

  if ((nondeterm << 2) > 10)
  {
    if (foobar(nondeterm) < 10)
    {
      return nondeterm % 2;
    }
    else
    {
      return nondeterm % 2;
    }
  }
  else
  {
    return nondeterm % 2;
  }
}


//
// Below is the deobfuscated ground-truth version of parity.c:
//
int main(int argc, char **argv)
{
  int nondeterm = atoi(argv[1]);
  return nondeterm % 2;
}
```

## Experimental Results with Yadegari et al.

We generated the trace from the obfuscated program using Intel PIN and exported it to `parity.trace`.
The integer input is `1`.

### Output Analysis

Yadegari's tool does not eliminate the `cmp` instruction (introduced by obfuscation) on line 19 in the simplified trace file `parity.simplified`.

```asm
// these instructions should be removed
FT     592: [0x08048448] cmp eax, 0xa                                                                               EAX:00000004 ECX:00000000 EDX:00000000 EBX:00000000 ESP:bff989e0 EBP:bff989f8 ESI:b4ff4000 EDI:b4ff4000  R: W: 
FT     593: [0x0804844b] jle 0x8048480     
```

### Incorrect Removal of Parity Check Computation

The input program computes the parity of the input integer in the last statement, specifically `return nondeterm % 2;`,
which expands to lines 203-209 in `parity.objdump`.
However, these instructions are incorrectly removed by the tool from the simplified parity trace.

In particular, in the simplified trace, the following two instructions are the last ones in the main function. The first `jle` instruction conditionally jumps if `(nondeterm << 2)` is less than `10` and should be followed by parity computation.
However, instead of computing parity, the subsequent instruction resets the stack pointer with the return value placed in the `%eax` register.
Notably, the parity computation is missing, and `%eax` is incorrectly updated with the return value without any write operation to `%eax`.

```
// Incorrect simplified trace output
FT     593: [0x0804844b] jle 0x8048480                                                                              EAX:00000004 ECX:00000000 EDX:00000000 EBX:00000000 ESP:bff989e0 EBP:bff989f8 ESI:b4ff4000 EDI:b4ff4000  R: W: 
+d:      603: [0x08048495] add esp, 0x4                                                                               EAX:00000001 ECX:bff98a10 EDX:00000000 EBX:00000000 ESP:bff98a0c EBP:00000000 ESI:b4ff4000 EDI:b4ff4000  R: W: 

// Ground truth trace
// 804844b:	7e 33                	jle    8048480 <main+0x68>
 8048480:	8b 45 f4             	mov    -0xc(%ebp),%eax
 8048483:	99                   	cltd   
 8048484:	c1 ea 1f             	shr    $0x1f,%edx
 8048487:	01 d0                	add    %edx,%eax
 8048489:	83 e0 01             	and    $0x1,%eax
 804848c:	29 d0                	sub    %edx,%eax
 804848e:	8b 4d fc             	mov    -0x4(%ebp),%ecx
 8048491:	c9                   	leave  
 8048492:	8d 61 fc             	lea    -0x4(%ecx),%esp
 8048495:	c3                   	ret    
```
