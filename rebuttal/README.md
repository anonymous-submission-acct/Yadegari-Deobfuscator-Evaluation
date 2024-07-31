# Supplemental Materials

## 1. Details of Tigress Parameters

In `tigress-parameters.md`, we have included the parameters and options used
in Tigress for generating obfuscated benchmark programs.

## 2. Explanation of Issues Building Yadegari et al

In `yadegari-build-issues.md`, we detail the build issues encountered and the
corresponding fixes applied.

## 3. Examples of Running Yadegari et al

In the `yadegari-examples` directory, we present five examples of obfuscated
C programs along with the outputs produced by applying Yadegari et al.'s tool:

  1. `trivial`
  2. `parity`
  3. `double_loop_branch_insertion`
  4. `double_loop_deadcode`
  5. `double_loop_flattened`

The first two programs, `trivial` and `parity`, are obfuscated solely through
branch insertion, as they are straight line programs without control flow
constructs. To further evaluate Yadegari et al.'s tool on other obfuscation
techniques, we apply three distinct types of obfuscations to the `double_loop`
program.

For each benchmark, we provide five files. Taking the program `trivial` as an
example, the files are as follows:
  
| Filename             | Comment                                                                                                     |
|----------------------|-------------------------------------------------------------------------------------------------------------|
| `trivial.c`          | The obfuscated C program.                                                                                   |
| `trivial.objdump`    | Assembly code generated by `objdump` from the obfuscated C program.                                         |
| `trivial.trace`      | Recorded trace of a single execution of the obfuscated C program, used as input for Yadegari et al.'s tool. |
| `trivial.simplified` | Simplified trace of the obfuscated C program, produced as output by Yadegari et al.'s tool.                 |
| `trivial.README.md`  | Analysis and explanation of the output.                                                                     |

Please refer to the respective README files for detailed explanations of Yadegari et al.'s tool's behavior.