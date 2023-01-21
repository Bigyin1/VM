#include <assert.h>
#include <string.h>
#include <stdint.h>
#include "binformat.hpp"
#include "encode.hpp"
#include "encoder.hpp"

static size_t evalStrTabSize()
{
    return 0;
}

static int writeHeader(AsmEncoder *as)
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
        return -1;
    }

    header.entrypoint = sd->val;

    fwrite(&header, sizeof(header), 1, as->out);

    as->offset += (sizeof(header) + header.sectionsCount * sizeof(SectionHeader));

    return 0;
}

static int writeSectionsHeaders(AsmEncoder *as)
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

        fwrite(&sect, sizeof(sect), 1, as->out);

        as->offset += sect.size;
    }

    SectionHeader stringTabSect = {0}; // TODO string table
    stringTabSect.type = SECT_STR_TAB;
    stringTabSect.offset = as->offset;
    stringTabSect.nameIdx = 0;

    fwrite(&stringTabSect, sizeof(stringTabSect), 1, as->out);

    as->offset += stringTabSect.size;

    return 0;
}

static int writeSectionData(sectionNode *sect, FILE *out)
{

    for (size_t i = 0; i < sect->commandsSz; i++)
    {
        if (sect->commands[i].Type == CMD_INSTR)
            Encode(&sect->commands[i].instr, out);
        else if (sect->commands[i].Type == CMD_DATA_DEF)
            fwrite(sect->commands[i].data, 1, sect->commands[i].dataSz, out);
    }

    return 0;
}

static int writeSectionsData(AsmEncoder *as)
{
    for (size_t i = 0; i < as->parser->sectionsSz; i++)
    {

        writeSectionData(&as->parser->sections[i], as->out);
    }

    // TODO str table

    return 0;
}

int GenObjectFile(AsmEncoder *as)
{
    assert(as != NULL);

    if (writeHeader(as) != 0)
        return -1;

    if (writeSectionsHeaders(as) != 0)
        return -1;

    if (writeSectionsData(as) != 0)
        return -1;

    return 0;
}
