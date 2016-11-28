[![Build Status](https://travis-ci.org/pamarcos/gencc.svg?branch=master)](https://travis-ci.org/pamarcos/gencc)
# gencc

*gencc* (**gen**erator of **c**ompile **c**ommands) is a simple standalone application developed in **C++11** that generates [compilation databases for clang](http://clang.llvm.org/docs/JSONCompilationDatabase.html). It is similar to [bear](https://github.com/rizsotto/Bear), but it does not rely on *LD_PRELOAD*, enabling its use in latest **OS X** versions (without disabling security features) and **Linux** with *SELinux* enabled.

## How to build

You just need *make* and a *C++11* compiler. Just run on the top directory: ```make```. This will generate the *gencc* binary.

## How to use it

Since *gencc* does not rely on hijacking any calls, you need to explicitly pass which compiler you want to use for C++ and/or C. You can do that by setting the **CXX** or **CC** environment variables or by passing the values directly in the command line. You can also decide whether you just want to build as well during the process or you just want to generate the database.

The same *gencc* binary is used both for the builder or main (master) as well as the compiler (slave). To avoid complex IPC between master and slaves that works in different OS's, every process competes to get access to the same DB file. Due to this, a very simple fallback mechanism has been implemented that retries accessing the file a number of times.

```
Usage:
	-cxx    [value] - CXX compiler
	-cc     [value] - CC compiler
	-o      [value] - DB file
	-r      [value] - Number of retries if DB locked
	-f      [value] - Max fallback time in ms in case of DB locked
	-build          - Call the actual compiler
```

Examples:

```CXX=g++ ./gencc make``` or ```./gencc -cxx g++ make``` will generate *compile_commands.json* for *g++*

```CC=gcc ./gencc make``` or ```./gencc -cc gcc make``` will generate *compile_commands.json* for *gcc*

```./gencc -cxx g++ -cc gcc make``` will generate *compile_commands.json* for both *g++* and *gcc*

## Status

This is a very early (and yet usable) version of *gencc*. Please tell me if you find any bug with as much information as possible.

It has been tested on OS X and Linux. Windows support will be added in the near future.

## License

Copyright (C) 2016 Pablo Marcos Oltra

GPLv3

## 3rd-party licenses

[JSON for Modern C++](https://github.com/nlohmann/json) is licensed under the MIT License

[googletest](https://github.com/google/googletest) is licensed under the BSD3 License

