# C++ Solutions to Advent of Code

This repository contains solutions to various years of the [Advent of Code](https://adventofcode.com)
puzzles.  All solutions are written in C++.  The solutions aren't designed to be nice or
pretty.  Instead, I am using it as an experiment in understanding how I approach solving
problems.

The solutions are authored by Matthew Gretton-Dann, and Copyright 2020-2021, Matthew Gretton-Dann.  Licensed under Apache 2.0 - see [license](./LICENSE).

## Getting the sources

To get the sources clone from git as follows:

```sh
git clone https://github.com/matt-gretton-dann/advent-of-code/
```

## Build requirements

The following is required to build the puzzles:

 * CMake
 * C++20 compiler
 * OpenSSL headers.

## Running the Puzzles

There is a driver script (`./driver.sh`) in the root of the checkout.  It will build and run specific tests.  Example
use is as follows:

```sh
YEAR=2020 # Year of puzzle to run
DAY=01 # Day of puzzle to run
PUZZLE=01 # ID of puzzle (1 or 2)
INPUT=input1.txt # Input to give the puzzle.
./driver.sh $YEAR $DAY $PUZZLE $INPUT
```

## Building the puzzles

The build system uses CMake.

```sh
cmake -Bbuild -S.
cmake --build build
```

Note that some of the puzzles require OpenSSL for the MD5 implementation.  On macOS this also requires you to pass an appropariate value for `OPENSSL_ROOT_DIR` to cmake.  On x86 this is `-DOPENSSL_ROOT_DIR=/usr/local/opt/openssl` and on AArch64 this is `-DOPENSSL_ROOT_DIR=/opt/homebrew/opt/openssl`.

## Sources

All code is written in C++.

### Directory Layout

Each year of puzzles is in a self-contained directory `YEAR`.  

Within the directory there are the following files:

 * `puzzle-DAY-NUMBER.cc`.  The source for puzzle NUMBER on day DAY in the year.
 * `driver-DAY-NUMBER.sh`.  If present this is used as the driver to run that puzzle.
 * `puzzle-DAY-NUMBER.CMakeLists.txt`.  If present this is included by `CMakeLists.txt`.  The variable `${puzzle_name}`
   contains the name of the puzzle target.  Some puzzles use this to depend on OpenSSL. 

### Command Line Interface

The executable command line interface should just take the input in on standard-input and print its
result on standard-output.

If this is not possible the script `driver-DAY-NUMBER.sh` within the year directory should be provided.  It should take
two arguments - the executable to run and the input file to use.