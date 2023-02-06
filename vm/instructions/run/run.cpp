#include <assert.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include "instructions.hpp"
#include "registers.hpp"
#include "run.hpp"

static int writeToAddr(CPU *cpu, uint64_t addr, uint64_t val, DataSize sz)
{

    assert(cpu != NULL);

    Device *dev = FindDevice(cpu->devices, addr);
    if (dev == NULL)
    {
        fprintf(stderr, "vm: unmapped address: %zu\n", addr);
        return -1;
    }

    int res = dev->writeTo(dev->concreteDevice, addr - dev->lowAddr, val, sz);
    if (res < 0)
    {
        fprintf(stderr, "vm: device %s unable to serve write request at address: %zu; size: %zu\n",
                dev->name, addr, DataSzToBytesSz(sz));
        return -1;
    }
    return 0;
}

static int readFromAddr(CPU *cpu, uint64_t addr, uint64_t *val, DataSize sz)
{

    assert(cpu != NULL);

    Device *dev = FindDevice(cpu->devices, addr);
    if (dev == NULL)
    {
        fprintf(stderr, "vm: unmapped address: %zu\n", addr);
        return -1;
    }

    int res = dev->readFrom(dev->concreteDevice, addr - dev->lowAddr, val, sz);
    if (res < 0)
    {
        fprintf(stderr, "vm: device %s unable to serve read request at address: %zu; size: %zu\n",
                dev->name, addr, DataSzToBytesSz(sz));
        return -1;
    }

    return 0;
}

static int run_ret(CPU *cpu, Instruction *ins)
{
    assert(cpu != NULL);
    assert(ins != NULL);

    if (cpu->gpRegs[RSP] < 8)
    {
        fprintf(stderr, "vm: ret: stack pointer in invalid state\n");
        return -1;
    }

    cpu->gpRegs[RSP] -= 8;

    if (readFromAddr(cpu, cpu->gpRegs[RSP], &cpu->regIP, DataWord) < 0)
        return -1;

    return 0;
}

