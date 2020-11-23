# Brainfinterpreter

A Brainfuck interpreter written in C89 with GNU extensions.
Supports the syscall extension from https://github.com/ajyoon/systemf.

## How to compile and run

Brainfinterpreter runs natively on Linux.
To compile Brainfinterpreter, you need GCC (or some other C compiler).
You may compile Brainfinterpreter by running the following command:

`make`

You can then run a Brainfuck file with the following command:

`./brainfinterpreter file.b`

To support systemf syscalls, add the command line argument `--systemf`.

## To developers

If you are interested in running Brainfuck from your own program (unoptimized),
you can take a look at bfinterpreter.c and adapt it for your own purposes.
