#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "utils.hpp"
#include "link.hpp"

typedef struct outputSect
{
    const char *sectName;
    uint64_t addr;
    uint32_t size;

    uint32_t offset;

    uint32_t nameIdx;

    SectionHeader **sectsHdrs;
    uint32_t sectsCount;

} outputSect;

static void execFileStringTableAdd(ExecutableFile *execFile, const char *str)
{
    uint32_t len = (uint32_t)strlen(str) + 1;
    if (execFile->strTabCap == 0)
    {
        execFile->strTabCap = 32;
        execFile->strTable = (char *)calloc(execFile->strTabCap, sizeof(char));
    }

    if (execFile->strTableSz + len > execFile->strTabCap)
    {
        execFile->strTabCap *= 2;
        execFile->strTable = (char *)realloc(execFile->strTable, sizeof(char) * execFile->strTabCap); // TODO: errors
    }

    strcpy(execFile->strTable + execFile->strTableSz, str);

    execFile->strTableSz += len;
}

static int buildOutputSect(LD *ld, outputSect *outSect, outSectionInfo *newSectInfo, uint16_t idx)
{

    outSect->sectName = newSectInfo->name;
    outSect->addr = newSectInfo->addr;

    outSect->sectsHdrs = (SectionHeader **)calloc(ld->args->filesCount, sizeof(SectionHeader *));
    if (outSect->sectsHdrs == NULL)
        return -1;

    for (size_t i = 0; i < ld->args->filesCount; i++)
    {
        LinkableFile *currFile = &ld->files[i];
        for (size_t j = 0; j < currFile->fileHdr.sectionsCount; j++)
        {
            const char *currSectName = getNameFromStrTableByIdx(currFile, currFile->sectHdrs[j].nameIdx);
            if (currSectName == NULL)
                return -1;

            if (strcmp(outSect->sectName, currSectName) != 0)
                continue;

            outSect->sectsHdrs[i] = &currFile->sectHdrs[j];
            outSect->sectsCount++;

            printf("added section %s from file %s to out section: %s\n",
                   currSectName, ld->args->files[i].name, outSect->sectName);
            break;
        }
    }

    return 0;
}

static outputSect *BuildOutputSections(LD *ld)
{

    outputSect *outSects = (outputSect *)calloc(ld->args->outSectsCount, sizeof(outputSect));
    if (outSects == NULL)
        return NULL;

    for (uint16_t i = 0; i < ld->args->outSectsCount; i++)
    {
        if (buildOutputSect(ld, &outSects[i], &ld->args->outSectInfo[i], i) < 0)
            return NULL;

        if (outSects[i].sectsCount == 0)
        {
            fprintf(stderr, "error: section %s does not exist in provided files\n",
                    ld->args->outSectInfo[i].name); // TODO: maybe change this to warning
            return NULL;
        }
    }

    return outSects;
}

static void setOutputSectionSizes(LD *ld, outputSect *outSect)
{
    uint32_t currSize = 0;
    for (size_t i = 0; i < ld->args->filesCount; i++)
    {
        SectionHeader *currHdr = outSect->sectsHdrs[i];
        if (currHdr == NULL)
            continue;

        currHdr->addr = outSect->addr + currSize;

        currSize += currHdr->size;
    }

    outSect->size = currSize;
    printf("out section %s now has addr: %lu ; size: %u\n",
           outSect->sectName, outSect->addr, outSect->size);
}

static void SetOutputSectionsSizes(LD *ld, outputSect *outSects)
{

    for (uint16_t i = 0; i < ld->args->outSectsCount; i++)
        setOutputSectionSizes(ld, &outSects[i]);
}

