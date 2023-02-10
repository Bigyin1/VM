#include "vm.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "instructions.hpp"
#include "registers.hpp"
#include "run.hpp"
#include "vmconfig/config.hpp"

#define REPORT_TEST_START printf("Test: %s: ", __FUNCTION__)

typedef bool (*vmTest)(CPU* cpu);

static bool vmTestWrapper(vmTest test)
{
    CPU cpu = {0};

    vmConfig.attachConsole = false;

    if (InitVM(&cpu, &vmConfig) < 0)
        return false;

    bool testStat = test(&cpu);

    testStat ? printf("SUCCESS\n") : printf("FAILED\n");

    DestructVM(&cpu, &vmConfig);

    return testStat;
}

static bool testMOV_Register(CPU* cpu)
{
    REPORT_TEST_START;
    uint64_t desiredVal = 129;
    cpu->gpRegs[R5]     = desiredVal;

    Instruction instr = {
        .im = &instructions[ins_mov],
        .Arg1 =
            Argument{
                .Type   = ArgRegister,
                .RegNum = R0,
            },
        .Arg2 =
            Argument{
                .Type   = ArgRegister,
                .RegNum = R5,
            },
    };

    if (Run(cpu, &instr) < 0)
        return false;

    if (cpu->gpRegs[R0] != desiredVal)
        return false;

    return true;
}

static bool testMOV_Imm(CPU* cpu)
{
    REPORT_TEST_START;
    uint64_t desiredVal = 477;

    // test 1
    Instruction instr = {
        .im = &instructions[ins_mov],
        .Arg1 =
            Argument{
                .Type   = ArgRegister,
                .RegNum = R11,
            },
        .Arg2 =
            Argument{
                .Type = ArgImm,
                .Imm  = desiredVal,
            },

    };

    if (Run(cpu, &instr) < 0)
        return false;

    if (cpu->gpRegs[R11] != desiredVal)
        return false;

    // test 2
    desiredVal     = INT64_MAX;
    instr.Arg2.Imm = desiredVal;

    if (Run(cpu, &instr) < 0)
        return false;

    if (cpu->gpRegs[R11] != desiredVal)
        return false;

    return true;
}

static bool testMOV_ImmSignExtend(CPU* cpu)
{
    REPORT_TEST_START;

    uint64_t desiredVal = 477;

    // test 1
    Instruction instr = {
        .im = &instructions[ins_mov],
        .Arg1 =
            Argument{
                .Type   = ArgRegister,
                .RegNum = R10,
            },
        .Arg2 =
            Argument{
                .Type      = ArgImm,
                .Imm       = 477,
                ._immArgSz = DataDByte,
            },
        .SignExt = SignExtended,
    };

    if (Run(cpu, &instr) < 0)
        return false;

    if (cpu->gpRegs[R10] != desiredVal)
        return false;

    // test 2
    desiredVal           = (uint64_t)-1;
    instr.Arg2._immArgSz = DataByte;
    instr.Arg2.Imm       = uint8_t(desiredVal);

    if (Run(cpu, &instr) < 0)
        return false;

    if (cpu->gpRegs[R10] != desiredVal)
        return false;

    // test 3
    desiredVal           = (uint64_t)(INT16_MIN + 1968);
    instr.Arg2._immArgSz = DataDByte;
    instr.Arg2.Imm       = uint16_t(desiredVal);

    if (Run(cpu, &instr) < 0)
        return false;

    if (cpu->gpRegs[R10] != desiredVal)
        return false;

    // test 4
    desiredVal           = (uint64_t)(INT32_MIN + 968);
    instr.Arg2._immArgSz = DataHalfWord;
    instr.Arg2.Imm       = uint32_t(desiredVal);

    if (Run(cpu, &instr) < 0)
        return false;

    if (cpu->gpRegs[R10] != desiredVal)
        return false;

    return true;
}

