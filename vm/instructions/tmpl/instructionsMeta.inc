

#ifndef INSTR

#error "No INSTR macro"

#endif

INSTR(ret, 0,
      {
          {.First = ArgNone COMMA .Second = ArgNone} COMMA
      })

INSTR(ld, 1,
      {
          {.First = ArgRegister COMMA .Second = ArgRegisterIndirect} COMMA
          {.First = ArgRegister COMMA .Second = ArgRegisterOffsetIndirect} COMMA
          {.First = ArgRegister COMMA .Second = ArgImmIndirect} COMMA
      }
)

INSTR(st, 2,
      {
          {.First = ArgRegister COMMA .Second = ArgRegisterIndirect} COMMA
          {.First = ArgRegister COMMA .Second = ArgRegisterOffsetIndirect} COMMA
          {.First = ArgRegister COMMA .Second = ArgImmIndirect} COMMA
      }

)

INSTR(mov, 3,
      {
          {.First = ArgRegister COMMA .Second = ArgRegister} COMMA
          {.First = ArgRegister COMMA .Second = ArgImm} COMMA
      })

INSTR(push, 4,
      {
          {.First = ArgRegister COMMA .Second = ArgNone} COMMA
          {.First = ArgImm COMMA .Second = ArgNone} COMMA
      })

INSTR(pop, 5,
      {
          {.First = ArgRegister COMMA .Second = ArgNone} COMMA
      })

INSTR(add, 6,
      {
          {.First = ArgRegister COMMA .Second = ArgRegister} COMMA
          {.First = ArgRegister COMMA .Second = ArgImm} COMMA
          {.First = ArgRegister COMMA .Second = ArgRegisterIndirect} COMMA
          {.First = ArgRegister COMMA .Second = ArgRegisterOffsetIndirect} COMMA
      })

INSTR(addf, 7,
      {
          {.First = ArgRegister COMMA .Second = ArgRegister} COMMA
          {.First = ArgRegister COMMA .Second = ArgImm} COMMA
          {.First = ArgRegister COMMA .Second = ArgRegisterIndirect} COMMA
          {.First = ArgRegister COMMA .Second = ArgRegisterOffsetIndirect} COMMA
      })

INSTR(sub, 8,
      {
          {.First = ArgRegister COMMA .Second = ArgRegister} COMMA
          {.First = ArgRegister COMMA .Second = ArgImm} COMMA
          {.First = ArgRegister COMMA .Second = ArgRegisterIndirect} COMMA
          {.First = ArgRegister COMMA .Second = ArgRegisterOffsetIndirect} COMMA
      })

INSTR(subf, 9,
      {
          {.First = ArgRegister COMMA .Second = ArgRegister} COMMA
          {.First = ArgRegister COMMA .Second = ArgImm} COMMA
          {.First = ArgRegister COMMA .Second = ArgRegisterIndirect} COMMA
          {.First = ArgRegister COMMA .Second = ArgRegisterOffsetIndirect} COMMA
      })

INSTR(mul, 10,
      {
          {.First = ArgRegister COMMA .Second = ArgRegister} COMMA
          {.First = ArgRegister COMMA .Second = ArgImm} COMMA
          {.First = ArgRegister COMMA .Second = ArgRegisterIndirect} COMMA
          {.First = ArgRegister COMMA .Second = ArgRegisterOffsetIndirect} COMMA
      })

INSTR(mulf, 11,
      {
          {.First = ArgRegister COMMA .Second = ArgRegister} COMMA
          {.First = ArgRegister COMMA .Second = ArgImm} COMMA
          {.First = ArgRegister COMMA .Second = ArgRegisterIndirect} COMMA
          {.First = ArgRegister COMMA .Second = ArgRegisterOffsetIndirect} COMMA
      })

INSTR(div, 12,
      {
          {.First = ArgRegister COMMA .Second = ArgRegister} COMMA
          {.First = ArgRegister COMMA .Second = ArgImm} COMMA
          {.First = ArgRegister COMMA .Second = ArgRegisterIndirect} COMMA
          {.First = ArgRegister COMMA .Second = ArgRegisterOffsetIndirect} COMMA
      })

INSTR(divf, 13,
      {
          {.First = ArgRegister COMMA .Second = ArgRegister} COMMA
          {.First = ArgRegister COMMA .Second = ArgImm} COMMA
          {.First = ArgRegister COMMA .Second = ArgRegisterIndirect} COMMA
          {.First = ArgRegister COMMA .Second = ArgRegisterOffsetIndirect} COMMA
      })

INSTR(jmp, 14,
      {
          {.First = ArgImm COMMA .Second = ArgNone} COMMA
          {.First = ArgRegister COMMA .Second = ArgNone} COMMA
      })

INSTR(call, 15,
      {
          {.First = ArgImm COMMA .Second = ArgNone} COMMA
          {.First = ArgRegister COMMA .Second = ArgNone} COMMA
      })

INSTR(cmp, 16,
      {
          {.First = ArgRegister COMMA .Second = ArgRegister} COMMA
          {.First = ArgRegister COMMA .Second = ArgImm} COMMA
          {.First = ArgRegister COMMA .Second = ArgRegisterIndirect} COMMA
          {.First = ArgRegister COMMA .Second = ArgRegisterOffsetIndirect} COMMA
      })

INSTR(cmpf, 17,
      {
          {.First = ArgRegister COMMA .Second = ArgRegister} COMMA
          {.First = ArgRegister COMMA .Second = ArgImm} COMMA
          {.First = ArgRegister COMMA .Second = ArgRegisterIndirect} COMMA
          {.First = ArgRegister COMMA .Second = ArgRegisterOffsetIndirect} COMMA
      })

INSTR(halt, 18,
      {
          {.First = ArgNone COMMA .Second = ArgNone} COMMA
      })

INSTR(sqrt, 19,
      {
          {.First = ArgRegister COMMA .Second = ArgRegister} COMMA
          {.First = ArgRegister COMMA .Second = ArgImm} COMMA
          {.First = ArgRegister COMMA .Second = ArgRegisterIndirect} COMMA
          {.First = ArgRegister COMMA .Second = ArgRegisterOffsetIndirect} COMMA
      })
