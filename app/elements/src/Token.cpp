#ifndef TOKEN_CPP
#define TOKEN_CPP

#include "Token.hpp"
#include "pretty-print.hpp"

#include "unreachable.hpp"
#include "io.hpp"

void clef::Token::printf() const {
   switch (_type) {
      case TokenType::NONE:
         mcsl::printf(mcsl::FMT("\033[31mNULL TOKEN\033[39m"));
         break;
      case TokenType::IDEN:
         mcsl::printf(mcsl::FMT("\033[35mIDENTIFIER:\033[39m %s"), _name);
         break;
      case TokenType::KEYWORD:
         mcsl::printf(mcsl::FMT("\033[35mKEYWORD:\033[39m %s"), toString(_keyword));
         break;
      case TokenType::INT_NUM:
         mcsl::printf(mcsl::FMT("\033[35mINTEGER:\033[39m %u"), _intVal);
         break;
      case TokenType::REAL_NUM:
         mcsl::printf(mcsl::FMT("\033[35mREAL NUMBER:\033[39m %f"), _realVal);
         break;
      case TokenType::OP:
         mcsl::printf(mcsl::FMT("\033[35mOPERATOR:\033[39;1m \033[34m%s\033[39;22m \033[3m[%s]\033[23m"), toString(_op.opID()), toString(_op.props()));
         break;
      case TokenType::PREPROC_INIT:
         mcsl::printf(mcsl::FMT("\033[35mINVOKE PREPROCESSOR\033[39m"));
         break;
      case TokenType::PREPROC_EOS:
         mcsl::printf(mcsl::FMT("\033[35mEXIT PREPROCESSOR\033[39m"));
         break;
      case TokenType::EOS:
         mcsl::printf(mcsl::FMT("\033[35mEND OF STATEMENT\033[39m"));
         break;
      case TokenType::ESC:
         mcsl::printf(mcsl::FMT("\033[35mESCAPE CHARACTER\033[39m"));
         break;
      case TokenType::BLOCK_DELIM:
         mcsl::printf(mcsl::FMT("\033[35mBLOCK DELIMITER:\033[39m %s \033[3m[%s]\033[23m"), toString(_blockDelim.first), toString(_blockDelim.second));
         break;
      case TokenType::PTXT_SEG:
         mcsl::printf(mcsl::FMT("\033[35mPLAINTEXT SEGMENT (\033[3m%s\033[23m):\033[39m "), toString(_ptxtType));
         switch (_ptxtType) {
            case PtxtType::CHAR           : mcsl::printf(mcsl::FMT("%c"), _charVal); break;
            case PtxtType::STR            : mcsl::printf(mcsl::FMT("%s"), _strVal); break;
            case PtxtType::UNPROCESSED_STR: mcsl::printf(mcsl::FMT("%s"), _strVal); break;

            default: throwError(ErrCode::LEXER_NOT_IMPLEMENTED, mcsl::FMT("\033[35munimplemented plaintext segment type used (%s)\033[39m"), toString(_ptxtType));
         }
         break;
      
      default: UNREACHABLE;
   }
}

#endif //TOKEN_CPP