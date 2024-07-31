# trivial.c

This program returns 1.
It is obfuscated with non-deterministic control flows that maintain the same semantics as the original program.

```c
int main(int argc, char **argv)
{
  int nondeterm = atoi(argv[1]);

  if (nondeterm > 5)
  {
    return 1;
  }
  else
  {
    return 1;
  }
}

//
// Below is the deobfuscated ground-truth version of trivial.c:
//
int main(int argc, char **argv)
{
  return 1;
}
```

## Experimental Results with Yadegari et al.

We generated the trace from the obfuscated program using Intel PIN and exported it to `trivial.trace`.
The integer input is `1`.

### Output Analysis

Yadegari's tool does not eliminate the `cmp` instruction (introduced by obfuscation) on line 17 in the simplified trace file `trivial.simplified`.

```asm
// these instructions should be removed
FT+c:      589: [0x08048432] mov [0xbf861dfc], eax                                                                      EAX:00000001 ECX:00000000 EDX:00000000 EBX:00000000 ESP:bf861df0 EBP:bf861e08 ESI:b5058000 EDI:b5058000  R: W: 0xbf861dfc--0xbf861dff; 
FT+c:      590: [0x08048435] cmp dword [0xbf861dfc], 0x5                                                                EAX:00000001 ECX:00000000 EDX:00000000 EBX:00000000 ESP:bf861df0 EBP:bf861e08 ESI:b5058000 EDI:b5058000  R: 0xbf861dfc--0xbf861dff; W: 
```

### Incorrect Removal of Return Value Update

In its objdump, the program writes the return value to the `%eax` register before returning.
Consider the simplified trace, where the following two instructions are the last ones in the main function.
The first `jle` instruction conditionally jumps if `nondeterm <= 5` and should be followed by returning.
However, instead of loading 0x1 to `%eax`, the subsequent instruction resets the stack pointer with the return value placed in the `%eax` register.
Notably, the loading/moving instruction is missing, and `%eax` is incorrectly updated with the return value without any write operation to `%eax`.

```
// Incorrect simplified trace output
FT     591: [0x08048439] jle 0x8048442                                                                              EAX:00000001 ECX:00000000 EDX:00000000 EBX:00000000 ESP:bf861df0 EBP:bf861e08 ESI:b5058000 EDI:b5058000  R: W: 
+d:      596: [0x0804844e] add esp, 0x4                                                                               EAX:00000001 ECX:bf861e20 EDX:00000000 EBX:00000000 ESP:bf861e1c EBP:00000000 ESI:b5058000 EDI:b5058000  R: W: 

// Ground truth trace
// 8048439:	7e 07                	jle    8048442 <main+0x37>
 804843b:	b8 01 00 00 00       	mov    $0x1,%eax
```
