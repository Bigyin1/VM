#include <assert.h>
#include <string.h>
#include <stdint.h>
#include "../errors.hpp"
#include "../../vm/instructions/instructions.hpp"
#include "assembler.hpp"

const uint32_t magicHeader = 0xFAAFAAAF;

static e_asm_codes writeHeader(size_t codeSz, FILE *out)
{
    fwrite(&magicHeader, sizeof(magicHeader), 1, out);
    fwrite(&codeSz, sizeof(codeSz), 1, out);

    return E_ASM_OK;
}

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

    if (writeHeader(as->prog->currOffset, as->out) != E_ASM_OK)
        return E_ASM_ERR;

    if (writeCode(as->prog->commands, as->out) != E_ASM_OK)
        return E_ASM_ERR;

    return E_ASM_OK;
}