static void updateSymbolAddressesInFileSection(LinkableFile *currFile, SectionHeader *currSectHdr)
{

    SymTabEntry *currSymTab = currFile->symTable;
    uint32_t currSymTabSz = currFile->symTabSz;

    for (size_t j = 0; j < currSymTabSz; j++)
    {
        SymTabEntry *currSymb = &currSymTab[j];

        if (currSymb->sectHeaderIdx == SHN_ABS)
            continue;
        if (currSymb->sectHeaderIdx == SHN_UNDEF)
            continue;

        uint32_t currSymbSectionOffset = currFile->sectHdrs[currSymb->sectHeaderIdx].offset;

        if (currSymbSectionOffset == currSectHdr->offset)
            currSymb->value += currSectHdr->addr;
    }
}

static void updateSymbolAddressesInOutSect(LD *ld, outputSect *outSect)
{
    for (size_t i = 0; i < ld->args->filesCount; i++)
    {
        LinkableFile *currFile = &ld->files[i];

        SectionHeader *currFileSectHdr = outSect->sectsHdrs[i];
        if (currFileSectHdr == NULL)
            continue;

        updateSymbolAddressesInFileSection(currFile, currFileSectHdr);
    }
}

static void UpdateSymbolAddresses(LD *ld, outputSect *outSects)
{

    for (uint16_t i = 0; i < ld->args->outSectsCount; i++)
        updateSymbolAddressesInOutSect(ld, &outSects[i]);
}

static void evalAbsSymbCountInFile(LD *ld, LinkableFile *f)
{
    for (uint32_t i = 0; i < f->symTabSz; i++)
    {
        if (f->symTable[i].sectHeaderIdx == SHN_UNDEF)
            continue;

        ld->execFile.symTabSz++;
    }
}

static void EvalExecFileSymbolTableSize(LD *ld)
{

    for (uint16_t i = 0; i < ld->args->filesCount; i++)
        evalAbsSymbCountInFile(ld, &ld->files[i]);
}

static int checkDuplicatesForNewSymbol(ExecutableFile *ex, uint32_t currIdx, const char *symbName)
{
    for (uint32_t i = 0; i < currIdx; i++)
    {
        SymTabEntry *currSym = &ex->symTable[i];
        const char *execSymbName = getNameFromExecFileStringTab(ex, currSym->nameIdx);

        if (strcmp(symbName, execSymbName) == 0)
        {
            fprintf(stderr, "ld: error: found duplicating symbols: %s\n", symbName);
            return -1;
        }
    }

    return 0;
}

static int addNewOutputSymbol(LD *ld, LinkableFile *f, SymTabEntry *oldSymb, uint16_t outSectIdx)
{
    const char *oldSymbName = getNameFromStrTableByIdx(f, oldSymb->nameIdx);

    if (checkDuplicatesForNewSymbol(&ld->execFile, ld->execFile.symTabCurrIdx, oldSymbName) < 0)
        return -1;

    SymTabEntry *currOutExeSymbol = &ld->execFile.symTable[ld->execFile.symTabCurrIdx];
    currOutExeSymbol->sectHeaderIdx = outSectIdx;
    currOutExeSymbol->value = oldSymb->value;

    currOutExeSymbol->nameIdx = ld->execFile.strTableSz;
    execFileStringTableAdd(&ld->execFile, oldSymbName);

    printf("added symbol %s with value %lu", oldSymbName, oldSymb->value);

    ld->execFile.symTabCurrIdx++;

    return 0;
}

static uint16_t getOutputSectHeaderIdxBySectName(outputSect *outSects, uint16_t count, const char *name)
{

    for (uint16_t i = 0; i < count; i++)
    {
        if (strcmp(outSects[i].sectName, name) == 0)
            return i;
    }

    return 0;
}

