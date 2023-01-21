#include <string.h>
#include "errors.hpp"
#include "labels.hpp"

ParserErrCode defineNewLabel(Parser *p, const char *label, uint64_t val)
{

    for (size_t i = 0; i < p->labelsSz; i++)
    {
        if (strcmp(p->labels[i].label, label) == 0)
        {
            if (p->labels[i].present)
                return PARSER_LABEL_REDEF;

            p->labels[i].label = label;
            p->labels[i].present = true;
            p->labels[i].val = val;

            return PARSER_OK;
        }
    }

    p->labels[p->labelsSz].label = label;
    p->labels[p->labelsSz].present = true;
    p->labels[p->labelsSz].val = val;

    p->labelsSz++;
    return PARSER_OK;
}

void addLabelImport(Parser *p, const char *label, uint64_t *v)
{

    for (size_t i = 0; i < p->labelsSz; i++)
    {
        if (strcmp(p->labels[i].label, label) == 0)
        {

            size_t impSz = p->labels[i].importsSz;

            p->labels[i].imports[impSz] = v;

            p->labels[i].importsSz++;

            return;
        }
    }

    p->labels[p->labelsSz].label = label;
    p->labels[p->labelsSz].imports[0] = v;
    p->labels[p->labelsSz].importsSz++;

    p->labelsSz++;
}

ParserErrCode resolveImports(Parser *p)
{

    for (size_t i = 0; i < p->labelsSz; i++)
    {
        labelData *ld = &p->labels[i];
        if (!ld->present)
        {
            ParserError *err = addNewParserError(p, PARSER_LABEL_UNDEFIEND);

            err->token = ld->label;
        }

        for (size_t j = 0; j < ld->importsSz; j++)
            *ld->imports[j] = ld->val;
    }
    return PARSER_OK;
}
