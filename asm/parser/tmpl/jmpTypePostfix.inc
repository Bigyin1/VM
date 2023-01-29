
#ifndef JMP_POSTFIX

#error "No JMP_POSTFIX macro"

#endif

JMP_POSTFIX(eq, JumpEQ)

JMP_POSTFIX(neq, JumpNEQ)

JMP_POSTFIX(g, JumpG)

JMP_POSTFIX(ge, JumpGE)

JMP_POSTFIX(l, JumpL)

JMP_POSTFIX(le, JumpLE)