static int getSymbolsFromFile(LD *ld, uint16_t linkFileIdx, outputSect *outSects)
{

    LinkableFile *f = &ld->files[linkFileIdx];

    for (uint32_t i = 0; i < f->symTabSz; i++)
    {
        if (f->symTable[i].sectHeaderIdx == SHN_UNDEF)
            continue;

        uint16_t outSectIdx = 0;
        const char *outSectName = NULL;
        if (f->symTable[i].sectHeaderIdx == SHN_ABS)
        {
            outSectIdx = SHN_ABS;
            outSectName = "SHN_ABS";
        }
        else
        {
            SectionHeader *symbSectHdr = &f->sectHdrs[f->symTable[i].sectHeaderIdx];

            outSectName = getNameFromStrTableByIdx(f, symbSectHdr->nameIdx);
            outSectIdx = getOutputSectHeaderIdxBySectName(outSects,
                                                          ld->args->outSectsCount,
                                                          outSectName);
        }

        if (addNewOutputSymbol(ld, f, &f->symTable[i], outSectIdx) < 0)
            return -1;

        printf(" from file %s to output section %s\n", ld->args->files[linkFileIdx].name, outSectName);
    }

    return 0;
}

static int BuildExecFileSymbolTable(LD *ld, outputSect *outSects)
{
    ld->execFile.symTable = (SymTabEntry *)calloc(ld->execFile.symTabSz, sizeof(SymTabEntry));
    if (ld->execFile.symTable == NULL)
        return -1;

    for (uint16_t i = 0; i < ld->args->filesCount; i++)
        if (getSymbolsFromFile(ld, i, outSects) < 0)
            return -1;

    return 0;
}

static void BuildOutputExeFileHeader(LD *ld)
{

    ld->execFile.fileHdr.fileType = BIN_EXEC;
    ld->execFile.fileHdr.version = formatVersion;
    ld->execFile.fileHdr.magic = magicHeader;

    ld->execFile.fileHdr.sectionsCount = ld->args->outSectsCount + 2;
    ld->execFile.fileHdr.symbolTableIdx = ld->execFile.fileHdr.sectionsCount - 2;
    ld->execFile.fileHdr.stringTableIdx = ld->execFile.fileHdr.sectionsCount - 1;
}

static void buildStrTabHdr(LD *ld, uint32_t offset)
{
    SectionHeader *strTabHdr = &ld->execFile.sectHdrs[ld->execFile.fileHdr.stringTableIdx];

    strTabHdr->nameIdx = ld->execFile.strTableSz;
    execFileStringTableAdd(&ld->execFile, "strtab");

    strTabHdr->offset = offset;
    strTabHdr->size = sizeof(char) * ld->execFile.strTableSz;
    strTabHdr->type = SECT_STR_TAB;

    printf("strtab at offset %u ; size %u\n", strTabHdr->offset, strTabHdr->size);
}

static uint32_t buildSymbolTabHdr(LD *ld, uint32_t offset)
{
    SectionHeader *symTabHdr = &ld->execFile.sectHdrs[ld->execFile.fileHdr.symbolTableIdx];

    symTabHdr->nameIdx = ld->execFile.strTableSz;
    execFileStringTableAdd(&ld->execFile, "symtab");

    symTabHdr->offset = offset;
    symTabHdr->size = sizeof(SymTabEntry) * ld->execFile.symTabSz;
    symTabHdr->type = SECT_SYM_TAB;

    printf("symtab at offset %u ; size %u\n", symTabHdr->offset, symTabHdr->size);

    return offset + symTabHdr->size;
}

