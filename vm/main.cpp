#include <stdio.h>
#include <stdlib.h>
#include <cassert>
#include "cpu/cpu.hpp"
#include "cpu/instructions.hpp"


instrBin_s *readCode(const char *fName) {
    assert(fName != NULL);

    instrBin_s *code = NULL;

    FILE *f = fopen(fName, "r");
    if (f == NULL)
        return NULL;


    if (fseek(f, 0, SEEK_END) != 0)
        return NULL;

    long fSize = ftell(f);
    if (fSize == -1)
        return NULL;

    if (fSize % sizeof(instrBin_s) != 0)
        return NULL;

    if (fseek(f, 0, SEEK_SET) != 0)
        return NULL;

    code = (instrBin_s *)calloc((size_t)fSize / sizeof(instrBin_s), sizeof(instrBin_s));
    if (code == NULL)
        return NULL;

    if (fread(code, sizeof(char), (size_t)fSize, f) != (size_t)fSize) {
        free(code);
        return NULL;
    }

    fclose(f);
    return code;
}

int main(int argc, char **argv) {

    if (argc != 2) {
        printf("Wrong args count\n");
        return 1;
    }





    instrBin_s *code = readCode(argv[1]);
    if (code == NULL) {
        perror(argv[1]);
        return 1;
    }

    while (code->opCode != 0xC) {
        printf("%s\n", findInstrByOpCode(code->opCode)->name);
        ++code;
    }



}
