# Tigress Parameters

We utilized the following Tigress options to generate our benchmark programs.
For each obfuscation transformation, we first present the Tigress options parameterized by variables
and then provide the concrete values used in the benchmark programs.

Additionally, we composed the obfuscation transformations by applying Tigress multiple times with different options.

## Flatten

```
["--Transform=Flatten", "--FlattenSplitBasicBlocks={split_basic_blocks}", "--Functions={func}",
                        "--FlattenDispatch={dispatch}",
                        "--FlattenNumberOfBlocksPerFunction={num_block_per_func}",
                        "--FlattenConditionalKinds={conditional_kinds}",
                        "--FlattenRandomizeBlocks={randomize_blocks}",
                        "--FlattenObfuscateNext={obfuscate_next}",
  "--Transform=CleanUp", "--CleanUpKinds={cleanup_kinds}"]
```

```
dispatch: switch, goto, indirect, call
num_block_per_func: 1, 2
split_basic_blocks: true
randomize_blocks: true
obfuscate_next: false
conditional_kinds: branch
cleanup_kinds: removeUnusedFunctions
```


## Deadcode Insertion

```
["--Transform=InitEntropy", "--Functions={func}", "--InitEntropyKinds=vars",
        "--Transform=InitOpaque", "--Functions={func}", "--InitOpaqueCount={num_opaques}", "--InitOpaqueStructs={opaque_structs}",
        "--Transform=AddOpaque", "--Functions={func}", "--AddOpaqueKinds={opaque_kinds}", "--AddOpaqueCount={num_opaque_per_function}", "--AddOpaqueStructs={opaque_structs}",
        "--Transform=CleanUp", "--CleanUpKinds={cleanup_kinds}"]
```

```
num_opaques: 2
num_opaque_per_function: 2
opaque_kinds: call, bug, true
opaque_structs: env, list
cleanup_kinds: removeUnusedFunctions
```

## Block Fission

```
["--Transform=Flatten", "--FlattenSplitBasicBlocks=true", "--Functions={func}",
    "--Transform=InitEntropy", "--Functions={func}", "--InitEntropyKinds=vars",
    "--Transform=InitOpaque", "--Functions={func}", "--InitOpaqueCount={num_opaques}", "--InitOpaqueStructs={opaque_structs}",
    "--Transform=AddOpaque", "--Functions={func}", "--AddOpaqueKinds={opaque_kinds}", "--AddOpaqueCount={num_opaque_per_function}", "--AddOpaqueStructs={opaque_structs}",
    "--Transform=CleanUp", "--CleanUpKinds={cleanup_kinds}"]
```

```
num_opaques: 2
num_opaque_per_function: 2
opaque_structs: env, list
opaque_kinds: true
cleanup_kinds: removeUnusedFunctions
```
