#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "utils.hpp"
#include "binformat.hpp"
#include "ld.hpp"

static int readLinkableFileHeader(LinkableFile *f, fileInfo *in) // TODO create separate library to use here and in readobj
{

    if (getObjFileHeader(in->file, &f->fileHdr) < 0)
    {
        perror("ld");
        return -1;
    }

    if (f->fileHdr.magic != binMagicHeader)
    {
        fprintf(stderr, "%s: bad magic header: %x\n", in->name, f->fileHdr.magic);
        return -1;
    }
    if (f->fileHdr.version != binFormatVersion)
    {
        fprintf(stderr, "%s: insufficient binary file version: %d\n", in->name, f->fileHdr.version);
        return -1;
    }
    if (f->fileHdr.fileType != BIN_LINKABLE)
    {
        fprintf(stderr, "%s: need linkable bin file type\n", in->name);
        return -1;
    }

    return 0;
}

static int readSymbolTable(LinkableFile *f, fileInfo *in)
{
    SectionHeader *symTabHdr = &f->sectHdrs[f->fileHdr.symbolTableIdx];
    f->symTabSz = symTabHdr->size / (uint32_t)sizeof(SymTabEntry);

    f->symTable = (SymTabEntry *)calloc(f->symTabSz, sizeof(SymTabEntry));
    if (f->symTable == NULL)
        return -1;

    fseek(in->file, symTabHdr->offset, SEEK_SET);

    if (fread(f->symTable, sizeof(SymTabEntry), f->symTabSz, in->file) != f->symTabSz)
        return -1;

    return 0;
}

static int readStringTable(LinkableFile *f, fileInfo *in)
{

    SectionHeader *strTabHdr = &f->sectHdrs[f->fileHdr.stringTableIdx];
    f->strTableSz = strTabHdr->size;

    f->strTable = (char *)calloc(strTabHdr->size, sizeof(char));
    if (f->strTable == NULL)
        return -1;

    fseek(in->file, strTabHdr->offset, SEEK_SET);

    if (fread(f->strTable, strTabHdr->size, sizeof(char), in->file) == 0)
        return -1;

    return 0;
}

static int findLoadableSectForRelocSect(LinkableFile *f, RelSection *relSect)
{

    const char *relSectName = getNameFromStrTableByIdx(f, f->sectHdrs[relSect->relSectHeaderIdx].nameIdx);
    if (relSectName == NULL)
    {
        fprintf(stderr, "failed to find section %u name", f->sectHdrs[relSect->relSectHeaderIdx].nameIdx);
        return -1;
    }

    relSectName += strlen("rel.");

    for (uint32_t i = 0; i < f->sectHdrsSz; i++)
    {
        SectionHeader *currHdr = &f->sectHdrs[i];
        if (currHdr->type != SECT_LOAD)
            continue;

        const char *loadSectName = getNameFromStrTableByIdx(f, currHdr->nameIdx);
        if (loadSectName == NULL)
        {
            fprintf(stderr, "failed to find section %u name\n", currHdr->nameIdx);
            return -1;
        }

        if (strcmp(relSectName, loadSectName) != 0)
            continue;

        relSect->loadableSectHeaderIdx = i;

        return 0;
    }

    fprintf(stderr, "failed to find loadable section for reloc section %s\n", relSectName);

    return -1;
}

static int readRelocSection(LinkableFile *f, RelSection *relSect, fileInfo *in)
{
    SectionHeader *sectHdr = &f->sectHdrs[relSect->relSectHeaderIdx];
    fseek(in->file, sectHdr->offset, SEEK_SET);

    relSect->relSectSz = sectHdr->size / (uint32_t)sizeof(RelEntry);
    relSect->relSectEnries = (RelEntry *)calloc(relSect->relSectSz, sizeof(RelEntry));

    if (fread(relSect->relSectEnries, sizeof(RelEntry), relSect->relSectSz, in->file) != relSect->relSectSz)
        return -1;

    if (findLoadableSectForRelocSect(f, relSect) < 0)
        return -1;

    return 0;
}

static int readRelocSections(LinkableFile *f, fileInfo *in)
{
    for (uint32_t i = 0; i < f->sectHdrsSz; i++)
    {
        if (f->sectHdrs[i].type != SECT_REL)
            continue;

        f->relSectionsSz++;
    }

    if (f->relSectionsSz == 0)
        return 0;

    f->relSections = (RelSection *)calloc(f->relSectionsSz, sizeof(RelSection));
    if (f->relSections == NULL)
        return -1;

    uint32_t relSectIdx = 0;
    for (uint32_t i = 0; i < f->sectHdrsSz; i++)
    {
        if (f->sectHdrs[i].type != SECT_REL)
            continue;

        f->relSections[relSectIdx].relSectHeaderIdx = i;

        if (readRelocSection(f, &f->relSections[relSectIdx], in) < 0)
            return -1;

        relSectIdx++;
    }

    return 0;
}

static int readFileData(LinkableFile *f, fileInfo *in)
{
    if (readLinkableFileHeader(f, in) < 0)
        return -1;

    f->sectHdrs = getSectionHeaders(in->file, f->fileHdr.sectionsCount);
    if (f->sectHdrs == NULL)
    {
        perror("ld");
        return -1;
    }
    f->sectHdrsSz = f->fileHdr.sectionsCount;

    if (readSymbolTable(f, in) < 0)
    {
        perror("ld");
        return -1;
    }

    if (readStringTable(f, in) < 0)
    {
        perror("ld");
        return -1;
    }

    if (readRelocSections(f, in) < 0)
        return -1;

    return 0;
}

int ReadFilesData(LD *ld)
{
    ld->files = (LinkableFile *)calloc(ld->args->filesCount, sizeof(LinkableFile));
    if (ld->files == 0)
    {
        perror("ld");
        return -1;
    }

    for (size_t i = 0; i < ld->args->filesCount; i++)
    {
        if (readFileData(&ld->files[i], &ld->args->files[i]) < 0)
            return -1;
    }

    return 0;
}

static void freeExecFile(ExecutableFile *exec)
{

    free(exec->sectHdrs);
    free(exec->strTable);
    free(exec->symTable);
}

static void freeLinkableFile(LinkableFile *l)
{
    for (size_t i = 0; i < l->relSectionsSz; i++)
    {
        free(l->relSections[i].relSectEnries);
    }

    free(l->relSections);
    free(l->sectHdrs);
    free(l->strTable);
    free(l->symTable);
}

static void freeCmdArgs(cmdArgs *args)
{

    fclose(args->out);

    for (size_t i = 0; i < args->filesCount; i++)
    {
        fclose(args->files[i].file);
    }
}

void FreeLD(LD *ld)
{
    freeExecFile(&ld->execFile);

    for (size_t i = 0; i < ld->args->filesCount; i++)
    {
        freeLinkableFile(&ld->files[i]);
    }

    free(ld->files);

    freeCmdArgs(ld->args);
}
