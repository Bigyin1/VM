#include <string.h>
#include "errors.hpp"
#include "labels.hpp"

ParserErrCode defineNewLabel(Parser *p, const char *label, uint64_t val)
{

    for (size_t i = 0; i < p->symTab.symbolsSz; i++)
    {
        symbolData *sd = &p->symTab.symbols[i];
        if (strcmp(sd->label, label) == 0)
        {
            if (sd->present)
                return PARSER_LABEL_REDEF;

            sd->label = label;
            sd->present = true;
            sd->val = val;

            return PARSER_OK;
        }
    }

    p->symTab.symbols[p->symTab.symbolsSz].label = label;
    p->symTab.symbols[p->symTab.symbolsSz].present = true;
    p->symTab.symbols[p->symTab.symbolsSz].val = val;

    p->symTab.symbolsSz++;
    return PARSER_OK;
}

void addLabelImport(Parser *p, const char *label, uint64_t *v)
{

    for (size_t i = 0; i < p->symTab.symbolsSz; i++)
    {
        symbolData *sd = &p->symTab.symbols[i];
        if (strcmp(sd->label, label) == 0)
        {

            size_t impSz = sd->importsSz;

            sd->imports[impSz] = v;

            sd->importsSz++;

            return;
        }
    }

    p->symTab.symbols[p->symTab.symbolsSz].label = label;
    p->symTab.symbols[p->symTab.symbolsSz].imports[0] = v;
    p->symTab.symbols[p->symTab.symbolsSz].importsSz++;

    p->symTab.symbolsSz++;
}

ParserErrCode resolveImports(Parser *p)
{

    for (size_t i = 0; i < p->symTab.symbolsSz; i++)
    {
        symbolData *sd = &p->symTab.symbols[i];
        if (!sd->present)
        {
            ParserError *err = addNewParserError(p, PARSER_LABEL_UNDEFIEND);

            err->token = sd->label;
        }

        for (size_t j = 0; j < sd->importsSz; j++)
            *sd->imports[j] = sd->val;
    }
    return PARSER_OK;
}

const symbolData *findSymbolByName(symbolTable *symTab, const char *name)
{

    for (size_t i = 0; i < symTab->symbolsSz; i++)
    {
        symbolData *sd = &symTab->symbols[i];
        if (!sd->present)
            continue;

        if (strcmp(sd->label, name) == 0)
            return sd;
    }

    return NULL;
}
