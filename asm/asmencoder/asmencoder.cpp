#include <assert.h>
#include <string.h>
#include <stdint.h>
#include "binformat.hpp"
#include "encode.hpp"
#include "asmencoder/asmencoder.hpp"

static size_t evalStrTabSize()
{
    return 0;
}

static EncErrCode writeHeader(AsmEncoder *as)
{
    BinformatHeader header = {0};
    header.magic = magicHeader;
    header.version = formatVersion;

    header.sectionsCount = as->parser->sectionsSz + 1; // + string table
    header.stringTableIdx = header.sectionsCount - 1;

    const symbolData *sd = findSymbolByName(&as->parser->symTab, "main");
    if (sd == NULL)
    {
        fprintf(stderr, "asm: entrypoint symbol \"main\" not found\n");
        return ENC_USER_ERROR;
    }

    header.entrypoint = sd->val;

    if (fwrite(&header, sizeof(header), 1, as->out) == 0)
        return ENC_SYSTEM_ERROR;

    as->offset += (sizeof(header) + header.sectionsCount * sizeof(SectionHeader));

    return ENC_OK;
}

static EncErrCode writeSectionsHeaders(AsmEncoder *as)
{
    assert(as != NULL);

    for (size_t i = 0; i < as->parser->sectionsSz; i++)
    {
        SectionHeader sect = {0};

        sect.addr = as->parser->sections[i].addr;
        sect.offset = as->offset;
        sect.type = SECT_LOAD;
        sect.nameIdx = 0; // TODO string table
        sect.size = as->parser->sections[i].currOffset;

        if (fwrite(&sect, sizeof(sect), 1, as->out) == 0)
            return ENC_SYSTEM_ERROR;

        as->offset += sect.size;
    }

    SectionHeader stringTabSect = {0}; // TODO string table
    stringTabSect.type = SECT_STR_TAB;
    stringTabSect.offset = as->offset;
    stringTabSect.nameIdx = 0;

    if (fwrite(&stringTabSect, sizeof(stringTabSect), 1, as->out) == 0)
        return ENC_SYSTEM_ERROR;

    as->offset += stringTabSect.size;

    return ENC_OK;
}

static EncErrCode writeSectionData(sectionNode *sect, FILE *out)
{

    for (size_t i = 0; i < sect->commandsSz; i++)
    {
        if (sect->commands[i].Type == CMD_INSTR)
        {
            if (Encode(&sect->commands[i].instr, out) != 0)
                return ENC_SYSTEM_ERROR;
        }
        else if (sect->commands[i].Type == CMD_DATA_DEF)
            if (fwrite(sect->commands[i].data, 1, sect->commands[i].dataSz, out) == 0)
                return ENC_SYSTEM_ERROR;
    }

    return ENC_OK;
}

static EncErrCode writeSectionsData(AsmEncoder *as)
{
    for (size_t i = 0; i < as->parser->sectionsSz; i++)
    {
        EncErrCode err = writeSectionData(&as->parser->sections[i], as->out);
        if (err != ENC_OK)
            return err;
    }

    // TODO: str table

    return ENC_OK;
}

EncErrCode GenObjectFile(AsmEncoder *as)
{
    assert(as != NULL);

    EncErrCode err = writeHeader(as);
    if (err != ENC_OK)
        return err;

    err = writeSectionsHeaders(as);
    if (err != ENC_OK)
        return err;

    return writeSectionsData(as);
}
