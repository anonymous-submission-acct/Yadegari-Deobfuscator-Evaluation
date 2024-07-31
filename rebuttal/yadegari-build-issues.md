# Building Issues with Yadegari et al's Tool

In this document, we describe the technical difficulties encountered during
our efforts to build and evaluate Yadegari et al's tool.

## Overview

We obtained the source code of Yadegari et al from the following URL:
https://www2.cs.arizona.edu/projects/lynx-project/Source/Deobfuscator.tar.gz

```shell
$ sha256sum Deobfuscator.tar.gz 
0defe735375fe4c38c86dff5f6dc0333c4019856f3a791379aeed6afb6ebf484  Deobfuscator.tar.gz
```

Despite initial linking failures, we successfully built the main executable
of Yadegari et al, `deobf`, in an Ubuntu 16.04 container. However, we soon
discovered that the trace extraction plugin (script), `InstructionTrace`,
provided by Yadegari et al had two major issues:

  1. The script outputs a trace format that mismatches the expected format in
  their main executable and lacks several pieces of information. We speculate
  that the script was updated after Yadegari et al published their source
  code but was not made publicly available.

  2. The script is specifically written to extract traces for a Windows
  NT executable and includes hacks for collecting traces on a 32-bit
  Windows machine. It does not support Linux executables.

Since we used an Ubuntu container for convenience and better reproducibility, we
had to reimplement the trace extraction script by understanding the expected
input format from the released source code of Yadegari et al's deobfuscator to
address both issues.

With the trace extraction script reimplemented and the main executable built,
we anticipated having Yadegari et al's tool ready for evaluation. However,
the tool crashed during taint analysis because of a segmentation fault. Upon
investigation, we found that an implicit type conversion breaks the procedure
that identifies the internal representation of memory pages used in taint
analysis. We suspected this runtime issue was also due to inconsistent compiler
behaviors on different platforms (namely, Windows and Linux). Ultimately, we
fixed the issue causing the segmentation fault and adapted Yadegari et al's tool
to run in an Ubuntu 16.04 container.

We intend to make both (1) our patch to Yadegari et al's tool and (2) the
reimplemented trace extractor publicly available to facilitate research
reproducibility.

## Building the Main Executable

We followed the instruction from `README.md` to build (1) `udis86` and (2) the
`deobf` binary. On an Ubuntu 20.04 and 22.04 container, we were able to build
`udis86`, but we encountered linker errors as shown below.

