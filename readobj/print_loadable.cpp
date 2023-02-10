#include "print_loadable.hpp"

#include <stdlib.h>
#include <string.h>

#include "decode.hpp"
#include "readobj.hpp"
#include "registers.hpp"

static const char* dataJmpTypeToPostfix(InstructionName instr, JumpType jmpT)
{
    if (strcmp(instr, "jmp") != 0) /* TODO !!! check VM files argument.hpp and
                                      instructions.hpp; Add validation for
                                      DataSize, JmpType and SignExtend fields */
        return NULL;

#define JMP_POSTFIX(pfix, jmpType)                                                                 \
    if (jmpT == jmpType)                                                                           \
        return #pfix;

#include "jmpTypePostfix.inc"

#undef JMP_POSTFIX

    return NULL;
}

static const char* dataSzAndSignToPostfix(InstructionName instr, DataSize sz, SignExtend sign)
{
    if (strcmp(instr, "st") != 0 && strcmp(instr, "ld") != 0 && strcmp(instr, "mov") != 0 &&
        strcmp(instr, "pop") != 0 &&
        strcmp(instr, "push") != 0) /* TODO !!! check VM files argument.hpp and instructions.hpp;
                                    Add validation for DataSize and SignExtend fields */

        return NULL;

#define DATA_POSTFIX(pfix, dataSz, signExt)                                                        \
    if (sz == dataSz && sign == signExt)                                                           \
        return #pfix;

#include "cmdDataPostfixes.inc"

#undef DATA_POSTFIX

    return NULL;
}

static const char* getInstrPostfix(Instruction* instr)
{
    const char* pfix = dataJmpTypeToPostfix(instr->im->Name, instr->JmpType);

    if (pfix == NULL)
        pfix = dataSzAndSignToPostfix(instr->im->Name, instr->DataSz, instr->SignExt);

    return pfix;
}

static int readSymbolTable(ReadObj* r)
{
    r->symTabSz = r->sectHdrs[r->fileHdr.symbolTableIdx].size / (uint32_t)sizeof(SymTabEntry);
    r->symTable = (SymTabEntry*)calloc(r->symTabSz, sizeof(SymTabEntry));
    if (r->symTable == NULL)
        return -1;

    fseek(r->in, r->sectHdrs[r->fileHdr.symbolTableIdx].offset, SEEK_SET);

    if (fread(r->symTable, sizeof(SymTabEntry), r->symTabSz, r->in) != r->symTabSz)
        return -1;

    return 0;
}

static void printBareData(ReadObj* r, SectionHeader* hdr, uint32_t offset)
{
    uint8_t bytesInRow = 16; // TODO to config

    uint8_t currInRow = 0;

    fprintf(r->out, "%u:\t", offset);
    for (size_t i = 0; i < hdr->size; i++)
    {
        uint8_t b = 0;
        fread(&b, 1, 1, r->in);
        fprintf(r->out, "0x%02X  ", b);
        currInRow++;
        if (currInRow == bytesInRow)
        {
            fprintf(r->out, "\n");
            offset += bytesInRow;
            fprintf(r->out, "%u:\t", offset);
            currInRow = 0;
        }
    }
    fprintf(r->out, "\n");
}

static uint32_t findSymbolNameForInstructionInLinkableFile(ReadObj* r, uint32_t instrOffset,
                                                           uint8_t instrSz, bool* ok)
{
    for (uint32_t i = 0; i < r->currRelSectSz; i++)
    {
        RelEntry* currEntry = &r->currRelSect[i];

        if (currEntry->offset <= instrOffset)
            continue;

        if (currEntry->offset - instrOffset < instrSz)
        {
            *ok = true;
            return r->symTable[currEntry->symbolIdx].nameIdx;
        }
    }

    *ok = false;
    return 0;
}

static uint32_t findSymbolNameForInstructionInExecFile(ReadObj* r, Instruction* instr, bool* ok)
{
    uint64_t val = 0;
    if (instr->Arg1.Type == ArgImm || instr->Arg1.Type == ArgImmIndirect)
        val = instr->Arg1.Imm;
    else if (instr->Arg2.Type == ArgImm || instr->Arg2.Type == ArgImmIndirect)
        val = instr->Arg2.Imm;
    else
    {
        *ok = false;
        return 0;
    }

    for (uint32_t i = 0; i < r->symTabSz; i++)
    {
        SymTabEntry* currSymb = &r->symTable[i];

        if (currSymb->value == val)
        {
            *ok = true;
            return currSymb->nameIdx;
        }
    }

    *ok = false;
    return 0;
}

static uint32_t findDefinedSymbolAtOffset(ReadObj* r, uint16_t hdrIdx, uint32_t offset, bool* ok)
{
    for (uint32_t i = 0; i < r->symTabSz; i++)
    {
        SymTabEntry* currSymb = &r->symTable[i];

        if (currSymb->sectHeaderIdx != hdrIdx)
            continue;

        if (currSymb->value - r->sectHdrs[hdrIdx].addr != offset)
            continue;

        *ok = true;
        return currSymb->nameIdx;
    }

    *ok = false;
    return 0;
}

