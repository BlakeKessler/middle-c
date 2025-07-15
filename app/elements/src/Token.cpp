#ifndef TOKEN_CPP
#define TOKEN_CPP

#include "Token.hpp"
#include "pretty-print.hpp"

#include "unreachable.hpp"
#include "io.hpp"

mcsl::File& mcsl::write(File& file, const clef::Token& tok) {
   using namespace clef;
   switch (tok.type()) {
      case TokenType::NONE:
         file.printf(mcsl::FMT("\033[31mNULL TOKEN\033[39m"));
         break;
      case TokenType::IDEN:
         file.printf(mcsl::FMT("\033[35mIDENTIFIER:\033[39m %s"), tok.name());
         break;
      case TokenType::KEYWORD:
         file.printf(mcsl::FMT("\033[35mKEYWORD:\033[39m %s"), toString(tok.keywordID()));
         break;
      case TokenType::UINT_NUM:
         file.printf(mcsl::FMT("\033[35mINTEGER:\033[39m %u"), tok.uintVal());
         break;
      case TokenType::REAL_NUM:
         file.printf(mcsl::FMT("\033[35mREAL NUMBER:\033[39m %f"), tok.realVal());
         break;
      case TokenType::OP:
         file.printf(mcsl::FMT("\033[35mOPERATOR:\033[39;1m \033[34m%s\033[39;22m \033[3m[%s]\033[23m"), toString(tok.opID()), toString(tok.opProps()));
         break;
      case TokenType::PREPROC_INIT:
         file.printf(mcsl::FMT("\033[35mINVOKE PREPROCESSOR\033[39m"));
         break;
      case TokenType::PREPROC_EOS:
         file.printf(mcsl::FMT("\033[35mEXIT PREPROCESSOR\033[39m"));
         break;
      case TokenType::EOS:
         file.printf(mcsl::FMT("\033[35mEND OF STATEMENT\033[39m"));
         break;
      case TokenType::ESC:
         file.printf(mcsl::FMT("\033[35mESCAPE CHARACTER\033[39m"));
         break;
      case TokenType::BLOCK_DELIM:
         file.printf(mcsl::FMT("\033[35mBLOCK DELIMITER:\033[39m %s \033[3m[%s]\033[23m"), toString(tok.blockType()), toString(tok.blockDelimRole()));
         break;
      case TokenType::PTXT_SEG:
         file.printf(mcsl::FMT("\033[35mPLAINTEXT SEGMENT (\033[3m%s\033[23m):\033[39m "), toString(tok.ptxtType()));
         switch (tok.ptxtType()) {
            case PtxtType::CHAR           : file.printf(mcsl::FMT("%c"), tok.charVal()); break;
            case PtxtType::STR            : file.printf(mcsl::FMT("%s"), tok.strVal()); break;
            case PtxtType::UNPROCESSED_STR: file.printf(mcsl::FMT("%s"), tok.unprocessedStrVal()); break;

            default: throwError(clef::ErrCode::LEXER_NOT_IMPLEMENTED, mcsl::FMT("\033[35munimplemented plaintext segment type used (%s)\033[39m"), toString(tok.ptxtType()));
         }
         break;
      
      default: UNREACHABLE;
   }
   return file;
}

uint mcsl::writef(File& file, const clef::Token& tok, char mode, FmtArgs args) {
   using namespace clef;
   const char intMode = (mode | CASE_BIT) == 'b' ? mode : ('i' | (mode & CASE_BIT));
   const char fltMode = (mode | CASE_BIT) == 'b' ? mode : ('f' | (mode & CASE_BIT));
   switch (tok.type()) {
      case TokenType::NONE:
         UNREACHABLE;
      case TokenType::IDEN:
         return writef(file, tok.name(), mode, args);
      case TokenType::KEYWORD:
         return writef(file, toString(tok.keywordID()), mode, args);
      case TokenType::UINT_NUM:
         return writef(file, tok.uintVal(), intMode, args);
      case TokenType::REAL_NUM:
         return writef(file, tok.realVal(), fltMode, args);
      case TokenType::OP:
         return writef(file, toString(tok.opID()), mode, args);
      case TokenType::PREPROC_INIT:
         return file.write(PREPROC_INIT), 1;
      case TokenType::PREPROC_EOS:
         return file.write('\n'), 1;
      case TokenType::EOS:
         return file.write(EOS), 1;
      case TokenType::ESC:
         return file.write(ESCAPE_CHAR), 1;
      case TokenType::BLOCK_DELIM:
         return writef(file, toString(tok.blockDelimRole()), mode, args) + writef(file, ' ', 'c', {}) + writef(file, toString(tok.blockType()), mode, args);
      case TokenType::PTXT_SEG:
         switch (tok.ptxtType()) {
            case PtxtType::CHAR:
               return file.write(tok.charVal()), 1;
            case PtxtType::STR:
               return writef(file, tok.strVal(), mode, args);
            case PtxtType::UNPROCESSED_STR:
               return writef(file, tok.unprocessedStrVal(), mode, args);

            default: throwError(clef::ErrCode::LEXER_NOT_IMPLEMENTED, mcsl::FMT("\033[35munimplemented plaintext segment type used (%s)\033[39m"), toString(tok.ptxtType()));
         }
      
      default: UNREACHABLE;
   }
   UNREACHABLE;
}

#endif //TOKEN_CPP