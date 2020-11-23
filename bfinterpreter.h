#ifndef BFINTERPRETER_H
#define BFINTERPRETER_H

#include <stdbool.h>
#include <stdint.h>

#define TAPE_SIZE 65536

#define BF_SYSCALL_ARG_REG 0
#define BF_SYSCALL_ARG_PTR 1
#define BF_SYSCALL_ARG_CELL_PTR 2

typedef struct {
    uint8_t bf_tape[TAPE_SIZE];
    uint8_t *tape_ptr;
    char *code;
    uint32_t code_len;
    uint32_t code_ptr;
    struct bf_flags {
        bool systemf;
    } flags;
} bfcode_t;

bfcode_t *bf_new(char *code, uint32_t code_len, struct bf_flags flags);
void bf_interpret(bfcode_t *code);
void bf_syscall(bfcode_t *bfcode);

#endif
