#ifndef TOKEN_DATA_CPP
#define TOKEN_DATA_CPP

#include "MidC_Data.hpp"
#include <cassert>

//!array of pairs of strings
//!represents opening and closing block delimiters
const mcs::array<clef::DelimPair> clef::BLOCK_DELIMS{
   {"(\0", ")\0"},   //PARENS
   {"[\0", "]\0"},   //SUBSCRIPT
   {"{\0", "}\0"},   //INITIALIZER LIST
   {"<\0", ">\0"},   //SPECIALIZER

   {"\'\0", "\'\0"}, //CHARACTER
   {"\"\0", "\"\0"}, //STRING

   // {"?\0", ":\0"},   //TERNARY STATEMENT
   
   {"/*", "*/"},     //BLOCK COMMENT
   // {"//", "\n\0"},   //SINGLE-LINE COMMENT
};

//!check if a node is the opening delimiter of a block expression
//!NOTE: REPLACE WITH HASHMAP
clef::DelimPairType clef::blockDelimType(const char* str, const uint length) {
   if (!str || !length || length > MAX_DELIM_LEN) {
      return DelimPairType::NONE;
   }

   //search array
   for (uint i = 0; i < BLOCK_DELIMS.size(); ++i) {
      const uint toklen = std::strlen(BLOCK_DELIMS[i].open);
      if (toklen <= length) {
         if (!std::strncmp(str,BLOCK_DELIMS[i].open,toklen)) {
            return static_cast<DelimPairType>(i);
         }
      }
   }

   return DelimPairType::NONE;
}

// //!array of all Middle-C keywords
// const char clef::KEYWORDS[][MAX_KEYWORD_LEN + 1] = {
//       "uint", "ushort", "ulong", "sint", "sshort", "slong", "ubyte", "sbyte", "bool", "float", "char", "wchar", "wwchar", 
//       "uint8", "uint16", "uint32", "uint64", "uint128", "uint256", 
//       "sint8", "sint16", "sint32", "sint64", "sint128", "sint256", 
//       "float16", "float32", "float64", "float128", "float256", 
//       "half", "single", "double", "quad", "oct", 
//       "void", "auto", 
//       "true", "false", "nullptr", 
//       "new", "delete", "sizeof", "arrsizeof", 
//       "const", "constexpr", "immediate", "mutable", "volatile", "atomic", 
//       "class", "struct", "union", "enum", "namespace", "private", "protected", "public", "friend", "this", "explicit", "using", "template", 
//       "extern", "inline", "static", "virtual", "override",
//       "asm", 
//       "typeof", "typeid", "typename", "alignas", "alignof", 
//       "return", "operator", "assert", "static_assert", "noexcept", 
//       "type_cast", "bit_cast", "safe_cast", "quick_cast", "const_cast", 
//       "if", "else", "for", "while", "do", "break", "continue", "switch", "case", "default", "goto", 
//       "try", "catch", "throw"
//    };

//!array encoding which tokens each character is a legal member of
using enum clef::TokenType;
#define CTMP (clef::TokenType)(0xA0)
clef::TokenType clef::tokTypeArr[256] = {
// NUL   SOH   STX   ETX   EOT   ENQ   ACK   BEL   BS    LF    VT    FF    CR    SO    SI    DLE
   NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,

// DLE   DC1   DC2   DC3   DC4   NAK   SYN   ETB   CAN   EM    SUB   ESC   FS    GS    RS    US
   NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,

// SPACE !     "     #     $     %     &     '     (     )     *     +     ,     -     .     /
   NONE, OP,   PTXT, OP,   OP,   OP,   OP,   PTXT, BLOC, BLOC, CTMP, OP,   OP,   OP, NUM,    OP,

// 0     1     2     3     4     5     6     7     8     9     :     ;     <     =     >     ?
   DGIT, DGIT, DGIT, DGIT, DGIT, DGIT, DGIT, DGIT, DGIT, DGIT, OP,   EOS,  DLIM, OP,   DLIM, OP,

// @     A     B     C     D     E     F     G     H     I     J     K     L     M     N     O
   OP,   XDGT, XDGT, XDGT, XDGT, XDGT, XDGT, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR,

// P     Q     R     S     T     U     V     W     X     Y     Z     [     \     ]     ^     _ 
   CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, BLOC, OP,   BLOC, OP,   CHAR,

// `     a     b     c     d     e     f     g     h     i     j     k     l     m     n     o
   NONE, XDGT, XDGT, XDGT, XDGT, XDGT, XDGT, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, XDGT,

// p     q     r     s     t     u     v     w     x     y     z     {     |     }     ~     DEL
   CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, XDGT, CHAR, CHAR, BLOC, OP,   BLOC, OP,   NONE,

//	€				‚		ƒ		„		…		†		‡		ˆ		‰		Š		‹		Œ				Ž			
   OP,   NONE, NONE, CHAR, NONE, OP,   OP,   OP,   NONE, OP,   CHAR, OP,   CHAR, NONE, CHAR, NONE,

//			‘		’		“		”		•		–		—		˜		™		š		›		œ				ž		Ÿ
   NONE, OP,   OP,   OP,   OP,   OP,   OP,   OP,   OP,   NONE, CHAR, OP,   CHAR, NONE, CHAR, CHAR,

//	NBSP	¡		¢		£		¤		¥		¦		§		¨		©		ª		«		¬		­		 ®		 ¯
   NONE, OP,   OP,   OP,   OP,   OP,   OP,   OP,   NONE, NONE, NONE, OP,   OP,   NONE, NONE, NONE,

//	°		±		²		³		´		µ		¶		·		¸		¹		º		»		¼		½		¾		¿
   OP,   OP,   OP,   OP,   NONE, CHAR, OP,   NONE, NONE, OP,   OP,   OP,   NONE, NONE, NONE, OP,

//	À		Á		Â		Ã		Ä		Å		Æ		Ç		È		É		Ê		Ë		Ì		Í		Î		Ï
   CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR,

//	Ð		Ñ		Ò		Ó		Ô		Õ		Ö		×		Ø		Ù		Ú		Û		Ü		Ý		Þ		ß
   CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR,

//	à		á		â		ã		ä		å		æ		ç		è		é		ê		ë		ì		í		î		ï
   CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR,

//	ð		ñ		ò		ó		ô		õ		ö		÷		ø		ù		ú		û		ü		ý		þ		ÿ
   CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR
};
#undef CTMP

#endif //TOKEN_DATA_CPP