#include "instructions.hpp"

Instruction testIns[] = {

    {
        .im        = &instructions[ins_ret],
        .Arg1      = {.Type = ArgNone},
        .Arg2      = {.Type = ArgNone},
        .ArgSetIdx = 0,
    },

    {
        .im        = &instructions[ins_ld],
        .Arg1      = {.Type = ArgRegister, .RegNum = 0},
        .Arg2      = {.Type = ArgImmIndirect, .Imm = 123},
        .ArgSetIdx = 2,
        .DataSz    = DataHalfWord,
        .SignExt   = SignExtended,
    },

    {
        .im        = &instructions[ins_ld],
        .Arg1      = {.Type = ArgRegister, .RegNum = 1},
        .Arg2      = {.Type = ArgImmIndirect, .Imm = 1},
        .ArgSetIdx = 2,
    },
    {
        .im   = &instructions[ins_st],
        .Arg1 = {.Type = ArgRegister, .RegNum = 2},
        .Arg2 =
            {
                .Type      = ArgRegisterOffsetIndirect,
                .RegNum    = 3,
                .ImmDisp16 = 128,
            },
        .ArgSetIdx = 1,
    },
    {
        .im        = &instructions[ins_st],
        .Arg1      = {.Type = ArgRegister, .RegNum = 0},
        .Arg2      = {.Type = ArgRegisterIndirect, .RegNum = 2},
        .ArgSetIdx = 0,
        .DataSz    = DataDByte,
    },
    {
        .im        = &instructions[ins_mov],
        .Arg1      = {.Type = ArgRegister, .RegNum = 0},
        .Arg2      = {.Type = ArgImm, .Imm = (uint64_t)-1, ._immArgSz = DataByte},
        .ArgSetIdx = 1,
    },
    {
        .im        = &instructions[ins_mov],
        .Arg1      = {.Type = ArgRegister, .RegNum = 0},
        .Arg2      = {.Type = ArgImm, .Imm = (uint64_t)-1, ._immArgSz = DataByte},
        .ArgSetIdx = 1,
        .SignExt   = SignExtended,
    },
    {
        .im        = &instructions[ins_mov],
        .Arg1      = {.Type = ArgRegister, .RegNum = 0},
        .Arg2      = {.Type = ArgRegister, .RegNum = 1},
        .ArgSetIdx = 0,
    },
    {
        .im        = &instructions[ins_mov],
        .Arg1      = {.Type = ArgRegister, .RegNum = 1},
        .Arg2      = {.Type = ArgImm, .Imm = 256, ._immArgSz = DataDByte},
        .ArgSetIdx = 1,
    },
    {
        .im        = &instructions[ins_mov],
        .Arg1      = {.Type = ArgRegister, .RegNum = 2},
        .Arg2      = {.Type = ArgImm, .Imm = 4659770375504729735U, ._immArgSz = DataWord},
        .ArgSetIdx = 1,
    },

    {
        .im        = &instructions[ins_push],
        .Arg1      = {.Type = ArgImm, .Imm = 13876515361109783347U, ._immArgSz = DataWord},
        .Arg2      = {.Type = ArgNone},
        .ArgSetIdx = 1,
        .DataSz    = DataWord,
    },
    {
        .im        = &instructions[ins_push],
        .Arg1      = {.Type = ArgRegister, .RegNum = 1},
        .Arg2      = {.Type = ArgNone},
        .ArgSetIdx = 0,
        .DataSz    = DataWord,
    },
    {
        .im        = &instructions[ins_push],
        .Arg1      = {.Type = ArgImm, .Imm = 128, ._immArgSz = DataDByte},
        .Arg2      = {.Type = ArgNone},
        .ArgSetIdx = 1,
        .DataSz    = DataByte,
    },
    {
        .im        = &instructions[ins_pop],
        .Arg1      = {.Type = ArgRegister, .RegNum = 0},
        .Arg2      = {.Type = ArgNone},
        .ArgSetIdx = 0,
        .DataSz    = DataHalfWord,
    },

    {
        .im        = &instructions[ins_add],
        .Arg1      = {.Type = ArgRegister, .RegNum = 0},
        .Arg2      = {.Type = ArgRegister, .RegNum = 1},
        .ArgSetIdx = 0,
    },

    {
        .im        = &instructions[ins_mul],
        .Arg1      = {.Type = ArgRegister, .RegNum = 0},
        .Arg2      = {.Type = ArgImm, .Imm = 3, ._immArgSz = DataByte},
        .ArgSetIdx = 1,
    },
    {
        .im        = &instructions[ins_mulf],
        .Arg1      = {.Type = ArgRegister, .RegNum = 1},
        .Arg2      = {.Type = ArgImm, .Imm = 4653142004841054208, ._immArgSz = DataWord},
        .ArgSetIdx = 1,
    },
    {
        .im        = &instructions[ins_divf],
        .Arg1      = {.Type = ArgRegister, .RegNum = 0},
        .Arg2      = {.Type = ArgImm, .Imm = 4611686018427387904, ._immArgSz = DataWord},
        .ArgSetIdx = 1,
    },
    {
        .im        = &instructions[ins_mul],
        .Arg1      = {.Type = ArgRegister, .RegNum = 2},
        .Arg2      = {.Type = ArgRegisterOffsetIndirect, .RegNum = 1, .ImmDisp16 = 10},
        .ArgSetIdx = 3,
    },
    {
        .im        = &instructions[ins_subf],
        .Arg1      = {.Type = ArgRegister, .RegNum = 14},
        .Arg2      = {.Type = ArgRegisterIndirect, .RegNum = 5},
        .ArgSetIdx = 2,
    },
    {
        .im        = &instructions[ins_cmp],
        .Arg1      = {.Type = ArgRegister, .RegNum = 6},
        .Arg2      = {.Type = ArgRegisterOffsetIndirect, .RegNum = 2, .ImmDisp16 = -4},
        .ArgSetIdx = 3,
    },
    {
        .im        = &instructions[ins_jmp],
        .Arg1      = {.Type = ArgImm, .Imm = 0, ._immArgSz = DataWord},
        .Arg2      = {.Type = ArgNone},
        .ArgSetIdx = 0,
        .JmpType   = JumpNEQ,
    },

    {
        .im        = &instructions[ins_jmp],
        .Arg1      = {.Type = ArgImm, .Imm = 0, ._immArgSz = DataWord},
        .Arg2      = {.Type = ArgNone},
        .ArgSetIdx = 0,
        .JmpType   = JumpUncond,
    },

    {
        .im        = &instructions[ins_jmp],
        .Arg1      = {.Type = ArgImm, .Imm = 0, ._immArgSz = DataWord},
        .Arg2      = {.Type = ArgNone},
        .ArgSetIdx = 0,
        .JmpType   = JumpUncond,
    },
    {
        .im        = &instructions[ins_jmp],
        .Arg1      = {.Type = ArgImm, .Imm = 0, ._immArgSz = DataWord},
        .Arg2      = {.Type = ArgNone},
        .ArgSetIdx = 0,
        .JmpType   = JumpEQ,
    },
    {
        .im        = &instructions[ins_jmp],
        .Arg1      = {.Type = ArgImm, .Imm = 0, ._immArgSz = DataWord},
        .Arg2      = {.Type = ArgNone},
        .ArgSetIdx = 0,
        .JmpType   = JumpUncond,
    },
    {
        .im        = &instructions[ins_jmp],
        .Arg1      = {.Type = ArgImm, .Imm = 0, ._immArgSz = DataWord},
        .Arg2      = {.Type = ArgNone},
        .ArgSetIdx = 0,
        .JmpType   = JumpUncond,
    },
    {
        .im        = &instructions[ins_jmp],
        .Arg1      = {.Type = ArgImm, .Imm = 0, ._immArgSz = DataWord},
        .Arg2      = {.Type = ArgNone},
        .ArgSetIdx = 0,
        .JmpType   = JumpUncond,
    },
    {
        .im        = &instructions[ins_jmp],
        .Arg1      = {.Type = ArgImm, .Imm = 0, ._immArgSz = DataWord},
        .Arg2      = {.Type = ArgNone},
        .ArgSetIdx = 0,
        .JmpType   = JumpL,
    },
    {
        .im        = &instructions[ins_jmp],
        .Arg1      = {.Type = ArgImm, .Imm = 0, ._immArgSz = DataWord},
        .Arg2      = {.Type = ArgNone},
        .ArgSetIdx = 0,
        .JmpType   = JumpUncond,
    },
    {
        .im        = &instructions[ins_jmp],
        .Arg1      = {.Type = ArgImm, .Imm = 0, ._immArgSz = DataWord},
        .Arg2      = {.Type = ArgNone},
        .ArgSetIdx = 0,
        .JmpType   = JumpGE,
    },

};
