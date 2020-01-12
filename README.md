# Brainfinterpreter
A Brainfuck interpreter written in C.
Supports the syscall extension from https://github.com/ajyoon/systemf.
## How to compile and run
Brainfinterpreter runs natively on Linux.
To compile Brainfinterpreter, you need GCC (or some other C compiler).
You may compile Brainfinterpreter by running the following command:

`gcc -std=c89 -Wall -Werror -pedantic main.c -o brainfinterpreter`

You can then run a Brainfuck file with the following command:

`./brainfinterpreter file.b`

To support systemf syscalls,
add the command line argument `--systemf` **after** the file.