static bool testLD_ImmInd(CPU* cpu)
{
    REPORT_TEST_START;

    uint64_t desiredVal = UINT64_MAX;

    Instruction instr = {
        .im = &instructions[ins_ld],
        .Arg1 =
            Argument{
                .Type   = ArgRegister,
                .RegNum = R0,
            },
        .Arg2 =
            Argument{
                .Type = ArgImmIndirect,
                .Imm  = 5000,
            },
        .DataSz = DataWord,
    };

    Device* dev = FindDevice(cpu->devices, cpu->gpRegs[RSP]);
    if (dev == NULL)
        return false;

    if (dev->writeTo(dev->concreteDevice, instr.Arg2.Imm - dev->lowAddr, desiredVal, instr.DataSz) <
        0)
        return false;

    if (Run(cpu, &instr) < 0)
        return false;

    if (cpu->gpRegs[R0] != desiredVal)
        return false;

    return true;
}

static bool testLD_RegInd(CPU* cpu)
{
    REPORT_TEST_START;

    uint64_t desiredVal = UINT16_MAX - 229;

    cpu->gpRegs[RSP] = 8000;

    Instruction instr = {
        .im = &instructions[ins_ld],
        .Arg1 =
            Argument{
                .Type   = ArgRegister,
                .RegNum = R0,
            },
        .Arg2 =
            Argument{
                .Type   = ArgRegisterIndirect,
                .RegNum = RSP,
            },
        .DataSz = DataDByte,
    };

    Device* dev = FindDevice(cpu->devices, cpu->gpRegs[RSP]);
    if (dev == NULL)
        return false;

    if (dev->writeTo(dev->concreteDevice, cpu->gpRegs[RSP] - dev->lowAddr, desiredVal,
                     instr.DataSz) < 0)
        return false;

    if (Run(cpu, &instr) < 0)
        return false;

    if (cpu->gpRegs[R0] != desiredVal)
        return false;

    return true;
}

static bool testST_ImmInd(CPU* cpu)
{
    REPORT_TEST_START;

    uint64_t desiredVal = UINT8_MAX - 17;

    cpu->gpRegs[R0] = desiredVal;

    Instruction instr = {
        .im = &instructions[ins_st],
        .Arg1 =
            Argument{
                .Type   = ArgRegister,
                .RegNum = R0,
            },
        .Arg2 =
            Argument{
                .Type = ArgImmIndirect,
                .Imm  = 5000,
            },
        .DataSz = DataByte,
    };

    if (Run(cpu, &instr) < 0)
        return false;

    Device* dev = FindDevice(cpu->devices, instr.Arg2.Imm);
    if (dev == NULL)
        return false;

    if (dev->readFrom(dev->concreteDevice, instr.Arg2.Imm - dev->lowAddr, &desiredVal,
                      instr.DataSz) < 0)
        return false;

    if (cpu->gpRegs[R0] != desiredVal)
        return false;

    return true;
}

static bool testST_RegInd(CPU* cpu)
{
    REPORT_TEST_START;
    uint64_t desiredVal = (uint64_t)(INT32_MIN + 100);

    cpu->gpRegs[R0]   = desiredVal;
    cpu->gpRegs[R5]   = 8000;
    Instruction instr = {
        .im = &instructions[ins_st],
        .Arg1 =
            Argument{
                .Type   = ArgRegister,
                .RegNum = R0,
            },
        .Arg2 =
            Argument{
                .Type   = ArgRegisterIndirect,
                .RegNum = R5,
            },
        .DataSz = DataHalfWord,
    };

    if (Run(cpu, &instr) < 0)
        return false;

    Device* dev = FindDevice(cpu->devices, cpu->gpRegs[R5]);
    if (dev == NULL)
        return false;

    desiredVal = 0;
    if (dev->readFrom(dev->concreteDevice, cpu->gpRegs[R5] - dev->lowAddr, &desiredVal,
                      instr.DataSz) < 0)
        return false;

    if ((uint32_t)cpu->gpRegs[R0] != (uint32_t)desiredVal)
        return false;

    return true;
}

const vmTest tests[] = {
    testMOV_Register, testMOV_Imm,   testMOV_ImmSignExtend,
    testLD_ImmInd,    testLD_RegInd, testST_ImmInd,
    testST_RegInd, // TODO: add more vm tests
};

int main()
{
    bool status = true;

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++)
    {
        if (!vmTestWrapper(tests[i]))
            status = false;
    }

    if (status)
        return EXIT_SUCCESS;

    return EXIT_FAILURE;
}
