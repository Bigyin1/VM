/** @file */
#ifndef ASM_PARSER_SYMTAB_HPP
#define ASM_PARSER_SYMTAB_HPP

#include <stddef.h>
#include <stdint.h>

typedef struct symbolData
{
    const char *label;
    uint64_t val;

    uint64_t *imports[16];
    size_t importsSz;

    bool present;

} symbolData;

typedef struct symbolTable
{
    symbolData symbols[64];
    size_t symbolsSz;
};

const symbolData *findSymbolByName(symbolTable *symTab, const char *name);

#endif
