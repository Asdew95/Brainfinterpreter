#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "bfinterpreter.h"

#define INITIAL_FILE_ARRAY_SIZE 16

int interpret(char *filename, struct bf_flags flags)
{
    long len;
    char *code;
    FILE *f;
    bfcode_t *bfcode;

    f = fopen(filename, "r");
    if(f == NULL) {
        fprintf(stderr, "Error when opening %s: %s\n",
                filename, strerror(errno));
        return errno;
    }

    fseek(f, 0L, SEEK_END);
    len = ftell(f);
    fseek(f, 0L, SEEK_SET);
    
    code = (char*) malloc(len);

    if(code == NULL) {
        fprintf(stderr, "Failed to allocate memory for a file!\n");
        return 1;
    }

    fread(code, 1, len, f);
    fclose(f);

    bfcode = bf_new(code, len, flags);
    
    if(bfcode == NULL) {
        fprintf(stderr, "Failed to allocate memory for a new Brainfuck code "
                "instance!\n");
        return 1;
    }

    bf_interpret(bfcode);

    free(code);
    free(bfcode);

    return 0;
}

int main(int argc, char *argv[])
{
    int i;
    int ret;
    struct bf_flags flags;
    char **files;
    size_t filesc = 0;
    size_t files_len = INITIAL_FILE_ARRAY_SIZE;

    files = malloc(sizeof(char**) * files_len);
    if(files == NULL) {
        fprintf(stderr, "Couldn't allocate memory for the list of files.\n");
        return 1;
    }

    for(i = 1; i < argc; i++) {
        if(!strcmp(argv[i], "--systemf")) {
            flags.systemf = true;
        } else {
            if(filesc > files_len) {
                files_len *= 2;
                files = realloc(files, sizeof(char**) * files_len);
                if(files == NULL) {
                    fprintf(stderr, "Failed to change the size of the list of "
                            "arrays.\n");
                    return 1;
                }
            }
            files[filesc++] = argv[i];
        }
    }

    if(filesc == 0) {
        fprintf(stderr, "Please provide me a file to interpret.\n");
        return 1;
    }

    for(i = 0; i < filesc; i++) {
        ret = interpret(files[i], flags);
        if(ret != 0) {
            return ret;
        }
    }

    free(files);

    return 0;
}