static void BuildExecFileSectionHdrs(LD *ld, outputSect *outSects)
{
    ld->execFile.sectHdrs = (SectionHeader *)calloc(ld->execFile.fileHdr.sectionsCount, sizeof(SectionHeader));

    uint32_t execFileDataOffset = sizeof(BinformatHeader) + ld->execFile.fileHdr.sectionsCount * sizeof(SectionHeader);

    for (uint16_t i = 0; i < ld->args->outSectsCount; i++)
    {
        outSects[i].offset = execFileDataOffset;

        outSects[i].nameIdx = ld->execFile.strTableSz;
        execFileStringTableAdd(&ld->execFile, outSects[i].sectName);

        execFileDataOffset += outSects[i].size;
    }
    for (uint16_t i = 0; i < ld->args->outSectsCount; i++)
    {
        ld->execFile.sectHdrs[i].addr = outSects[i].addr;
        ld->execFile.sectHdrs[i].nameIdx = outSects[i].nameIdx;
        ld->execFile.sectHdrs[i].offset = outSects[i].offset;
        ld->execFile.sectHdrs[i].size = outSects[i].size;
        ld->execFile.sectHdrs[i].type = SECT_LOAD;

        printf("section %s at offset %u ; address: %lu ; size: %u\n",
               getNameFromExecFileStringTab(&ld->execFile, ld->execFile.sectHdrs[i].nameIdx),
               ld->execFile.sectHdrs[i].offset, ld->execFile.sectHdrs[i].addr,
               ld->execFile.sectHdrs[i].size);
    }

    execFileDataOffset = buildSymbolTabHdr(ld, execFileDataOffset);

    buildStrTabHdr(ld, execFileDataOffset);
}

static int getEntrypoint(ExecutableFile *exe)
{
    for (size_t i = 0; i < exe->symTabSz; i++)
    {
        SymTabEntry *currSym = &exe->symTable[i];

        const char *symName = getNameFromExecFileStringTab(exe, currSym->nameIdx);

        if (strcmp("main", symName) == 0) // TODO : make settable
        {
            exe->fileHdr.entrypoint = currSym->value;
            return 0;
        }
    }

    fprintf(stderr, "failed to find enrypoint \"main\\n");

    return -1;
}

static int WriteHdrs(ExecutableFile *exe, FILE *out)
{
    if (getEntrypoint(exe) < 0)
        return -1;

    if (fwrite(&exe->fileHdr, sizeof(BinformatHeader), 1, out) == 0)
    {
        perror("ld");
        return -1;
    }

    if (fwrite(exe->sectHdrs, sizeof(SectionHeader), exe->fileHdr.sectionsCount, out) < exe->fileHdr.sectionsCount)
    {
        perror("ld");
        return -1;
    }

    return 0;
}

static RelSection *findRelSectionForSectHdr(LinkableFile *l, SectionHeader *hdr)
{
    for (uint32_t i = 0; i < l->relSectionsSz; i++)
    {
        RelSection *currRelSect = &l->relSections[i];

        SectionHeader *relocTargetSectHdr = &l->sectHdrs[currRelSect->loadableSectHeaderIdx];

        if (relocTargetSectHdr->offset == hdr->offset)
            return currRelSect;
    }

    return NULL;
}

static SymTabEntry *getExecFileSymTabEntryByName(ExecutableFile *exe, const char *symbName)
{

    for (uint32_t i = 0; i < exe->symTabSz; i++)
    {
        SymTabEntry *curr = &exe->symTable[i];
        const char *exeSymbName = getNameFromExecFileStringTab(exe, curr->nameIdx);

        if (strcmp(exeSymbName, symbName) == 0)
            return curr;
    }

    return NULL;
}

static int relocateSectionData(ExecutableFile *exe, LinkableFile *l, RelSection *rel, char *sectData)
{
    if (rel == NULL)
        return 0;

    for (uint32_t i = 0; i < rel->relSectSz; i++)
    {
        RelEntry *currRelEntry = &rel->relSectEnries[i];

        SymTabEntry *symb = &l->symTable[currRelEntry->symbolIdx];

        const char *symbName = getNameFromStrTableByIdx(l, symb->nameIdx);
        if (symbName == NULL)
        {
            fprintf(stderr, "ld: error: failed to find symbol name for relocation"); // TODO: do somethig with error handling
            return -1;
        }

        SymTabEntry *exeSymbEnt = getExecFileSymTabEntryByName(exe, symbName);
        if (exeSymbEnt == NULL)
        {
            fprintf(stderr, "ld: error: symbol %s is undefined\n", symbName);
            return -1;
        }

        memcpy(sectData + currRelEntry->offset, &exeSymbEnt->value, sizeof(exeSymbEnt->value));
    }

    return 0;
}

