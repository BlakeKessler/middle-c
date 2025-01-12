#ifndef TOKEN_CPP
#define TOKEN_CPP

#include "Token.hpp"
#include "assert.hpp"

#include <cstdio>
#ifdef MCSL
   #undef NULL
#endif

void clef::Token::printf() const {
   switch (_type) {
      case TokenType::NONE:
         std::printf("\033[31mNULL TOKEN\033[39m");
         break;
      case TokenType::IDEN:
         std::printf("\033[35mIDENTIFIER:\033[39m %.*s", _name.size(), _name.begin());
         break;
      case TokenType::KEYWORD:
         std::printf("\033[35mKEYWORD:\033[39m %hhu", +_keyword);
         break;
      case TokenType::INT_NUM:
         std::printf("\033[35mINTEGER:\033[39m %lu", _intVal);
         break;
      case TokenType::REAL_NUM:
         std::printf("\033[35mREAL NUMBER:\033[39m %lf", _realVal);
         break;
      case TokenType::OP:
         std::printf("\033[35mOPERATOR:\033[39m %hhu %hhu", +_op.opID(), +_op.props());
         break;
      case TokenType::PREPROC_INIT:
         std::printf("\033[35mINVOKE PREPROCESSOR\033[39m");
         break;
      case TokenType::PREPROC_EOS:
         std::printf("\033[35mEXIT PREPROCESSOR\033[39m");
         break;
      case TokenType::EOS:
         std::printf("\033[35mEND OF STATEMENT\033[39m");
         break;
      case TokenType::ESC:
         std::printf("\033[35mESCAPE CHARACTER\033[39m");
         break;
      case TokenType::BLOCK_DELIM:
         std::printf("\033[35mBLOCK DELIMITER:\033[39m %hhu %huu", +_blockDelim.first, +_blockDelim.second);
         break;
      case TokenType::PTXT_SEG:
         std::printf("\033[35mPLAINTEXT SEGMENT (%hhu):\033[39m ", +_ptxtType);
         switch (_ptxtType) {
            case PtxtType::CHAR           : std::printf("%c", _charVal); break;
            case PtxtType::STR            : std::printf("%.*s", _strVal.size(), _strVal.begin()); break;
            case PtxtType::UNPROCESSED_STR: std::printf("%.*s", _strVal.size(), _strVal.begin()); break;

            default: throwError(ErrCode::LEXER_NOT_IMPLEMENTED, "\033[35munimplemented plaintext segment type used (%hhu)\033[39m", +_ptxtType);
         }
         break;
      
      default: UNREACHABLE;
   }
}

#endif //TOKEN_CPP