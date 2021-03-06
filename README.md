[![Build Status](https://travis-ci.org/pamarcos/gencc.svg?branch=master)](https://travis-ci.org/pamarcos/gencc)
[![codecov](https://codecov.io/gh/pamarcos/gencc/branch/master/graph/badge.svg)](https://codecov.io/gh/pamarcos/gencc)
[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](http://www.gnu.org/licenses/gpl-3.0)
[![Coverity](https://scan.coverity.com/projects/11476/badge.svg)](https://scan.coverity.com/projects/pamarcos-gencc)

# gencc

*gencc* (**gen**erator of **c**ompile **c**ommands) is a simple standalone application developed in **C++11** that generates [compilation databases for clang](http://clang.llvm.org/docs/JSONCompilationDatabase.html). It is similar to [bear](https://github.com/rizsotto/Bear), but it does not rely on *LD_PRELOAD*, enabling its use in latest **OS X** versions (without disabling security features) and **Linux** with *SELinux* enabled.

*gencc* is useful to run build systems such as *make*, capturing compiler calls. If you are using *CMake*, forget about this and use directly *CMAKE\_EXPORT\_COMPILE\_COMMANDS* so that it exports the database at the same time it generates the build files.

## How to build

You just need *make* and a *C++11* compiler. Just run on the top directory: ```make```. This will generate the *gencc* binary.

## How to use it

Since *gencc* does not rely on hijacking any calls, you need to explicitly pass which compiler you want to use for C++ and/or C. You can do that by setting the **CXX** or **CC** environment variables or by passing the values directly in the command line. You can also decide whether you just want to build as well during the process or you just want to generate the database.

The same *gencc* binary is used both for the builder or main (master) as well as the compiler (slave). They communicate with each other through a shared memory area.

```
Usage:
	-cxx    [value] - CXX compiler
	-cc     [value] - CC compiler
	-o      [value] - Output file
	-m      [value] - Size of shared memory to use in bytes
	-build          - Call the actual compiler
```

Examples:

```CXX=g++ ./gencc make``` or ```./gencc -cxx g++ make``` will generate *compile_commands.json* for *g++*

```CC=gcc ./gencc make``` or ```./gencc -cc gcc make``` will generate *compile_commands.json* for *gcc*

```./gencc -cxx g++ -cc gcc make``` will generate *compile_commands.json* for both *g++* and *gcc*

## How to run unit tests

You need a system with Unix tools such as *bash*, *diff*, *sed* and *sort*. For Windows you can get that with [MSYS](https://msys2.github.io/).

Run ```make check``` and both unit and functional tests will run.

## Status

This is a very early (and yet usable) version of *gencc*. Please tell me if you find any bug with as much information as possible.

It has been tested on OS X, Linux and Windows (with [MSYS](https://msys2.github.io/)).

## License

**GPLv3**

Copyright (C) 2017 Pablo Marcos Oltra

gencc is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

gencc is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with gencc.  If not, see <http://www.gnu.org/licenses/>.

## 3rd-party licenses

[Cross-platform C++ Snippet Library](https://github.com/cubiclesoft/cross-platform-cpp) is licensed under a dual MIT and LGPL license

[JSON for Modern C++](https://github.com/nlohmann/json) is licensed under the MIT License

[googletest](https://github.com/google/googletest) is licensed under the BSD3 License

