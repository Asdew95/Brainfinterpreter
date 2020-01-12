#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define TAPE_SIZE 65536

#define BF_SYSCALL_ARG_REG      0
#define BF_SYSCALL_ARG_PTR      1
#define BF_SYSCALL_ARG_CELL_PTR 2

uint8_t bf_tape[TAPE_SIZE] = {0};

/* This is a pointer to the currently selected cell. */
uint8_t *tape_pointer = (uint8_t*) bf_tape;

uint32_t flags = 0x00000000;

int interpret_loop(FILE *f);

void bf_syscall(void);

int interpret(char *filename) {
    FILE *f = fopen(filename, "r");
    if(f == NULL) {
        fprintf(stderr, "Error when opening %s: %s\n",
                filename, strerror(errno));
        return errno;
    }
    interpret_loop(f);
    fclose(f);
    return 0;
}

int interpret_loop(FILE *f) {
    int c;
    uint32_t loop_start_count, loop_end_count;
    long loop_start = ftell(f);
    
    while((c = fgetc(f)) != EOF) {
        /* Here interpret the character c. */
        switch(c) {
            case '+':
                (*tape_pointer)++;
                break;
            case '-':
                (*tape_pointer)--;
                break;
            case '>':
                tape_pointer++;
                break;
            case '<':
                tape_pointer--;
                break;
            case '.':
                putchar(*tape_pointer);
                break;
            case ',':
                *tape_pointer = getchar();
                break;
            case '[':
                if(*tape_pointer != 0) {
                    interpret_loop(f);
                } else {
                    loop_start_count = 1;
                    loop_end_count = 0;
                    while((c = fgetc(f)) != EOF) {
                        if(c == '[') {
                            loop_start_count++;
                        } else if(c == ']') {
                            if(loop_start_count == ++loop_end_count) {
                                break;
                            }
                        }
                    }
                }
                break;
            case ']':
                if(*tape_pointer != 0) {
                    fseek(f, loop_start, SEEK_SET);
                } else {
                    return 0;
                }
                break;
            case '%':
                /* Only support this if bit 0 is set (systemf mode flag). */
                if(flags & 0x00000001) {
                    bf_syscall();
                }
                break;
            default:
                break;
        }
    }
    return 0;
}

void bf_syscall(void) {
    int i, j;
    uint8_t *arg_pointer = tape_pointer + 2;
    uint8_t arg_cells;
    long ret_value = 0;
    /* There can only be at most 6 arguments, so just make an array of 6 */
    long args[6];
    uint8_t argc = *(tape_pointer + 1);
    long syscall_num = (long) *tape_pointer;
    uint8_t arg_type;

    for(i = 0; i < argc; i++) {
        arg_type = *arg_pointer;
        arg_cells = (uint8_t) *(arg_pointer + 1);
        args[i] = 0;
        if(arg_type == BF_SYSCALL_ARG_PTR) {
            args[i] = (long) (arg_pointer + 2);
        } else {
            for(j = 0; j < arg_cells; j++) {
                args[i] |= *(arg_pointer + 2 + j) << (arg_cells * 8 - (j + 1) * 8);
            }
            if(arg_type == BF_SYSCALL_ARG_CELL_PTR) {
                args[i] += (long) &bf_tape;
            }
        }
        arg_pointer += arg_cells + 2;
    }

    /* I'm not proud of this. I'm sure there is a better way. I just don't */
    /* know the better way. If someone knows it, please tell me.           */
    switch(argc) {
        case 0:
            ret_value = syscall(syscall_num);
            break;
        case 1:
            ret_value = syscall(syscall_num, args[0]);
            break;
        case 2:
            ret_value = syscall(syscall_num, args[0], args[1]);
            break;
        case 3:
            ret_value = syscall(syscall_num, args[0], args[1], args[2]);
            break;
        case 4:
            ret_value = syscall(syscall_num, args[0], args[1], args[2],
                    args[3]);
            break;
        case 5:
            ret_value = syscall(syscall_num, args[0], args[1], args[2],
                    args[3], args[4]);
            break;
        case 6:
            ret_value = syscall(syscall_num, args[0], args[1], args[2],
                    args[3], args[4], args[5]);
            break;
    }
    
    *tape_pointer = (uint8_t) ret_value;
}

int main(int argc, char *argv[]) {
    int ret;
    int i;
    if(argc == 1) {
        printf("Please provide me a file to interpret.\n");
        return 1;
    }
    for(i = 1; i < argc; i++) {
        if(!strcmp(argv[i], "--systemf")) {
            flags |= 0x00000001;
        }
    }
    ret = interpret(argv[1]);
    return ret;
}
