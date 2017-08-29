## LLParser

LLParser is a light-weight LLVM assembly parser that is not dependent on LLVM libraries. It is designed
for quick & dirty text-oriented manipulation of the LLVM assemblies. If you need to write a LLVM pass 
but do not want to use LLVM as a library to avoid complexity, LLParser is for you.

The most prominent distinction between LLParser and LLVM's libraries is that LLParser 
is string-oriented, meaning it mainly operates on string, while LLVM's API is
object-oriented. For example, using LLVM's library, to create a call to a new function, you'd 
first need to create the function, to create which you first need to create the return type 
and a list of arguments. To create arguments you'd first need to create their corresponding types, etc.
While using LLParser, you can simple write create a function by making a string such as 
"declare void @myfunction(i32)", and then call `IRBuilder::create_function_declaration` and pass 
the string as the parameter, which returns a pointer to a data structure that represents the function. 
Specifically, the following recipe gives you an idea of creating a new function call and inserting it:

- Find the insertion position
- Create a new instruction by literally writing the text of it in LLVM language
  - Such as `call void @yourfunction(i32 100)`
- Pass the text to IRBuilder to automatically build an instruction in memory according to your text
- Insert the instruction into the basic block
- Create a new function declaration by writing the text of it in LLVM language
  - Such as `declare void @yourfunction(i32);`
- Likewise, create a function in memory from the string
- Insert the new function declaration into the module
- Write the modified module back to file

Below is an incomplete list of advantages LLParser could have over LLVM's library:

- Construct data structures directly from strings.
- No need to compile LLVM from source and study its enormous core libraries, which are not necessarily consistent across versions.
- LLParser is much more light-weight, manageable and highly flexible.
- Especially good for simple transformations such as inserting an instruction, adding an argument, etc.
- Modest build requirements (GCC > 4.4 with C++0x) (Will give up this abstinence soon).
- No complex data structures, mostly raw STL, which is easy to handle.

LLParser also implements hot-pluggable passes, so that your pass can be loaded at run time by specifying in the command line.

## Build

LLParser uses `CMake`. A typical build process starts with first creating a build directory in the project's root directory.

```bash
$ mkdir build
$ cd build
$ cmake ..  # require at least cmake 3.5
$ make
```

Running `make` makes a `debug` directory where binaries and libraries are placed. 

## Run

```bash
$ cd debug/bin
$ ./sopt --help
```

## Overview

LLParser operates on LLVM assembly level and leverages existing standard LLVM tool chain. Any inputs
need to be compiled to assembly form first using
`clang -S -emit-llvm sourcefile.c`. If you have multiple source files, you might also want to link them
using `llvm-link -S source1.ll source2.ll source3.ll -o linked.ll`

LLParser scans the assembly file for one pass.


## Status

LLParser is a young project and is in constant change. It should support LLVM 3.4+. LLVM 3.4's metadata format is different from later
versions, so right now all of its metadata will be ignored. There are a few example passes in directory 
`src/transform`, which implement transformations like:

- Function call replacement
- Create call instruction
- Output a call graph to a dot file. You can also only output call graphs for some specific functions

Basically LLParser provides interfaces like 
- Get a function by name from the input module
- Get a list of all functions in the module
- Get a list of all basic blocks in a function
- Get users of each function

Which is similar to what `opt` does. A pass is also written in a similar way as the LLVM pass, such as defining your routine to run on each function, etc. 

TODO: add official programming guide for LLParser.

Doxygen is hosted on [CodeDocs](https://codedocs.xyz/GentlyGuitar/LLParser/classes.html).


## Caveat

LLParser does perform some syntax check on the input file, but it is always safer to make 
sure the .ll file is already well-formed.

If you need to manually add some comments to the .ll files produced by clang, note that
a random inserted comment might break the parser, but inserting the comment as a new line
starting with ';' is fine.

```
; My custom comment
declare i32 @puts(i8* nocapture) nounwind
```