static int applyRelocsToOldSection(LD *ld, LinkableFile *l, SectionHeader *hdr, uint32_t fileIdx)
{
    char *sectData = (char *)calloc(hdr->size, sizeof(char));
    if (sectData == NULL)
    {
        perror("ld");
        return -1;
    }

    FILE *currFile = ld->args->files[fileIdx].file;

    fseek(currFile, hdr->offset, SEEK_SET);

    if (fread(sectData, hdr->size, 1, currFile) == 0)
    {
        free(sectData);
        perror("ld");
        return -1;
    }

    RelSection *relocsToApply = findRelSectionForSectHdr(l, hdr);

    if (relocateSectionData(&ld->execFile, l, relocsToApply, sectData) < 0)
    {
        free(sectData);
        return -1;
    }

    if (fwrite(sectData, hdr->size, 1, ld->args->out) == 0)
    {
        free(sectData);
        perror("ld");
        return -1;
    }

    free(sectData);
    return 0;
}

static int applyRelocsToOutputSection(LD *ld, outputSect *outSect)
{

    for (uint16_t i = 0; i < ld->args->filesCount; i++)
    {
        LinkableFile *currFile = &ld->files[i];

        SectionHeader *currFileSectHdr = outSect->sectsHdrs[i];
        if (currFileSectHdr == NULL)
            continue;

        if (applyRelocsToOldSection(ld, currFile, currFileSectHdr, i) < 0)
            return -1;
    }

    return 0;
}

static int ApplyRelocations(LD *ld, outputSect *outSect)
{

    for (uint16_t i = 0; i < ld->args->outSectsCount; i++)
    {
        if (applyRelocsToOutputSection(ld, &outSect[i]) < 0)
            return -1;
    }

    return 0;
}

static int WriteSymTabAndStrTab(ExecutableFile *exe, FILE *out)
{
    if (fwrite(exe->symTable, sizeof(SymTabEntry), exe->symTabSz, out) != exe->symTabSz)
    {
        perror("ld");
        return -1;
    }

    if (fwrite(exe->strTable, sizeof(char), exe->strTableSz, out) != exe->strTableSz)
    {
        perror("ld");
        return -1;
    }

    return 0;
}

static void FreeOutputSects(outputSect *outSects, uint32_t sz)
{
    for (size_t i = 0; i < sz; i++)
    {
        free(outSects[i].sectsHdrs);
    }

    free(outSects);
}

int LinkFiles(LD *ld)
{
    BuildOutputExeFileHeader(ld);

    outputSect *outSects = BuildOutputSections(ld);
    if (outSects == NULL)
    {
        FreeOutputSects(outSects, ld->args->outSectsCount);
        return -1;
    }

    SetOutputSectionsSizes(ld, outSects);

    UpdateSymbolAddresses(ld, outSects);

    EvalExecFileSymbolTableSize(ld);

    if (BuildExecFileSymbolTable(ld, outSects) < 0)
    {
        FreeOutputSects(outSects, ld->args->outSectsCount);
        return -1;
    }

    BuildExecFileSectionHdrs(ld, outSects);

    if (WriteHdrs(&ld->execFile, ld->args->out) < 0)
    {
        FreeOutputSects(outSects, ld->args->outSectsCount);
        return -1;
    }

    if (ApplyRelocations(ld, outSects) < 0)
    {
        FreeOutputSects(outSects, ld->args->outSectsCount);
        return -1;
    }

    if (WriteSymTabAndStrTab(&ld->execFile, ld->args->out) < 0)
    {
        FreeOutputSects(outSects, ld->args->outSectsCount);
        return -1;
    }

    FreeOutputSects(outSects, ld->args->outSectsCount);

    return 0;
}
