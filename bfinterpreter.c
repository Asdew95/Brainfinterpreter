#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "bfinterpreter.h"

bfcode_t *bf_new(char *code, uint32_t code_len, struct bf_flags flags)
{
    bfcode_t *bf = calloc(sizeof(bfcode_t), 1);
    if(bf == NULL) {
        return NULL;
    }
    bf->tape_ptr = (uint8_t*) &bf->bf_tape;
    bf->code = code;
    bf->code_len = code_len;
    bf->code_ptr = 0;
    bf->flags.systemf = flags.systemf;
    return bf;
}

void bf_interpret(bfcode_t *code)
{
    char c;
    uint32_t lstart = code->code_ptr;
    uint32_t lstartc, lendc;
    
    while(code->code_ptr < code->code_len) {
        c = code->code[code->code_ptr];
        code->code_ptr++;

        switch(c) {
            case '+':
                *(code->tape_ptr) += 1;
                break;
            case '-':
                *(code->tape_ptr) -= 1;
                break;
            case '>':
                code->tape_ptr++;
                break;
            case '<':
                code->tape_ptr--;
                break;
            case '.':
                putchar(*code->tape_ptr);
                break;
            case ',':
                *code->tape_ptr = getchar();
                break;
            case '[':
                if(*code->tape_ptr != 0) {
                    bf_interpret(code);
                } else {
                    /* Find the matching bracket. */
                    lstartc = 1;
                    lendc = 0;
                    while(code->code_ptr < code->code_len) {
                        c = code->code[code->code_ptr];
                        code->code_ptr++;
                        if(c == '[') {
                            lstartc++;
                        } else if(c == ']') {
                            if(lstartc == ++lendc) {
                                break;
                            }
                        }
                    }
                }
                break;
            case ']':
                if(*code->tape_ptr != 0) {
                    code->code_ptr = lstart;
                } else {
                    return;
                }
                break;
            case '%':
                if(code->flags.systemf) {
                    bf_syscall(code);
                }
                break;
            default:
                break;
        }
    }
}

void bf_syscall(bfcode_t *bfcode)
{
    int i, j;
    uint8_t *arg_pointer = bfcode->tape_ptr + 2;
    uint8_t arg_cells;
    long ret_value = 0;
    long args[6];
    uint8_t argc = *(bfcode->tape_ptr + 1);
    long syscall_num = (long) *bfcode->tape_ptr;
    uint8_t arg_type;

    for(i = 0; i < argc; i++) {
        arg_type = *arg_pointer;
        arg_cells = (uint8_t) *(arg_pointer + 1);
        args[i] = 0;
        if(arg_type == BF_SYSCALL_ARG_PTR) {
            args[i] = (long) (arg_pointer + 2);
        } else {
            for(j = 0; j < arg_cells; j++) {
                args[i] |= *(arg_pointer + 2 + j)
                    << (arg_cells * 8 - (j + 1) * 8);
            }
            if(arg_type == BF_SYSCALL_ARG_CELL_PTR) {
                args[i] += (long) &bfcode->bf_tape;
            }
        }
        arg_pointer += arg_cells + 2;
    }

    ret_value = syscall(syscall_num, args[0], args[1], args[2],
                        args[3], args[4], args[5]);
    
    *bfcode->tape_ptr = (uint8_t) ret_value;
}
