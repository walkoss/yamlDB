/*
**  Filename: xmalloc.c
**
**  Made by: Walkoss on 10/11/2017.
**
**  Description: 
*/

#include "xmalloc.h"
#include "../database/database.h"

void *xmalloc(size_t size, const char *functionName) {
    void *ptr;

    ptr = malloc(size);
    if (!ptr) {
        sprintf(error, "Malloc failed for %s function\n", functionName);
    }
    return ptr;
}