static uint64_t getEffectiveAddress(CPU *cpu, Argument *arg)
{

    uint64_t addr = 0;
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

static int run_ld(CPU *cpu, Instruction *ins)
{
    assert(cpu != NULL);
    assert(ins != NULL);

    uint64_t addr = getEffectiveAddress(cpu, &ins->Arg2);

    uint64_t val = 0;

    if (readFromAddr(cpu, addr, &val, ins->DataSz) < 0)
        return -1;

    cpu->gpRegs[ins->Arg1.RegNum] = ins->SignExt ? signExtendValue(val, ins->DataSz) : val;

    return 0;
}

static int run_st(CPU *cpu, Instruction *ins)
{
    assert(cpu != NULL);
    assert(ins != NULL);

    uint64_t addr = getEffectiveAddress(cpu, &ins->Arg2);

    if (writeToAddr(cpu, addr, cpu->gpRegs[ins->Arg1.RegNum], ins->DataSz) < 0)
        return -1;

    return 0;
}

static int run_mov(CPU *cpu, Instruction *ins)
{
    assert(cpu != NULL);
    assert(ins != NULL);

    uint64_t val = 0;
    if (ins->Arg2.Type == ArgImm)
        val = ins->Arg2.Imm;
    else
        val = cpu->gpRegs[ins->Arg2.RegNum];

    cpu->gpRegs[ins->Arg1.RegNum] = ins->SignExt ? signExtendValue(val, ins->Arg2._immArgSz) : val;

    return 0;
}

static int run_push(CPU *cpu, Instruction *ins)
{
    assert(cpu != NULL);
    assert(ins != NULL);

    uint64_t addr = cpu->gpRegs[RSP];

    uint64_t val = 0;
    if (ins->Arg1.Type == ArgImm)
        val = ins->Arg1.Imm;
    else
        val = cpu->gpRegs[ins->Arg1.RegNum];

    if (writeToAddr(cpu, addr, val, ins->DataSz) < 0)
        return -1;

    cpu->gpRegs[RSP] += DataSzToBytesSz(ins->DataSz);

    return 0;
}

static int run_pop(CPU *cpu, Instruction *ins)
{
    assert(cpu != NULL);
    assert(ins != NULL);

    cpu->gpRegs[RSP] -= DataSzToBytesSz(ins->DataSz);

    uint64_t addr = cpu->gpRegs[RSP];

    uint64_t val = 0;

    if (readFromAddr(cpu, addr, &val, ins->DataSz) < 0)
        return -1;

    cpu->gpRegs[ins->Arg1.RegNum] = ins->SignExt ? signExtendValue(val, ins->DataSz) : val;

    return 0;
}

static int runArithm(CPU *cpu, Instruction *ins, void (*op)(CPU *cpu, Instruction *, uint64_t, uint64_t))
{

    uint64_t val = 0;
    if (ins->Arg2.Type == ArgImm)
        val = ins->Arg2.Imm;
    else if (ins->Arg2.Type == ArgRegister)
        val = cpu->gpRegs[ins->Arg2.RegNum];
    else
    {
        uint64_t addr = getEffectiveAddress(cpu, &ins->Arg2);

        if (readFromAddr(cpu, addr, &val, ins->DataSz) < 0)
            return -1;
    }

    op(cpu, ins, cpu->gpRegs[ins->Arg1.RegNum], val);

    return 0;
}

static int runArithmf(CPU *cpu, Instruction *ins, void (*op)(CPU *cpu, Instruction *, double, double))
{

    double op1 = 0;
    memcpy(&op1, &cpu->gpRegs[ins->Arg1.RegNum], sizeof(double));

    double op2 = 0;
    if (ins->Arg2.Type == ArgImm)
        memcpy(&op2, &ins->Arg2.Imm, sizeof(double));
    else if (ins->Arg2.Type == ArgRegister)
        memcpy(&op2, &cpu->gpRegs[ins->Arg2.RegNum], sizeof(double));
    else
    {
        uint64_t addr = getEffectiveAddress(cpu, &ins->Arg2);

        uint64_t val = 0;
        if (readFromAddr(cpu, addr, &val, ins->DataSz) < 0)
            return -1;

        memcpy(&op2, &val, sizeof(double));
    }

    op(cpu, ins, op1, op2);

    return 0;
}

static void opAdd(CPU *cpu, Instruction *ins, uint64_t a1, uint64_t a2)
{
    cpu->gpRegs[ins->Arg1.RegNum] = a1 + a2;
}

static int run_add(CPU *cpu, Instruction *ins)
{

    assert(cpu != NULL);
    assert(ins != NULL);

    return runArithm(cpu, ins, opAdd);
}

static void opAddf(CPU *cpu, Instruction *ins, double op1, double op2)
{
    double res = op1 + op2;

    memcpy(&cpu->gpRegs[ins->Arg1.RegNum], &res, sizeof(double));
}

static int run_addf(CPU *cpu, Instruction *ins)
{

    assert(cpu != NULL);
    assert(ins != NULL);

    return runArithmf(cpu, ins, opAddf);
}

static void opSub(CPU *cpu, Instruction *ins, uint64_t a1, uint64_t a2)
{
    cpu->gpRegs[ins->Arg1.RegNum] = a1 - a2;
}

static int run_sub(CPU *cpu, Instruction *ins)
{

    assert(cpu != NULL);
    assert(ins != NULL);

    return runArithm(cpu, ins, opSub);
}

static void opSubf(CPU *cpu, Instruction *ins, double op1, double op2)
{
    double res = op1 - op2;

    memcpy(&cpu->gpRegs[ins->Arg1.RegNum], &res, sizeof(double));
}

static int run_subf(CPU *cpu, Instruction *ins)
{

    assert(cpu != NULL);
    assert(ins != NULL);

    return runArithmf(cpu, ins, opSubf);
}

static void opMul(CPU *cpu, Instruction *ins, uint64_t a1, uint64_t a2)
{
    cpu->gpRegs[ins->Arg1.RegNum] = a1 * a2;
}

static int run_mul(CPU *cpu, Instruction *ins)
{

    assert(cpu != NULL);
    assert(ins != NULL);

    return runArithm(cpu, ins, opMul);
}

static void opMulf(CPU *cpu, Instruction *ins, double op1, double op2)
{
    double res = op1 * op2;

    memcpy(&cpu->gpRegs[ins->Arg1.RegNum], &res, sizeof(double));
}

static int run_mulf(CPU *cpu, Instruction *ins)
{

    assert(cpu != NULL);
    assert(ins != NULL);

    return runArithmf(cpu, ins, opMulf);
}

static void opDiv(CPU *cpu, Instruction *ins, uint64_t a1, uint64_t a2)
{
    cpu->gpRegs[ins->Arg1.RegNum] = a1 / a2;
}

static int run_div(CPU *cpu, Instruction *ins)
{

    assert(cpu != NULL);
    assert(ins != NULL);

    return runArithm(cpu, ins, opDiv);
}

static void opDivf(CPU *cpu, Instruction *ins, double op1, double op2)
{
    double res = op1 / op2;

    memcpy(&cpu->gpRegs[ins->Arg1.RegNum], &res, sizeof(double));
}

static int run_divf(CPU *cpu, Instruction *ins)
{

    assert(cpu != NULL);
    assert(ins != NULL);

    return runArithmf(cpu, ins, opDivf);
}

static void opSqrt(CPU *cpu, Instruction *ins, double op1, double op2)
{
    double res = sqrt(op2);

    memcpy(&cpu->gpRegs[ins->Arg1.RegNum], &res, sizeof(double));
}

static int run_sqrt(CPU *cpu, Instruction *ins)
{

    assert(cpu != NULL);
    assert(ins != NULL);

    return runArithmf(cpu, ins, opSqrt);
}

static void opCmp(CPU *cpu, Instruction *ins, uint64_t op1, uint64_t op2)
{
    int64_t res = (int64_t)(op1 - op2);

    cpu->statusReg = 0;
    if (res > 0)
        cpu->statusReg = 1;
    else if (res < 0)
        cpu->statusReg = -1;
}

static int run_cmp(CPU *cpu, Instruction *ins)
{
    return runArithm(cpu, ins, opCmp);
}

static void opCmpf(CPU *cpu, Instruction *, double op1, double op2)
{

    cpu->statusReg = 0;

    double diff = op1 - op2;

    const double eps = 1.0e-8;
    if (fabs(diff) <= eps)
    {
        cpu->statusReg = 0;
        return;
    }

    if (diff > 0)
        cpu->statusReg = 1;
    else if (diff < 0)
        cpu->statusReg = -1;
}

static int run_cmpf(CPU *cpu, Instruction *ins)
{

    assert(cpu != NULL);
    assert(ins != NULL);

    return runArithmf(cpu, ins, opCmpf);
}

static int run_jmp(CPU *cpu, Instruction *ins)
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
        if (cpu->statusReg == 0)
            return 0;
        break;
    }

    uint64_t addr = 0;
    if (ins->Arg1.Type == ArgRegister)
        addr = cpu->gpRegs[ins->Arg1.RegNum];
    else
        addr = ins->Arg1.Imm;

    cpu->regIP = addr;
    return 0;
}

static int run_call(CPU *cpu, Instruction *ins)
{
    uint64_t addr = 0;

    if (ins->Arg1.Type == ArgImm)
        addr = ins->Arg1.Imm;
    else if (ins->Arg1.Type == ArgRegister)
        addr = ins->Arg1.RegNum;

    if (writeToAddr(cpu, cpu->gpRegs[RSP], cpu->regIP, DataWord) < 0)
        return -1;

    cpu->gpRegs[RSP] += 8;

    cpu->regIP = addr;

    return 0;
}

static int run_halt(CPU *cpu, Instruction *ins)
{
    assert(cpu != NULL);
    assert(ins != NULL);

    cpu->running = false;

    return 0;
}

typedef int (*RunFunc)(CPU *, Instruction *);

static RunFunc getRunFunc(InstrOpCode opCode)
{
    switch (opCode)
    {

#define INSTR(name, opCode, argSets) \
    case opCode:                     \
        return run_##name;

#include "instructionsMeta.inc"

#undef INSTR

    default:
        break;
    }

    return NULL;
}

int Run(CPU *cpu, Instruction *ins)
{

    return getRunFunc(ins->im->OpCode)(cpu, ins);
}
