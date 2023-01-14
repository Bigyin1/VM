


#ifndef INSTR

#error "No INSTR macros"

#endif

INSTR(ret, 0,
      {
          {.First = ArgNone, .Second = ArgNone},
      }
)

INSTR(ld, 1,
        {
          {.First = ArgRegister, .Second = ArgRegisterIndirect},
          {.First = ArgRegister, .Second = ArgRegisterOffsetIndirect},
          {.First = ArgRegister, .Second = ArgImmIndirect},
          {.First = ArgRegister, .Second = ArgImmOffsetIndirect},
        }
)

INSTR(st, 2,
      {
          {.First = ArgRegister, .Second = ArgRegisterIndirect},
          {.First = ArgRegister, .Second = ArgRegisterOffsetIndirect},
          {.First = ArgRegister, .Second = ArgImmIndirect},
          {.First = ArgRegister, .Second = ArgImmOffsetIndirect},
      }

)

INSTR(mov, 3,
      {
          {.First = ArgRegister, .Second = ArgRegister},
          {.First = ArgRegister, .Second = ArgImm},
      }
)

INSTR(push, 4,
      {
          {.First = ArgRegister, .Second = ArgNone},
          {.First = ArgImm, .Second = ArgNone},
      }
)

INSTR(pop, 5,
        {
            {.First = ArgRegister, .Second = ArgNone},
        }
)

INSTR(add, 6,
        {
            {.First = ArgRegister, .Second = ArgRegister},
            {.First = ArgRegister, .Second = ArgImm},
        }
)

INSTR(addf, 7,
        {
            {.First = ArgRegister, .Second = ArgRegister},
            {.First = ArgRegister, .Second = ArgImm},
        }
)

INSTR(sub, 8,
        {
            {.First = ArgRegister, .Second = ArgRegister},
            {.First = ArgRegister, .Second = ArgImm},
        }
)

INSTR(subf, 9,
        {
            {.First = ArgRegister, .Second = ArgRegister},
            {.First = ArgRegister, .Second = ArgImm},
        }
)

INSTR(mul, 10,
        {
            {.First = ArgRegister, .Second = ArgRegister},
            {.First = ArgRegister, .Second = ArgImm},
        }
)

INSTR(mulf, 11,
        {
            {.First = ArgRegister, .Second = ArgRegister},
            {.First = ArgRegister, .Second = ArgImm},
        }
)

INSTR(div, 12,
        {
            {.First = ArgRegister, .Second = ArgRegister},
            {.First = ArgRegister, .Second = ArgImm},
        }
)

INSTR(divf, 13,
        {
            {.First = ArgRegister, .Second = ArgRegister},
            {.First = ArgRegister, .Second = ArgImm},
        }
)

INSTR(jmp, 14,
        {
            {.First = ArgImm, .Second = ArgNone},
            {.First = ArgRegister, .Second = ArgNone},
        }
)

INSTR(call, 15,
        {
            {.First = ArgImm, .Second = ArgNone},
            {.First = ArgRegister, .Second = ArgNone},
        }
)

INSTR(cmp, 16,
        {
            {.First = ArgRegister, .Second = ArgRegister},
            {.First = ArgRegister, .Second = ArgImm},
        }
)

INSTR(halt, 17,
      {
          {.First = ArgNone, .Second = ArgNone},
      }
)


