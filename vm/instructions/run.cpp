#include <assert.h>
#include <string.h>
#include "instructions.hpp"
#include "registers/registers.hpp"
#include "run.hpp"

static int writeToAddr(CPU *cpu, size_t addr, uint64_t val, DataSize sz)
{

    assert(cpu != NULL);

    Device *dev = FindDevice(cpu, addr);
    if (dev == NULL)
    {
        printf("vm: unmapped address: %zu\n", cpu->regIP);
        return -1;
    }

    FILE *writer = dev->getWriter(dev->concreteDevice, addr - dev->lowAddr);
    if (writer == NULL)
    {
        printf("vm: device %s unable to serve write request at address: %zu\n",
               dev->name, cpu->regIP - dev->lowAddr);
        return -1;
    }

    if (fwrite(&val, DataSzToBytesSz(sz), 1, writer) == 0)
    {
        printf("vm: failed to write %d byte(s) at address %zu(device: %s)\n",
               DataSzToBytesSz(sz), addr, dev->name);
        return -1;
    }

    return 0;
}

static int readFromAddr(CPU *cpu, size_t addr, void *val, DataSize sz)
{

    assert(cpu != NULL);

    Device *dev = FindDevice(cpu, addr);
    if (dev == NULL)
    {
        printf("vm: unmapped address: %zu\n", cpu->regIP);
        return -1;
    }

    FILE *reader = dev->getReader(dev->concreteDevice, addr - dev->lowAddr);
    if (reader == NULL)
    {
        printf("vm: device %s unable to serve read request at address: %zu\n",
               dev->name, cpu->regIP - dev->lowAddr);
        return -1;
    }

    if (fread(val, DataSzToBytesSz(sz), 1, reader) == 0)
    {
        printf("vm: failed to read %d byte(s) from address %zu(device: %s)\n",
               DataSzToBytesSz(sz), addr, dev->name);
        return -1;
    }

    return 0;
}

int runRET(CPU *cpu, Instruction *ins)
{
    assert(cpu != NULL);
    assert(ins != NULL);

    if (cpu->gpRegs[RSP] < 8)
    {
        printf("vm: ret: stack pointer in invalid state\n");
        return -1;
    }

    cpu->gpRegs[RSP] -= 8;

    if (readFromAddr(cpu, cpu->gpRegs[RSP], &cpu->regIP, DataWord) < 0)
        return -1;

    return 0;
}

static size_t getEffectiveAddress(CPU *cpu, Argument *arg)
{

    size_t addr = 0;
    switch (arg->Type)
    {
    case ArgRegisterIndirect:
        addr = cpu->gpRegs[arg->RegNum];
        break;

    case ArgImmIndirect:
        addr = arg->Imm;
        break;

    case ArgRegisterOffsetIndirect:
        addr = cpu->gpRegs[arg->RegNum] + arg->ImmDisp16;
        break;

    case ArgImmOffsetIndirect:
        addr = arg->Imm + arg->ImmDisp16;
        break;

    default:
        return 0;
    }

    return addr;
}

static uint64_t signExtendValue(uint64_t val, DataSize sz)
{

    switch (sz)
    {
    case DataWord:
        break;

    case DataByte:
    {
        int8_t v8 = val;
        int64_t v64 = v8;
        return v64;
    }

    case DataDByte:
    {
        int16_t v16 = val;
        int64_t v64 = v16;
        return v64;
    }
    case DataHalfWord:
    {
        int32_t v32 = val;
        int64_t v64 = v32;
        return v64;
    }
    }

    return val;
}

int runLD(CPU *cpu, Instruction *ins)
{
    assert(cpu != NULL);
    assert(ins != NULL);

    size_t addr = getEffectiveAddress(cpu, &ins->Arg2);

    uint64_t val = 0;

    if (readFromAddr(cpu, addr, &val, (DataSize)ins->DataSz) < 0)
        return -1;

    cpu->gpRegs[ins->Arg1.RegNum] = ins->SignExtend ? signExtendValue(val, (DataSize)ins->DataSz) : val;

    return 0;
}

int runST(CPU *cpu, Instruction *ins)
{
    assert(cpu != NULL);
    assert(ins != NULL);

    size_t addr = getEffectiveAddress(cpu, &ins->Arg2);

    if (writeToAddr(cpu, addr, cpu->gpRegs[ins->Arg1.RegNum], (DataSize)ins->DataSz) < 0)
        return -1;

    return 0;
}

