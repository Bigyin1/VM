#include <stdio.h>
#include <stdlib.h>
#include <cassert>
#include "utils.hpp"


char *readFile(const char *name) {
    assert(name != NULL);

    char *text = NULL;

    FILE *f = fopen(name, "r");
    if (f == NULL)
        return NULL;


    if (fseek(f, 0, SEEK_END) != 0)
        return NULL;

    long fSize = ftell(f);
    if (fSize == -1)
        return NULL;

    if (fseek(f, 0, SEEK_SET) != 0)
        return NULL;

    text = (char *)calloc((size_t)fSize + 1, sizeof(char));
    if (text == NULL)
        return NULL;

    if (fread(text, sizeof(char), (size_t)fSize, f) != (size_t)fSize) {
        free(text);
        return NULL;
    }

    text[fSize] = '\0';

    fclose(f);
    return text;
}
