#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "binformat.hpp"
#include "encode.hpp"
#include "asmencoder/asmencoder.hpp"

static bool sectionHasRelocs(symbolsData *symTab, const char *sectName)
{

    for (size_t j = 0; j < symTab->symbolsSz; j++)
    {
        if (strcmp(sectName, symTab->symbols[j].sectionName) == 0)
            if (!symTab->symbols[j].defined)
                return true;
    }

    return false;
}

static uint16_t getSectionsCount(AsmEncoder *as)
{
    uint16_t sectCount = (uint16_t)as->parser->sectionsSz;
    sectCount += 2; // symtab and strtab

    for (size_t i = 0; i < as->parser->sectionsSz; i++)
    {
        if (sectionHasRelocs(&as->parser->symsData, as->parser->sections[i].name))
            sectCount++;
    }

    return sectCount;
}

static size_t getRelocSectsCount(AsmEncoder *as)
{
    size_t sectCount = 0;
    for (size_t i = 0; i < as->parser->sectionsSz; i++)
    {
        if (sectionHasRelocs(&as->parser->symsData, as->parser->sections[i].name))
            sectCount++;
    }

    return sectCount;
}

static uint32_t getUserDefinedSectionsSummarySize(AsmEncoder *as)
{

    uint32_t size = 0;
    for (size_t i = 0; i < as->parser->sectionsSz; i++)
        size += (uint32_t)as->parser->sections[i].currOffset;

    return size;
}

static uint32_t getRelocationsCountInSection(symbolsData *symTab, const char *sectName)
{
    uint32_t count = 0;
    for (size_t i = 0; i < symTab->symbolsSz; i++)
    {
        if (symTab->symbols[i].defined)
            continue;

        if (strcmp(symTab->symbols[i].sectionName, sectName) == 0)
            count++;
    }

    return count;
}

static void strTabAdd(AsmEncoder *as, const char *str, bool terminate)
{
    uint32_t len = (uint32_t)strlen(str);
    if (terminate)
        len++;

    if (as->strTabSize + len > as->strTabCapacity)
    {
        as->strTabCapacity *= 2;
        as->strTab = (char *)realloc(as->strTab, sizeof(char) * as->strTabCapacity); // TODO: errors
    }

    if (terminate)
        strcpy(as->strTab + as->strTabSize, str);
    else
        memcpy(as->strTab + as->strTabSize, str, len);

    as->strTabSize += len;
}

static void strTabAddCompositeSectName(AsmEncoder *as, const char *prefix, const char *name)
{
    strTabAdd(as, prefix, false);
    strTabAdd(as, name, true);
}

static uint16_t getSectHdrIdxBySectName(AsmEncoder *as, const char *sectName)
{

    for (uint16_t i = 0; i < as->parser->sectionsSz; i++)
    {
        if (strcmp(sectName, as->parser->sections[i].name) == 0)
            return i;
    }

    return 0;
}

static void buildSymTab(AsmEncoder *as)
{
    size_t symTabSz = as->parser->symsData.symTabSz;
    SymTabEntry *symTab = (SymTabEntry *)calloc(symTabSz, sizeof(SymTabEntry));

    for (size_t i = 0; i < symTabSz; i++)
    {
        symbolData *currSymb = as->parser->symsData.symTab[i];

        symTab[i].nameIdx = as->strTabSize;
        strTabAdd(as, currSymb->name, true);

        if (currSymb->defined)
            symTab[i].value = currSymb->val;
        else
            symTab[i].sectHeaderIdx = SHN_UNDEF;

        if (currSymb->absolute)
            symTab[i].sectHeaderIdx = SHN_ABS;
        else if (currSymb->defined)
            symTab[i].sectHeaderIdx = getSectHdrIdxBySectName(as, currSymb->sectionName);

        if (currSymb->global)
            symTab[i].symbVis = SYMB_GLOBAL;
        else
            symTab[i].symbVis = SYMB_LOCAL;
    }

    as->symTable = symTab;
    as->symTabSz = symTabSz;
}

static void buildUserAndRelSectionHeaders(AsmEncoder *as)
{
    uint32_t userSectsBodyOffset = sizeof(BinformatHeader) + sizeof(SectionHeader) * getSectionsCount(as);
    uint32_t relocSectsBodyOffset = userSectsBodyOffset + getUserDefinedSectionsSummarySize(as);

    as->usrSectionCount = as->parser->sectionsSz;
    as->usrSectHdrs = (SectionHeader *)calloc(as->usrSectionCount, sizeof(SectionHeader));

    as->relocSectionCount = getRelocSectsCount(as);
    as->relocSectHdrs = (SectionHeader *)calloc(as->relocSectionCount, sizeof(SectionHeader));

    size_t usrSectIdx = 0;
    size_t relocSectIdx = 0;
    for (; usrSectIdx < as->usrSectionCount; usrSectIdx++)
    {
        SectionHeader *currUsrSect = &as->usrSectHdrs[usrSectIdx];
        sectionNode *currUsrSectNode = &as->parser->sections[usrSectIdx];

        currUsrSect->offset = userSectsBodyOffset;
        currUsrSect->size = (uint32_t)currUsrSectNode->currOffset;
        currUsrSect->type = SECT_LOAD;

        currUsrSect->nameIdx = as->strTabSize;
        strTabAdd(as, currUsrSectNode->name, true);

        userSectsBodyOffset += currUsrSect->size;

        uint32_t sectRelocsCount = getRelocationsCountInSection(&as->parser->symsData, currUsrSectNode->name);

        if (sectRelocsCount == 0)
            continue;

        printf("section %s: %d relocations at file offset %d\n",
               currUsrSectNode->name, sectRelocsCount, relocSectsBodyOffset);

        SectionHeader *currRelocSect = &as->relocSectHdrs[relocSectIdx];
        currRelocSect->offset = relocSectsBodyOffset;
        currRelocSect->size = sectRelocsCount * sizeof(RelEntry);
        currRelocSect->type = SECT_REL;

        currRelocSect->nameIdx = as->strTabSize;
        strTabAddCompositeSectName(as, "rel.", currUsrSectNode->name);

        relocSectsBodyOffset += currRelocSect->size;

        relocSectIdx++;
    }

    as->relocSectsBodyEnd = relocSectsBodyOffset;
}

