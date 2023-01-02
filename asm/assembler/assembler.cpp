#include <assert.h>
#include <string.h>
#include <stdint.h>
#include "../errors.hpp"
#include "../../vm/instructions/instructions.hpp"
#include "assembler.hpp"

static e_asm_codes writeCode(commandNode *commands, FILE *out)
{
    assert(commands != NULL);
    assert(out != NULL);

    for (size_t i = 0; commands[i].instrName != NULL; i++)
    {
        Encode(&commands[i].instr, out);
    }

    return E_ASM_OK;
}

asm_ecode assemble(assembler_s *as)
{
    assert(as != NULL);

    if (writeCode(as->prog->commands, as->out) != E_ASM_OK)
        return E_ASM_ERR;

    return E_ASM_OK;
}
