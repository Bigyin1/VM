#include <assert.h>
#include <string.h>
#include <stdint.h>
#include "../errors.hpp"
#include "encode.hpp"
#include "encoder.hpp"

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

    for (size_t i = 0; commands[i].name != NULL; i++)
    {
        Encode(&commands[i].instr, out); // TODO: handle errors
    }

    return E_ASM_OK;
}

asm_ecode Encode(AsmEncoder *as)
{
    assert(as != NULL);

    if (writeHeader(as->parser->sections[0].currOffset, as->out) != E_ASM_OK)
        return E_ASM_ERR;

    if (writeCode(as->parser->sections[0].commands, as->out) != E_ASM_OK)
        return E_ASM_ERR;

    return E_ASM_OK;
}