static void buildStrTabAndSymTabHdrs(AsmEncoder *as)
{

    as->symTabHdr = (SectionHeader *)calloc(1, sizeof(SectionHeader));
    as->strTabHdr = (SectionHeader *)calloc(1, sizeof(SectionHeader));

    as->symTabHdr->nameIdx = as->strTabSize;
    strTabAdd(as, "symtab", true);

    as->symTabHdr->offset = as->relocSectsBodyEnd;
    as->symTabHdr->size = as->symTabSz * sizeof(SymTabEntry);
    as->symTabHdr->type = SECT_SYM_TAB;

    as->strTabHdr->nameIdx = as->strTabSize;
    strTabAdd(as, "strtab", true);

    as->strTabHdr->offset = as->relocSectsBodyEnd + as->symTabHdr->size;
    as->strTabHdr->size = as->strTabSize;
    as->strTabHdr->type = SECT_STR_TAB;
}

static void writeStrTab(AsmEncoder *as)
{
    printf("writing strtab at offset: %d ; size: %ld\n", ftell(as->out), sizeof(char) * as->strTabSize);
    fwrite(as->strTab, sizeof(char), as->strTabSize, as->out);
}

static void writeSymTab(AsmEncoder *as)
{
    printf("writing symtab at offset: %d ; size: %ld\n", ftell(as->out), sizeof(SymTabEntry) * as->symTabSz);
    fwrite(as->symTable, sizeof(SymTabEntry), as->symTabSz, as->out);
}

static EncErrCode writeUserSection(sectionNode *sect, FILE *out)
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

static void writeUserSectionsBody(AsmEncoder *as)
{

    for (size_t i = 0; i < as->usrSectionCount; i++)
        writeUserSection(&as->parser->sections[i], as->out);
}

static void writeSectHdrs(AsmEncoder *as)
{
    fwrite(as->usrSectHdrs, sizeof(SectionHeader), as->usrSectionCount, as->out);

    fwrite(as->relocSectHdrs, sizeof(SectionHeader), as->relocSectionCount, as->out);

    fwrite(as->symTabHdr, sizeof(SectionHeader), 1, as->out);

    fwrite(as->strTabHdr, sizeof(SectionHeader), 1, as->out);
}

static uint32_t getSymbolIdx(AsmEncoder *as, const char *symbolName)
{

    for (uint32_t i = 0; i < as->symTabSz; i++)
    {
        symbolData *currSymb = as->parser->symsData.symTab[i];
        if (strcmp(currSymb->name, symbolName) == 0)
            return i;
    }

    return 0;
}

static void writeRelocForSection(AsmEncoder *as, const char *sectName)
{
    for (size_t i = 0; i < as->parser->symsData.symbolsSz; i++)
    {
        symbolData *currSymb = &as->parser->symsData.symbols[i];
        if (currSymb->defined)
            continue;

        if (strcmp(currSymb->sectionName, sectName) != 0)
            continue;

        RelEntry rel = {0};
        rel.offset = currSymb->val;
        rel.symbolIdx = getSymbolIdx(as, currSymb->name);

        long offset = ftell(as->out);

        fwrite(&rel, sizeof(RelEntry), 1, as->out);
        printf("added relocation for section %s symbol: %s at file offset %d\n",
               sectName, currSymb->name, offset);
    }
}

static void writeRelocSectionsBody(AsmEncoder *as)
{
    for (size_t i = 0; i < as->usrSectionCount; i++)
    {

        if (!sectionHasRelocs(&as->parser->symsData, as->parser->sections[i].name))
            continue;

        writeRelocForSection(as, as->parser->sections[i].name);
    }
}

static EncErrCode writeFileHeader(AsmEncoder *as)
{

    as->header.fileType = BIN_LINKABLE;
    as->header.magic = binMagicHeader;
    as->header.version = binFormatVersion;

    as->header.sectionsCount = getSectionsCount(as);
    as->header.stringTableIdx = as->header.sectionsCount - 1;
    as->header.symbolTableIdx = as->header.sectionsCount - 2;

    if (fwrite(&as->header, sizeof(as->header), 1, as->out) == 0)
        return ENC_SYSTEM_ERROR;

    return ENC_OK;
}

EncErrCode GenObjectFile(AsmEncoder *as)
{
    assert(as != NULL);

    as->strTab = (char *)calloc(32, sizeof(char));
    as->strTabCapacity = 32;

    buildUserAndRelSectionHeaders(as);
    buildSymTab(as); // TODO: need large refactoring
    buildStrTabAndSymTabHdrs(as);

    writeFileHeader(as);
    writeSectHdrs(as);
    writeUserSectionsBody(as);
    writeRelocSectionsBody(as);
    writeSymTab(as);
    writeStrTab(as);

    return ENC_OK;
}

void AsmEncoderFree(AsmEncoder *as)
{

    free(as->strTab);
    free(as->symTabHdr);
    free(as->strTabHdr);
    free(as->usrSectHdrs);
    free(as->relocSectHdrs);
    free(as->symTable);
}