```
g++ -o ./deobf bin/al_helper.o bin/array_list.o bin/decode.o bin/dis.o bin/hash.o bin/instr.o bin/lynx_types.o bin/main.o bin/mem_maps.o bin/print.o bin/read.o bin/simplify.o bin/table.o bin/flowgraph.o bin/taint_analysis.o -ludis86 -pthread -Ludis86-1.7/libudis86/.libs 
/usr/bin/ld: bin/hash.o:/workspace/yadegari-tool/include/lynx_types.h:162: multiple definition of `module_names'; bin/al_helper.o:/workspace/yadegari-tool/include/lynx_types.h:162: first defined here
/usr/bin/ld: bin/hash.o:/workspace/yadegari-tool/include/lynx_types.h:163: multiple definition of `function_names'; bin/al_helper.o:/workspace/yadegari-tool/include/lynx_types.h:163: first defined here
/usr/bin/ld: bin/hash.o:/workspace/yadegari-tool/include/lynx_types.h:561: multiple definition of `handle_rop'; bin/al_helper.o:/workspace/yadegari-tool/include/lynx_types.h:561: first defined here
/usr/bin/ld: bin/instr.o:/workspace/yadegari-tool/include/lynx_types.h:162: multiple definition of `module_names'; bin/al_helper.o:/workspace/yadegari-tool/include/lynx_types.h:162: first defined here
/usr/bin/ld: bin/instr.o:/workspace/yadegari-tool/include/lynx_types.h:163: multiple definition of `function_names'; bin/al_helper.o:/workspace/yadegari-tool/include/lynx_types.h:163: first defined here
/usr/bin/ld: bin/instr.o:/workspace/yadegari-tool/include/lynx_types.h:561: multiple definition of `handle_rop'; bin/al_helper.o:/workspace/yadegari-tool/include/lynx_types.h:561: first defined here
/usr/bin/ld: bin/lynx_types.o:/workspace/yadegari-tool/include/lynx_types.h:162: multiple definition of `module_names'; bin/al_helper.o:/workspace/yadegari-tool/include/lynx_types.h:162: first defined here
/usr/bin/ld: bin/lynx_types.o:/workspace/yadegari-tool/include/lynx_types.h:163: multiple definition of `function_names'; bin/al_helper.o:/workspace/yadegari-tool/include/lynx_types.h:163: first defined here
/usr/bin/ld: bin/lynx_types.o:/workspace/yadegari-tool/include/lynx_types.h:561: multiple definition of `handle_rop'; bin/al_helper.o:/workspace/yadegari-tool/include/lynx_types.h:561: first defined here
/usr/bin/ld: bin/main.o:/workspace/yadegari-tool/include/lynx_types.h:162: multiple definition of `module_names'; bin/al_helper.o:/workspace/yadegari-tool/include/lynx_types.h:162: first defined here
/usr/bin/ld: bin/main.o:/workspace/yadegari-tool/include/lynx_types.h:163: multiple definition of `function_names'; bin/al_helper.o:/workspace/yadegari-tool/include/lynx_types.h:163: first defined here
/usr/bin/ld: bin/main.o:/workspace/yadegari-tool/include/lynx_types.h:561: multiple definition of `handle_rop'; bin/al_helper.o:/workspace/yadegari-tool/include/lynx_types.h:561: first defined here
/usr/bin/ld: bin/mem_maps.o:/workspace/yadegari-tool/include/lynx_types.h:162: multiple definition of `module_names'; bin/al_helper.o:/workspace/yadegari-tool/include/lynx_types.h:162: first defined here
/usr/bin/ld: bin/mem_maps.o:/workspace/yadegari-tool/include/lynx_types.h:163: multiple definition of `function_names'; bin/al_helper.o:/workspace/yadegari-tool/include/lynx_types.h:163: first defined here
/usr/bin/ld: bin/mem_maps.o:/workspace/yadegari-tool/include/lynx_types.h:561: multiple definition of `handle_rop'; bin/al_helper.o:/workspace/yadegari-tool/include/lynx_types.h:561: first defined here
/usr/bin/ld: bin/print.o:/workspace/yadegari-tool/include/lynx_types.h:162: multiple definition of `module_names'; bin/al_helper.o:/workspace/yadegari-tool/include/lynx_types.h:162: first defined here
/usr/bin/ld: bin/print.o:/workspace/yadegari-tool/include/lynx_types.h:163: multiple definition of `function_names'; bin/al_helper.o:/workspace/yadegari-tool/include/lynx_types.h:163: first defined here
/usr/bin/ld: bin/print.o:/workspace/yadegari-tool/include/lynx_types.h:561: multiple definition of `handle_rop'; bin/al_helper.o:/workspace/yadegari-tool/include/lynx_types.h:561: first defined here
/usr/bin/ld: bin/read.o:/workspace/yadegari-tool/include/lynx_types.h:162: multiple definition of `module_names'; bin/al_helper.o:/workspace/yadegari-tool/include/lynx_types.h:162: first defined here
/usr/bin/ld: bin/read.o:/workspace/yadegari-tool/include/lynx_types.h:163: multiple definition of `function_names'; bin/al_helper.o:/workspace/yadegari-tool/include/lynx_types.h:163: first defined here
/usr/bin/ld: bin/read.o:/workspace/yadegari-tool/include/lynx_types.h:561: multiple definition of `handle_rop'; bin/al_helper.o:/workspace/yadegari-tool/include/lynx_types.h:561: first defined here
/usr/bin/ld: bin/simplify.o:/workspace/yadegari-tool/include/lynx_types.h:162: multiple definition of `module_names'; bin/al_helper.o:/workspace/yadegari-tool/include/lynx_types.h:162: first defined here
/usr/bin/ld: bin/simplify.o:/workspace/yadegari-tool/include/lynx_types.h:163: multiple definition of `function_names'; bin/al_helper.o:/workspace/yadegari-tool/include/lynx_types.h:163: first defined here
/usr/bin/ld: bin/simplify.o:/workspace/yadegari-tool/include/lynx_types.h:561: multiple definition of `handle_rop'; bin/al_helper.o:/workspace/yadegari-tool/include/lynx_types.h:561: first defined here
/usr/bin/ld: bin/flowgraph.o:/workspace/yadegari-tool/include/lynx_types.h:162: multiple definition of `module_names'; bin/al_helper.o:/workspace/yadegari-tool/include/lynx_types.h:162: first defined here
/usr/bin/ld: bin/flowgraph.o:/workspace/yadegari-tool/include/lynx_types.h:163: multiple definition of `function_names'; bin/al_helper.o:/workspace/yadegari-tool/include/lynx_types.h:163: first defined here
/usr/bin/ld: bin/flowgraph.o:/workspace/yadegari-tool/include/lynx_types.h:561: multiple definition of `handle_rop'; bin/al_helper.o:/workspace/yadegari-tool/include/lynx_types.h:561: first defined here
/usr/bin/ld: bin/taint_analysis.o:/workspace/yadegari-tool/include/lynx_types.h:162: multiple definition of `module_names'; bin/al_helper.o:/workspace/yadegari-tool/include/lynx_types.h:162: first defined here
/usr/bin/ld: bin/taint_analysis.o:/workspace/yadegari-tool/include/lynx_types.h:163: multiple definition of `function_names'; bin/al_helper.o:/workspace/yadegari-tool/include/lynx_types.h:163: first defined here
/usr/bin/ld: bin/taint_analysis.o:/workspace/yadegari-tool/include/lynx_types.h:561: multiple definition of `handle_rop'; bin/al_helper.o:/workspace/yadegari-tool/include/lynx_types.h:561: first defined here
/usr/bin/ld: udis86-1.7/libudis86/.libs/libudis86.a(input.o): relocation R_X86_64_32S against `.text' can not be used when making a PIE object; recompile with -fPIE
/usr/bin/ld: failed to set dynamic section sizes: bad value
collect2: error: ld returned 1 exit status
make: *** [Makefile:53: deobf] Error 1
```

We conjectured that these issues are due to differences in language or compiler
standards. To avoid these issues, we aimed to find an Ubuntu version that was
released around the same time as the publication date of Yadegari et al.

**Solution.** We ended up using an Ubuntu 16.04 container with gcc/g++ 5.4.0,
and successfully built both `udis86` and the main executable, `deobf`.


## Reimplementing the InstructionTrace Plugin

InstructionTrace is a plugin intended to run on the Intel PIN tool that extracts
trace from a single run of an executable. Unfortunately, the URL included in
`README.md` is no longer available, and Intel no longer publishes older releases
of PIN.

Despite the difficulty obtaining older versions of PIN, we found a working version
(PIN 3.0) on GitHub: https://github.com/rohithmukku/Architecture-HW1

We also resolved some runtime errors by following a 2018 blog post about common
issues with compiling PIN plugins: https://chunkaichang.com/tool/pin-notes/

**Plugin Issues.** After resolving compilation issues, we found the plugin
would always crash with a segmentation fault. This runtime error stems from the
plugin's assumption that it is running on a 32-bit Windows system with the NT
executable format. Additionally, the plugin outputs a different format than what
`deobf` accepts.

**Solution.** We reimplemented the InstructionTrace plugin by comprehending the
expected input format from the released source code of Yadegari et al's
deobfuscator.


## Fixing Segmentation Fault

Finally, we encountered a segmentation fault when running Yadegari et al's tool on
a well-formed trace input. We identified the cause of this runtime error: an
if-clause condition induced an implicit type conversion, altering its intended
behavior and making the condition check invalid.

**Solution.** We applied the following patch to fix this issue:

```diff
diff --git a/src/mem_maps.c b/src/mem_maps.c
index c1f939b..7b2f700 100644
--- a/src/mem_maps.c
+++ b/src/mem_maps.c
@@ -49,7 +49,8 @@ void MemoryPageTableLookup(InstrList *iList, ADDRESS addr, int *index){
        for(i = 0; i < iList->constantNumPageEntries; i++){
                for(j = 0; j < 8; j++){
                        int startAddr = iList->constantPageDir[i].startAddrs[j];
-                       if(addr >= startAddr && addr < startAddr + PAGE_SIZE){
+                       if(addr >= startAddr && addr < ((ADDRESS) (startAddr + PAGE_SIZE))){
+
                                index[0] = i;
                                index[1] = j;
                                return;
```

We also added relevant Linux libraries to the `ignored_modules` list, which documents
the modules to be ignored by the tool when producing simplified traces:

```diff
diff --git a/src/read.c b/src/read.c
index c78e3a5..abec81a 100644
--- a/src/read.c
+++ b/src/read.c
@@ -40,6 +40,7 @@ char *ignore_modules[] = {
         //
         "ADVAPI32", "RPCRT4", "comctl32", "SHELL32", "SHLWAPI", "USER32", "GDI32", "MPR",
         "WS2HELP", "WS2_32",
+        "ld-linux.so.2", "libc.so.6",
         //
         "", "dll",
         /*"unknown",*/ NULL };
```