int runMOV(CPU *cpu, Instruction *ins)
{
    assert(cpu != NULL);
    assert(ins != NULL);

    uint64_t val = 0;
    if (ins->Arg2.Type == ArgImm)
        val = ins->Arg2.Imm;
    else
        val = cpu->gpRegs[ins->Arg2.RegNum];

    cpu->gpRegs[ins->Arg1.RegNum] = val;

    return 0;
}

int runPUSH(CPU *cpu, Instruction *ins)
{
    assert(cpu != NULL);
    assert(ins != NULL);

    size_t addr = cpu->gpRegs[RSP];

    uint64_t val = 0;
    if (ins->Arg1.Type == ArgImm)
        val = ins->Arg1.Imm;
    else
        val = cpu->gpRegs[ins->Arg1.RegNum];

    if (writeToAddr(cpu, addr, val, (DataSize)ins->DataSz) < 0)
        return -1;

    switch (ins->DataSz)
    {
    case DataWord:
        cpu->gpRegs[RSP] += 8;
        break;

    case DataByte:
        cpu->gpRegs[RSP] += 1;
        break;

    case DataDByte:
        cpu->gpRegs[RSP] += 2;
        break;

    case DataHalfWord:
        cpu->gpRegs[RSP] += 4;
        break;
    }

    return 0;
}

int runPOP(CPU *cpu, Instruction *ins)
{
    assert(cpu != NULL);
    assert(ins != NULL);

    switch (ins->DataSz)
    {
    case DataWord:
        cpu->gpRegs[RSP] -= 8;
        break;

    case DataByte:
        cpu->gpRegs[RSP] -= 1;
        break;

    case DataDByte:
        cpu->gpRegs[RSP] -= 2;
        break;

    case DataHalfWord:
        cpu->gpRegs[RSP] -= 4;
        break;
    }

    size_t addr = cpu->gpRegs[RSP];

    uint64_t val = 0;

    if (readFromAddr(cpu, addr, &val, (DataSize)ins->DataSz) < 0)
        return -1;

    cpu->gpRegs[ins->Arg1.RegNum] = ins->SignExtend ? signExtendValue(val, (DataSize)ins->DataSz) : val;

    return 0;
}

int runADD(CPU *cpu, Instruction *ins)
{

    assert(cpu != NULL);
    assert(ins != NULL);

    uint64_t val = 0;
    if (ins->Arg2.Type == ArgImm)
        val = ins->Arg2.Imm;
    else
        val = cpu->gpRegs[ins->Arg2.RegNum];

    cpu->gpRegs[ins->Arg1.RegNum] += val;

    return 0;
}

int runADDF(CPU *cpu, Instruction *ins)
{

    assert(cpu != NULL);
    assert(ins != NULL);

    double val1 = 0;
    memcpy(&val1, &cpu->gpRegs[ins->Arg1.RegNum], sizeof(double));

    double val2 = 0;
    if (ins->Arg2.Type == ArgImm)
        memcpy(&val2, &ins->Arg2.Imm, sizeof(double));
    else
        memcpy(&val2, &cpu->gpRegs[ins->Arg2.RegNum], sizeof(double));

    val1 += val2;

    memcpy(&cpu->gpRegs[ins->Arg1.RegNum], &val1, sizeof(double));

    return 0;
}

int runSUB(CPU *cpu, Instruction *ins)
{

    assert(cpu != NULL);
    assert(ins != NULL);

    uint64_t val = 0;
    if (ins->Arg2.Type == ArgImm)
        val = ins->Arg2.Imm;
    else
        val = cpu->gpRegs[ins->Arg2.RegNum];

    cpu->gpRegs[ins->Arg1.RegNum] -= val;

    cpu->statusReg = 0;
    if ((int64_t)cpu->gpRegs[ins->Arg1.RegNum] > 0)
        cpu->statusReg = 1;
    else if ((int64_t)cpu->gpRegs[ins->Arg1.RegNum] < 0)
        cpu->statusReg = -1;

    return 0;
}

int runSUBF(CPU *cpu, Instruction *ins)
{

    assert(cpu != NULL);
    assert(ins != NULL);

    double val1 = 0;
    memcpy(&val1, &cpu->gpRegs[ins->Arg1.RegNum], sizeof(double));

    double val2 = 0;
    if (ins->Arg2.Type == ArgImm)
        memcpy(&val2, &ins->Arg2.Imm, sizeof(double));
    else
        memcpy(&val2, &cpu->gpRegs[ins->Arg2.RegNum], sizeof(double));

    val1 -= val2;

    memcpy(&cpu->gpRegs[ins->Arg1.RegNum], &val1, sizeof(double));

    return 0;
}

