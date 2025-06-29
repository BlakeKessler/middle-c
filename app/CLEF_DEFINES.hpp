#pragma once
#ifndef CLEF_DEFINES_HPP
#define CLEF_DEFINES_HPP

#include "MCSL_DEFINES.hpp"

//compiler parameters
#define PARALLEL_COMPILE_FILES false

#define MAX_OP_LEN 3

#define OP_DECODER_HASH_BASE 37

#define EST_TOKS_FROM_SOURCE_SIZE(bytes) (bytes / 8)
#define EST_NODES_FROM_TOKS(toks) toks

#define MANGLE_PREFIX "_M"

//language parameters
#define MAX_BIN_CHAR_LIT_DIGITS 8
#define MAX_OCT_CHAR_LIT_DIGITS 3
#define MAX_DEC_CHAR_LIT_DIGITS 3
#define MAX_HEX_CHAR_LIT_DIGITS 2

#endif //CLEF_DEFINES_HPP