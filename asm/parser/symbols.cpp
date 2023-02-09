#include "symbols.hpp"

#include <string.h>

#include "errors.hpp"

static ParserErrCode addNewSymbol(Parser* p, const char* symbol, uint64_t val,
                                  bool define, bool abs);

ParserErrCode defineNewSymbol(Parser* p, const char* name, uint64_t val)
{

    return addNewSymbol(p, name, val, true, false);
}

ParserErrCode defineNewAbsSymbol(Parser* p, const char* name, uint64_t val)
{

    return addNewSymbol(p, name, val, true, true);
}

ParserErrCode addSymbolReference(Parser* p, const char* name, uint64_t val)
{

    return addNewSymbol(p, name, val, false, false);
}

static bool addSymbolToSymTab(symbolsData* t, symbolData* symb)
{
    for (size_t i = 0; i < t->symTabSz; i++)
    {
        if (strcmp(t->symTab[i]->name, symb->name) == 0)
        {

            if (t->symTab[i]->defined && symb->defined)
                return false;

            if (!t->symTab[i]->defined && symb->defined)
                t->symTab[i] = symb;

            return true;
        }
    }

    t->symTab[t->symTabSz] = symb;

    t->symTabSz++;

    return true;
}

static bool symbolIsGlobal(const char* name)
{

    if (name[0] != '.')
        return true;

    return false;
}

static ParserErrCode addNewSymbol(Parser* p, const char* name, uint64_t val,
                                  bool define, bool abs)
{

    symbolData* nextSymb = &p->symsData.symbols[p->symsData.symbolsSz];

    nextSymb->name        = name;
    nextSymb->sectionName = p->currSection->name;
    nextSymb->defined     = define;
    nextSymb->absolute    = abs;
    nextSymb->global      = symbolIsGlobal(name);
    nextSymb->val         = val;

    bool added = addSymbolToSymTab(&p->symsData, nextSymb);
    if (define && !added)
        return PARSER_LABEL_REDEF;

    p->symsData.symbolsSz++;

    return PARSER_OK;
}
