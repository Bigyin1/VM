/** @file */
#ifndef ASM_PARSER_SYMTAB_HPP
#define ASM_PARSER_SYMTAB_HPP

#include <stddef.h>
#include <stdint.h>

typedef struct symbolData
{
    const char *name;
    const char *sectionName;
    uint64_t val;

    bool defined;
    bool absolute;

    bool global;

} symbolData;

typedef struct symbolsData
{
    symbolData symbols[72]; // TODO: do something with that
    size_t symbolsSz;

    symbolData *symTab[72];
    size_t symTabSz;
} symbolsData;

#endif