static bool printRegisterArg(ReadObj* r, Argument* arg)
{
    if (arg->Type != ArgRegister && arg->Type != ArgRegisterIndirect &&
        arg->Type != ArgRegisterOffsetIndirect)
        return false;

    RegName regName = FindRegByCode(arg->RegNum);

    if (arg->Type == ArgRegister)
        fprintf(r->out, "%s", regName);
    if (arg->Type == ArgRegisterIndirect)
        fprintf(r->out, "[%s]", regName);
    if (arg->Type == ArgRegisterOffsetIndirect)
        arg->ImmDisp16 >= 0 ? fprintf(r->out, "[%s+%d]", regName, arg->ImmDisp16)
                            : fprintf(r->out, "[%s%d]", regName, arg->ImmDisp16);

    return true;
}

static int printImmArg(ReadObj* r, Argument* arg, uint32_t symbNameIdx, bool isSymbol)
{
    if (arg->Type != ArgImm && arg->Type != ArgImmIndirect)
        return false;

    const char* symbName = getNameFromStrTable(r, symbNameIdx);
    if (symbName == NULL)
    {
        fprintf(stderr, "\tfailed to get symbol name for idx: %u\n", symbNameIdx);
        return -1;
    }

    if (arg->Type == ArgImm)
        isSymbol ? fprintf(r->out, "%s", symbName) : fprintf(r->out, "%llu", arg->Imm);
    if (arg->Type == ArgImmIndirect)
        isSymbol ? fprintf(r->out, "[%s]", symbName) : fprintf(r->out, "[%llu]", arg->Imm);

    return 0;
}

static int disasmInstruction(ReadObj* r, uint16_t sectHdrIdx, Instruction* instr,
                             uint32_t instrOffset, uint8_t instrSz)
{
    bool     isLabel      = false;
    uint32_t labelNameIdx = findDefinedSymbolAtOffset(r, sectHdrIdx, instrOffset, &isLabel);
    if (isLabel)
    {
        const char* labelName = getNameFromStrTable(r, labelNameIdx);
        if (labelName == NULL)
        {
            fprintf(stderr, "\tfailed to get label name for idx: %u\n", labelNameIdx);
            return -1;
        }

        fprintf(r->out, "%s:\n", labelName);
    }

    InstructionName instrName    = instr->im->Name;
    const char*     instrPostfix = getInstrPostfix(instr);

    fprintf(r->out, "%u:  %s", instrOffset, instrName);
    if (instrPostfix != NULL)
        fprintf(r->out, "(%s)", instrPostfix);

    if (instr->Arg1.Type == ArgNone)
    {
        fprintf(r->out, "\n");
        return 0;
    }

    fprintf(r->out, " ");

    bool     isSymbol    = false;
    uint32_t symbNameIdx = 0;

    if (r->fileHdr.fileType == BIN_LINKABLE)
        symbNameIdx =
            findSymbolNameForInstructionInLinkableFile(r, instrOffset, instrSz, &isSymbol);
    else
        symbNameIdx = findSymbolNameForInstructionInExecFile(r, instr, &isSymbol);

    if (!printRegisterArg(r, &instr->Arg1))
        if (printImmArg(r, &instr->Arg1, symbNameIdx, isSymbol) < 0)
            return -1;

    if (instr->Arg2.Type == ArgNone)
    {
        fprintf(r->out, "\n");
        return 0;
    }

    fprintf(r->out, ", ");

    if (!printRegisterArg(r, &instr->Arg2))
        if (printImmArg(r, &instr->Arg2, symbNameIdx, isSymbol) < 0)
            return -1;

    fprintf(r->out, "\n");

    return 0;
}

static int printLoadableSection(ReadObj* r, SectionHeader* hdr, uint16_t sectHdrIdx)
{
    const char* sectName = getNameFromStrTable(r, hdr->nameIdx);

    getSectionRelocations(r, hdr);

    fprintf(r->out, "\nLoadable section \"%s\" at address: %llu: \n", sectName, hdr->addr);

    long offset = 0;
    while (offset < hdr->size)
    {
        Instruction      instr = {0};
        InstrCreationErr err   = Decode(&instr, r->in);
        if (err != INSTR_OK)
        {
            fseek(r->in, hdr->offset + offset, SEEK_SET);

            printBareData(r, hdr, (uint32_t)offset);

            free(r->currRelSect);
            r->currRelSectSz = 0;
            return 0;
        }

        long instrSz = ftell(r->in) - (hdr->offset + offset);

        if (disasmInstruction(r, sectHdrIdx, &instr, (uint32_t)offset, (uint8_t)instrSz) < 0)
        {
            free(r->currRelSect);
            return -1;
        }

        offset += instrSz;
    }

    if (offset != hdr->size)
    {
        fprintf(stderr,
                "wrong section size: instruction decoding overflows section "
                "\"%s\"\n",
                sectName);
        free(r->currRelSect);
        return -1;
    }

    free(r->currRelSect);
    r->currRelSectSz = 0;

    return 0;
}

int printLoadableSections(ReadObj* r)
{
    if (readSymbolTable(r) < 0)
        return -1;

    for (uint16_t i = 0; i < r->fileHdr.sectionsCount; i++)
    {
        if (r->sectHdrs[i].type != SECT_LOAD)
            continue;

        fseek(r->in, r->sectHdrs[i].offset, SEEK_SET);

        if (printLoadableSection(r, &r->sectHdrs[i], i) < 0)
            return -1;
    }

    return 0;
}
