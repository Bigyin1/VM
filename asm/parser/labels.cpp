#include <string.h>
#include "parser.hpp"



int defineNewLabel(parser_s *p, const char *label, uint64_t val) {

    for (size_t i = 0; i < p->labelsSz; i++)
    {
        if (strcmp(p->labels[i].label, label) == 0) {
            if (p->labels[i].present)
                return -1;

            p->labels[i].label = label;
            p->labels[i].present = true;
            p->labels[i].val = val;

            return 0;
        }
    }

    p->labels[p->labelsSz].label = label;
    p->labels[p->labelsSz].present = true;
    p->labels[p->labelsSz].val = val;

    p->labelsSz++;
    return 0;

}


void addLabelImport(parser_s *p, const char *label, uint64_t *v) {

    for (size_t i = 0; i < p->labelsSz; i++)
    {
        if (strcmp(p->labels[i].label, label) == 0) {

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
