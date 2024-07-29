#ifndef ALL_KEYWORDS_HASH_CPP
#define ALL_KEYWORDS_HASH_CPP
/* adapted from C++ code produced by gperf version 3.1 */

#include "CLEF.hpp"
#include "Hash.hpp"
#include <cstring>


//!hash function for Middle-C keyword hash table
//!hash ∈[9,122]
inline unsigned int clef::Hash::keywordHash(const char* str, uint len) {
	return len + keywordCalcTable[+(str[1]+1)] + 
      keywordCalcTable[+(str[0])] +
      keywordCalcTable[+(str[len - 1])];
}

//!check if a string is a Middle-C keyword
//!NOTE: BROKEN - intentionally always returns nullptr until it is fixed
const char* clef::Hash::isKeyword(const char* str, const uint len) {
	return nullptr; str; len;

   // if (len && len <= MAX_KEYWORD_LEN) {
   //    const uint key = keywordHash(str, len);

   //    if (key <= MAX_KEYWORD_HASH) {
   //       const char* keyword = keywordHashTable[key];

   //       if (((*str ^ *keyword) & ~32) && !clef::strcmp(str, keyword, len)) {
   //          return keyword;
   //       }
   //    }
   // }
	// return nullptr;
}

const byte clef::Hash::keywordCalcTable[] = {
   123, 123, 123, 123, 123, 123, 123, 123, 123, 123,
   123, 123, 123, 123, 123, 123, 123, 123, 123, 123,
   123, 123, 123, 123, 123, 123, 123, 123, 123, 123,
   123, 123, 123, 123, 123, 123, 123, 123, 123, 123,
   123, 123, 123, 123, 123, 123, 123, 123, 123, 123,
    94, 123,  89, 123,  14, 123,  81, 123, 123, 123,
   123, 123, 123, 123, 123,   9,  42,  24,  23,   0,
     6,  53,  50,  50,   1,  21,  47,  15,  32,  22,
    16,  39,  25,   3,	 4,   0,  22,  26,   7,  70,
    81, 123, 123, 123, 123, 123, 123,   9,  42,  24,
    23,   0,	6,  53,  50,  50,   1,  21,  47,  15,
    32,  22,  16,  39,  25,   3,	  4,   0,  22,  26,
     7,  70,  81, 123, 123, 123, 123, 123, 123, 123,
   123, 123, 123, 123, 123, 123, 123, 123, 123, 123,
   123, 123, 123, 123, 123, 123, 123, 123, 123, 123,
   123, 123, 123, 123, 123, 123, 123, 123, 123, 123,
   123, 123, 123, 123, 123, 123, 123, 123, 123, 123,
   123, 123, 123, 123, 123, 123, 123, 123, 123, 123,
   123, 123, 123, 123, 123, 123, 123, 123, 123, 123,
   123, 123, 123, 123, 123, 123, 123, 123, 123, 123,
   123, 123, 123, 123, 123, 123, 123, 123, 123, 123,
   123, 123, 123, 123, 123, 123, 123, 123, 123, 123,
   123, 123, 123, 123, 123, 123, 123, 123, 123, 123,
   123, 123, 123, 123, 123, 123, 123, 123, 123, 123,
   123, 123, 123, 123, 123, 123, 123, 123, 123, 123,
   123, 123, 123, 123, 123, 123
};
const char* const clef::Hash::keywordHashTable[] = {
   "",  "", "",  "", "",  "", "",  "", "",
   "uint",
   "single",
   "true",
   "sint",
   "struct",
   "ushort",
   "",
   "sizeof",
   "sshort",
   "template",
   "else",
   "static_assert",
   "uint16",
   "uint256",
   "assert",
   "sint16",
   "sint256",
   "private",
   "arrsizeof",
   "",
   "ubyte",
   "float",
   "asm",
   "sbyte",
   "static",
   "alignas",
   "delete",
   "",
   "alignof",
   "friend",
   "atomic",
   "default",
   "enum",
   "float16",
   "float256",
   "mutable",
   "double",
   "volatile",
   "class",
   "continue",
   "const",
   "for",
   "protected",
   "oct",
   "false",
   "const_cast",
   "bit_cast",
   "override",
   "auto",
   "safe_cast",
   "union",
   "noexcept",
   "this",
   "using",
   "do",
   "wwchar",
   "void",
   "switch",
   "new",
   "public",
   "return",
   "case",
   "break",
   "",
   "ulong",
   "constexpr",
   "quick_cast",
   "slong",
   "virtual",
   "inline",
   "wchar",
   "try",
   "while",
   "explicit",
   "namespace",
   "",
   "throw",
   "nullptr",
   "uint8",
   "quad",
   "uint128",
   "sint8",
   "immediate",
   "sint128",
   "typename",
   "operator",
   "goto",
   "uint64",
   "typeof",
   "type_cast",
   "sint64",
   "",
   "uint32",
   "half",
   "char",
   "sint32",
   "",  "", "",
   "extern",
   "bool",
   "float128",
   "if",
   "",  "",
   "typeid",
   "",  "",
   "float64",
   "",  "", "",
   "catch",
   "float32"
};

#endif //ALL_KEYWORDS_HASH_CPP