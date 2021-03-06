## LLParser

LLParser is a light-weight LLVM assembly parser that is not dependent on LLVM libraries. It is designed
for quick & dirty text-oriented manipulation of LLVM assembly files.


## Build

LLParser uses `CMake` and requires `CMake >= 3.5` and `GCC >= 4.8`. A typical build process starts with first creating a build directory in the project's root directory.

```bash
$ mkdir cmake-build
$ cd cmake-build
$ cmake ..  # require at least cmake 3.5
$ make
```

Running `make` makes a `build` directory where binaries and libraries are placed.

## Run A Pass

```bash
# in project root directory
$ cd build/bin
$ ./sopt -load ../pass/libHello.so yourIR.ll
```

## Overview

The most prominent distinction between LLParser and LLVM's libraries is that LLParser
is string-oriented, meaning it mainly operates on string, while LLVM's API is highly
object-oriented. For example, using LLVM's library, to create a call to a new function,
you'd first need to create the function object, to create which you need to create the objects for
the return type and a list of arguments.
To create arguments you'd first need to create their corresponding types, etc.
While using LLParser, you can create a function instance by simply passing a string such as
`declare void @myfunction(i32)` to `IRBuilder::create_function_declaration`.

Specifically, the following recipe gives you an idea of creating a new function call and inserting it:

- Find the insertion position
- Create a new instruction by literally writing it in LLVM language
  - Such as `call void @yourfunction(i32 100)`
- Pass the text to IRBuilder to automatically build an instruction in memory according to your text
- Insert the instruction into the basic block
- Create a new function declaration by writing it in LLVM language, or else `yourfunction` is undefined
  - Pass string `declare void @yourfunction(i32);` to `IRBuilder::create_function_declaration`
- Insert the new function declaration into the module
- Write the modified module back to file

Below is an incomplete list of advantages LLParser could have over LLVM's library:

- Construct data structures directly from strings.
- No need to compile LLVM from source and study its enormous APIs, which are not necessarily consistent between versions.
- LLParser is much more light-weight, manageable and highly flexible.
- Especially handy for simple transformations such as inserting an instruction, adding an argument, etc.
- No complex data structures, mostly raw STL, which is easy to handle.

It also has some interfaces that are not directly available in LLVM's pass framework:
- Get a function's debug information
- Support for inter-modular pass

LLParser also implements hot-pluggable passes, so that your pass can be loaded at run time by specifying in the command line.

LLParser operates on LLVM assembly level and leverages existing standard LLVM tool chain. Any inputs
need to be compiled to LLVM language form first using
`clang -S -emit-llvm sourcefile.c`.


## Write A LLParser Pass

- Create a directory under transform which would be the root directory of your customized pass
- Copy a pass class from `src/transform/examples/examplePasses.cpp` as well as the `src/transform/examples/CMakeLists.txt`
- Modify the pass source file and the CMakeLists.txt according to your project
- Create `cmake-build` directory and do `cd cmake-build && cmake .. && make`

The build process of the pass is similar to LLVM's pass or Intel Pin's tool.
For now each pass's CMakeLists.txt highly relies on the hard-wired relative path to
actually place the library in the LLParser' build directory.

Alternatively, you can add the pass as an a target in the root CMakeLists.txt,
instead of creating a sub-CMakelists.txt in the pass directory.


## Status

LLParser is a young project and is in constant change.
It has been tested with the C/C++ benchmarks in [CPU2006](https://www.spec.org/cpu2006/) and should support LLVM 3.4+.
LLVM 3.4's metadata format is different from later versions,
so right now all of its metadata will be ignored.
There are a few example passes in directory `src/transform`, which implement transformations like:

- Function call replacement
- Create and insert call instruction/function
- Output a call graph to a dot file

Basically LLParser provides interfaces like 
- Get a function by name from the input module
- Get a list of all functions in the module
- Get a list of all basic blocks in a function
- Get a list of all instructions in a basic block
- Get users of each function

Sopt does what is similar to what `opt` does.
A LLParser pass is also written in a similar way as the LLVM pass.


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

## Documentation

### Class Architecture

- asmParser/
    - IRParser: StringParser
      - Base class for parsing LLVM languages
    - LLParser: FileParser, IRParser
      - Parses a LLVM assembly file
    - InstParser: StringParser
      - Parses a single instruction
    - IRBuilder
      - Exposes an interface to the pass framework to build functions or instructions
    - SysDict
      - System dictionary. Provides thread-safe interfaces such as filename(), module()

- ir/
    - Shadow
      - Implements the string-oriented model. All properties of a Value object are stored
      as key-value pairs in a hash map field of Shadow
    - Value: Shadow
      - All IR objects inherit from it. Defines fundamental properties of an IR object
      such as `name`

- peropheral/
    - SysArgs
      - Maintains arguments from command line
    - ArgsParser: StringParser
      - Parses arguments from command line
    - StringParser
      - Base string parsing class
    - FileParser: StringParser
      - Base file parsing class


### Parallelize Parsing

The reason why instruction parsing is delegated to an InstParser instance instead of as part of a LLParser is
to build a asynchronous model where the file parsing and instruction parsing could happen simultaneously.
This model is similar to a parallel scavenging garbage collector(a worker thread fetches one grey object
at a time from the stack and starts traversing its neighbors).
Empirically I found that function parsing
constitute 60% of the parsing job, 30% for debug information, and 10% for other texts.
So one possible strategy where this model could be beneficial is to delay the instruction parsing to the point
where debug info parsing starts. Because debug info and instructions operate on two different lists of the
module, synchronization will be minimized.

While LLVM's `opt` only accepts one input file, `sopt` accepts multiple input files and will do the
parsing/analyzing/transforming in parallel if `-XX:+ParallelModule` is on.
Flag `-XX:+ParallelModule` is totally transparent, that is, just turning this on makes your pass run faster
without any additional work, if you have multiple files as input.

To add `-XX:+UseSplitModule` doc.