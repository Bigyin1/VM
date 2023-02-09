#include "utils.hpp"

const char* getNameFromStrTableByIdx(LinkableFile* f, uint32_t nameIdx)
{
    if (nameIdx > f->strTableSz)
        return NULL;

    return f->strTable + nameIdx;
}

const char* getNameFromExecFileStringTab(ExecutableFile* ex, uint32_t nameIdx)
{
    if (nameIdx > ex->strTableSz)
        return NULL;

    return ex->strTable + nameIdx;
}
