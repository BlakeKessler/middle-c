#ifndef TOKEN_DATA_CPP
#define TOKEN_DATA_CPP

#include "TokenData.hpp"

constexpr clef::TokenType clef::tokType(const char c) {
   switch (c) {
      //WHITESPACE
      case ' ': case '\n': case '\r': case '\t': case '\v': case '\f':
         return clef::TokenType::NONE;

      //OPERATORS
      case '!': case '#': case '$': case '%': case '&': case '+': case ',': case '-': case '.': case '=': case '?': case '@': case '^': case '`': case '|': case '~': 
         return clef::TokenType::OP;

      //PLAINTEXT DELIMITERS
      case '\'': case '\"':
         return clef::TokenType::PTXT;

      //BLOCK DELIMITERS
      case '(': case ')': case '[': case ']': case '{': case '}':
         return clef::TokenType::BLOC;

      //COMMENT DELIMITERS (or operators)
      case '*': case '/':
         return clef::TokenType::CMNT;

      //BLOCK DELIMITER or OPERATOR
      case '<': case '>': case ':':
         return clef::TokenType::DLIM;

      //DECIMAL DIGITS
      case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
         return clef::TokenType::DGIT;

      //HEX DIGITS (or base indicator)
      case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'X': case 'o':
      case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'x': case 'O':
               return clef::TokenType::XDGT;

      //CHARACTERS (or underscores)
      case 'G': case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'Y': case 'Z':
      case 'g': case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n': case 'p': case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w': case 'y': case 'z':
      case '_':
         return clef::TokenType::CHAR;

      //END OF STATEMENT
      case ';':
         return clef::TokenType::EOS;

      //ESCAPE CHARACER:
      case '\\':
         return clef::TokenType::ESC;

      //non-printing character
      default: return clef::TokenType::NONE;
   }
}


// //!array encoding which tokens each character is a legal member of
// using enum clef::TokenType;
// const mcsl::static_arr<clef::TokenType,256> clef::tokTypeArr{
// // NUL   SOH   STX   ETX   EOT   ENQ   ACK   BEL   BS    LF    VT    FF    CR    SO    SI    DLE
//    NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,

// // DLE   DC1   DC2   DC3   DC4   NAK   SYN   ETB   CAN   EM    SUB   ESC   FS    GS    RS    US
//    NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,

// // SPACE !     "     #     $     %     &     '     (     )     *     +     ,     -     .     /
//    NONE, OP,   PTXT, OP,   OP,   OP,   OP,   PTXT, BLOC, BLOC, CMNT, OP,   OP,   OP,   OP,   CMNT,

// // 0     1     2     3     4     5     6     7     8     9     :     ;     <     =     >     ?
//    DGIT, DGIT, DGIT, DGIT, DGIT, DGIT, DGIT, DGIT, DGIT, DGIT, OP,   EOS,  DLIM, OP,   DLIM, OP,

// // @     A     B     C     D     E     F     G     H     I     J     K     L     M     N     O
//    OP,   XDGT, XDGT, XDGT, XDGT, XDGT, XDGT, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, XDGT,

// // P     Q     R     S     T     U     V     W     X     Y     Z     [     \     ]     ^     _ 
//    CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, XDGT, CHAR, CHAR, BLOC, ESC,  BLOC, OP,   CHAR,

// // `     a     b     c     d     e     f     g     h     i     j     k     l     m     n     o
//    OP,   XDGT, XDGT, XDGT, XDGT, XDGT, XDGT, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, XDGT,

// // p     q     r     s     t     u     v     w     x     y     z     {     |     }     ~     DEL
//    CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, XDGT, CHAR, CHAR, BLOC, OP,   BLOC, OP,   NONE,

// //	0x80  0x81  0x82  0x83  0x84  0x85  0x86  0x87  0x88  0x89  0x8a  0x8b  0x8c  0x8d  0x8e  0x8f
//    NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,

// //	0x90  0x91  0x92  0x93  0x94  0x95  0x96  0x97  0x98  0x99  0x9a  0x9b  0x9c  0x9d  0x9e  0x9f
//    NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,

// //	0xa0  0xa1  0xa2  0xa3  0xa4  0xa5  0xa6  0xa7  0xa8  0xa9  0xaa  0xab  0xac  0xad  0xae  0xaf
//    NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,

// //	0xb0  0xb1  0xb2  0xb3  0xb4  0xb5  0xb6  0xb7  0xb8  0xb9  0xba  0xbb  0xbc  0xbd  0xbe  0xbf
//    NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,

// //	0xc0  0xc1  0xc2  0xc3  0xc4  0xc5  0xc6  0xc7  0xc8  0xc9  0xca  0xcb  0xcc  0xcd  0xce  0xcf
//    NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,

// //	0xd0  0xd1  0xd2  0xd3  0xd4  0xd5  0xd6  0xd7  0xd8  0xd9  0xda  0xdb  0xdc  0xdd  0xde  0xdf
//    NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,

// //	0xe0  0xe1  0xe2  0xe3  0xe4  0xe5  0xe6  0xe7  0xe8  0xe9  0xea  0xeb  0xec  0xed  0xee  0xef
//    NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,

// //	0xf0  0xf1  0xf2  0xf3  0xf4  0xf5  0xf6  0xf7  0xf8  0xf9  0xfa  0xfb  0xfc  0xfd  0xfe  0xff
//    NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE
// };


//!calculate token type of a string
template<mcsl::str_t str_t> constexpr clef::TokenType clef::tokType(const str_t& str) {
   TokenType type = TokenType::ANY;
   for (uint i = str.size(); i;) { type = type & tokType(str[--i]); }
   return type;
}

#endif //TOKEN_DATA_CPP