int runMUL(CPU *cpu, Instruction *ins)
{

    assert(cpu != NULL);
    assert(ins != NULL);

    uint64_t val = 0;
    if (ins->Arg2.Type == ArgImm)
        val = ins->Arg2.Imm;
    else
        val = cpu->gpRegs[ins->Arg2.RegNum];

    cpu->gpRegs[ins->Arg1.RegNum] *= val;

    return 0;
}

int runMULF(CPU *cpu, Instruction *ins)
{

    assert(cpu != NULL);
    assert(ins != NULL);

    double val1 = 0;
    memcpy(&val1, &cpu->gpRegs[ins->Arg1.RegNum], sizeof(double));

    double val2 = 0;
    if (ins->Arg2.Type == ArgImm)
        memcpy(&val2, &ins->Arg2.Imm, sizeof(double));
    else
        memcpy(&val2, &cpu->gpRegs[ins->Arg2.RegNum], sizeof(double));

    val1 *= val2;

    memcpy(&cpu->gpRegs[ins->Arg1.RegNum], &val1, sizeof(double));

    return 0;
}

int runDIV(CPU *cpu, Instruction *ins)
{

    assert(cpu != NULL);
    assert(ins != NULL);

    uint64_t val = 0;
    if (ins->Arg2.Type == ArgImm)
        val = ins->Arg2.Imm;
    else
        val = cpu->gpRegs[ins->Arg2.RegNum];

    cpu->gpRegs[ins->Arg1.RegNum] /= val;

    return 0;
}

int runDIVF(CPU *cpu, Instruction *ins)
{

    assert(cpu != NULL);
    assert(ins != NULL);

    double val1 = 0;
    memcpy(&val1, &cpu->gpRegs[ins->Arg1.RegNum], sizeof(double));

    double val2 = 0;
    if (ins->Arg2.Type == ArgImm)
        memcpy(&val2, &ins->Arg2.Imm, sizeof(double));
    else
        memcpy(&val2, &cpu->gpRegs[ins->Arg2.RegNum], sizeof(double));

    val1 /= val2;

    memcpy(&cpu->gpRegs[ins->Arg1.RegNum], &val1, sizeof(double));

    return 0;
}

int runJMP(CPU *cpu, Instruction *ins)
{

    JumpType jmpType = ins->JmpType;
    switch (jmpType)
    {
    case JumpUncond:
        break;

    case JumpEQ:
        if (cpu->statusReg != 0)
            return 0;
        break;
    case JumpG:
        if (cpu->statusReg <= 0)
            return 0;
        break;
    case JumpGE:
        if (cpu->statusReg < 0)
            return 0;
        break;
    case JumpL:
        if (cpu->statusReg >= 0)
            return 0;
        break;
    case JumpLE:
        if (cpu->statusReg > 0)
            return 0;
        break;
    case JumpNEQ:
        if (cpu->statusReg != 0)
            return 0;
        break;
    }

    size_t addr = 0;
    if (ins->Arg1.Type == ArgRegister)
        addr = cpu->gpRegs[ins->Arg1.RegNum];
    else
        addr = ins->Arg1.Imm;

    cpu->regIP = addr;
    return 0;
}

int runCALL(CPU *cpu, Instruction *ins)
{
    size_t addr = 0;

    if (ins->Arg1.Type == ArgImm)
        addr = ins->Arg1.Imm;
    else if (ins->Arg1.Type == ArgRegister)
        addr = ins->Arg1.RegNum;

    if (writeToAddr(cpu, cpu->gpRegs[RSP], addr, DataWord) < 0)
        return -1;

    cpu->gpRegs[RSP] += 8;

    return 0;
}

int runCMP(CPU *cpu, Instruction *ins)
{

    uint64_t val = 0;
    if (ins->Arg2.Type == ArgImm)
        val = ins->Arg2.Imm;
    else
        val = cpu->gpRegs[ins->Arg2.RegNum];

    int64_t res = cpu->gpRegs[ins->Arg1.RegNum] - val;

    cpu->statusReg = 0;
    if (res > 0)
        cpu->statusReg = 1;
    else if (res < 0)
        cpu->statusReg = -1;

    return 0;
}
