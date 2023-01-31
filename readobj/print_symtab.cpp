#include <stdlib.h>
#include "readobj.hpp"
#include "print_symtab.hpp"

static const char *ifSpecialSectIdx(uint32_t sectIdx)
{

    if (sectIdx == SHN_ABS)
        return "ABS";

    if (sectIdx == SHN_UNDEF)
        return "UND";

    return NULL;
}

int printSymbolTable(ReadObj *r)
{

    fprintf(r->out, "\nSymbol table has %u elements:\n", r->symTabSz);

    fprintf(r->out, "[Idx:]\tValue\t\tSect.Idx\tName\n");
    for (uint16_t i = 0; i < r->symTabSz; i++)
    {
        SymTabEntry *curr = &r->symTable[i];

        const char *specSect = ifSpecialSectIdx(curr->sectHeaderIdx);
        if (specSect == NULL)
            fprintf(r->out, "  %u:\t%011lu\t\t%u\t%s\n", i,
                    curr->value, curr->sectHeaderIdx, getNameFromStrTable(r, curr->nameIdx));
        else
        {

            fprintf(r->out, "  %u:\t%011lu\t\t%s", i, curr->value, specSect);
            fprintf(r->out, "\t%s\n", getNameFromStrTable(r, curr->nameIdx));
        }
    }

    return 0;
}
