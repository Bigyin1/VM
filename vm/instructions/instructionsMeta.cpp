#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "instructionsMeta.hpp"
#include "encode.hpp"
#include "decode.hpp"




const RegMeta regs[15] = {

    {
        .Name = "r0",
        .RegCode = 0,
    },
    {
        .Name = "r1",
        .RegCode = 1,
    },
    {
        .Name = "r2",
        .RegCode = 2,
    },
    {
        .Name = "r3",
        .RegCode = 3,
    },
    {
        .Name = "r4",
        .RegCode = 4,
    },
    {
        .Name = "r5",
        .RegCode = 5,
    },
    {
        .Name = "rbp",
        .RegCode = 14,
    }
};


const InstructionMeta instructions[] = {

    {
        .Name = "ld",
        .OpCode = 1,
        .ArgSets = {
            {.First = ArgRegister, .Second = ArgRegisterIndirect},
            {.First = ArgRegister, .Second = ArgRegisterOffsetIndirect},
            {.First = ArgRegister, .Second = ArgRegisterOffsetRegIndirect},
            {.First = ArgRegister, .Second = ArgImmIndirect},
            {.First = ArgRegister, .Second = ArgImmOffsetIndirect},
        },
        .EncFunc = encodeLD,
        .DecFunc = decodeLD,

    },
    {
        .Name = "st",
        .OpCode = 2,
        .ArgSets = {
            {.First = ArgRegister, .Second = ArgRegisterIndirect},
            {.First = ArgRegister, .Second = ArgRegisterOffsetIndirect},
            {.First = ArgRegister, .Second = ArgRegisterOffsetRegIndirect},
            {.First = ArgRegister, .Second = ArgImmIndirect},
            {.First = ArgRegister, .Second = ArgImmOffsetIndirect},
        },
        .EncFunc = encodeST,
        .DecFunc = decodeST,
    },
    {
        .Name = "mov",
        .OpCode = 3,
        .ArgSets = {
            {.First = ArgRegister, .Second = ArgRegister},
            {.First = ArgRegister, .Second = ArgImm},
        },
        .EncFunc = encodeMOV,
        .DecFunc = decodeMOV,
    },
    {
        .Name = "push",
        .OpCode = 4,
        .ArgSets = {
            {.First = ArgRegister, .Second = ArgNone},
            {.First = ArgImm, .Second = ArgNone},
        },
        .EncFunc = encodePUSH,
        .DecFunc = decodePUSH,
    },
    {
        .Name = "pop",
        .OpCode = 5,
        .ArgSets = {
            {.First = ArgRegister, .Second = ArgNone},
        },
        .EncFunc = encodePOP,
        .DecFunc = decodePOP,
    },
    {
        .Name = "add",
        .OpCode = 6,
        .ArgSets = {
            {.First = ArgRegister, .Second = ArgRegister},
            {.First = ArgRegister, .Second = ArgImm},
        },
        .EncFunc = encodeARITHM,
        .DecFunc = decodeARITHM,
    },
    {
        .Name = "addf",
        .OpCode = 7,
        .ArgSets = {
            {.First = ArgRegister, .Second = ArgRegister},
            {.First = ArgRegister, .Second = ArgImm},
        },
        .EncFunc = encodeARITHMF,
        .DecFunc = decodeARITHMF,
    },
    {
        .Name = "sub",
        .OpCode = 8,
        .ArgSets = {
            {.First = ArgRegister, .Second = ArgRegister},
            {.First = ArgRegister, .Second = ArgImm},
        },
        .EncFunc = encodeARITHM,
        .DecFunc = decodeARITHM,
    },
    {
        .Name = "subf",
        .OpCode = 9,
        .ArgSets = {
            {.First = ArgRegister, .Second = ArgRegister},
            {.First = ArgRegister, .Second = ArgImm},
        },
        .EncFunc = encodeARITHMF,
        .DecFunc = decodeARITHMF,
    },
    {
        .Name = "mul",
        .OpCode = 10,
        .ArgSets = {
            {.First = ArgRegister, .Second = ArgRegister},
            {.First = ArgRegister, .Second = ArgImm},
        },
        .EncFunc = encodeARITHM,
        .DecFunc = decodeARITHM,
    },
    {
        .Name = "mulf",
        .OpCode = 11,
        .ArgSets = {
            {.First = ArgRegister, .Second = ArgRegister},
            {.First = ArgRegister, .Second = ArgImm},
        },
        .EncFunc = encodeARITHMF,
        .DecFunc = decodeARITHMF,
    },
    {
        .Name = "div",
        .OpCode = 12,
        .ArgSets = {
            {.First = ArgRegister, .Second = ArgRegister},
            {.First = ArgRegister, .Second = ArgImm},
        },
        .EncFunc = encodeARITHM,
        .DecFunc = decodeARITHM,
    },
    {
        .Name = "divf",
        .OpCode = 13,
        .ArgSets = {
            {.First = ArgRegister, .Second = ArgRegister},
            {.First = ArgRegister, .Second = ArgImm},
        },
        .EncFunc = encodeARITHMF,
        .DecFunc = decodeARITHMF,
    },
    {
        .Name = "jmp",
        .OpCode = 14,
        .ArgSets = {
            {.First = ArgImm, .Second = ArgNone},
            {.First = ArgRegister, .Second = ArgNone},
            {.First = ArgRegisterIndirect, .Second = ArgNone},
            {.First = ArgImmIndirect, .Second = ArgNone},
        },
        .EncFunc = encodeJMP,
        .DecFunc = decodeJMP,
    },

};



const InstructionMeta *findInsMetaByName(InstructionName name) {
    assert(name != NULL);

    for (size_t i = 0; i < sizeof(instructions)/sizeof(InstructionMeta); i++) {

        if (strcmp(instructions[i].Name, name) == 0)
            return &instructions[i];

    }

    return NULL;
}

const InstructionMeta *findInsMetaByOpCode(uint8_t opCode) {

    for (size_t i = 0; i < sizeof(instructions)/sizeof(InstructionMeta); i++) {

        if (opCode == instructions[i].OpCode)
            return &instructions[i];

    }

    return NULL;
}


int findArgSetIdx(uint8_t opCode, ArgSet args) {

    for (size_t i = 0; i < sizeof(instructions)/sizeof(InstructionMeta); i++) {

        if (opCode == instructions[i].OpCode)
        {
            for (size_t j = 0; i < sizeof(instructions->ArgSets)/sizeof(ArgSet); j++)
            {
                if (instructions[i].ArgSets[j].First == args.First &&
                        instructions[i].ArgSets[j].Second == args.Second)

                    return j;
            }
        }
    }

    return -1;
}


int findRegByName(RegName name) {

    for (size_t i = 0; i < sizeof(regs)/sizeof(RegMeta); i++) {

        if (strcmp(regs[i].Name, name) == 0)
            return i;
    }

    return -1;
}
