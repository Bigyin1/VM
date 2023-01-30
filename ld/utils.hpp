/** @file */
#ifndef LD_UTILS_HPP
#define LD_UTILS_HPP

#include <stdio.h>
#include <stdint.h>
#include "ld.hpp"

const char *getNameFromStrTableByIdx(LinkableFile *f, uint32_t nameIdx);

const char *getNameFromExecFileStringTab(ExecutableFile *ex, uint32_t nameIdx);

#endif
