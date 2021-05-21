# cmdlib

An easy library that implements a command line interface.
Best used for adding debug CLI interfaces in firmwares.

Features:

- written in ANSI C
- builtin help
- argument parsing
- single source file
- no dynamic memory allocation
- history
- less than 500 LOC
- ideal for embedded contexts

## Usage

You can build this library with any ANSI C compatible C compiler.
The only libc headers that are required are:

- stdio.h (for snprintf, does not print to screen/use files)
- stdarg.h (for varargs)
- string.h (for various string functions, `strlen`, `memcpy`, `strcmp`, etc)

The things your environment should provided are:

- a function that writes characters somewhere (typically to a serial port)
- a method to read unbuffered characters and feed them to Cmdlib

This program doesn't do any reading/writing itself: rather it's the environment that it
has to provide characters to the program one by one.
