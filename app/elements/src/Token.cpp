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
         file.write(mcsl::FMT("\033[31mNULL TOKEN\033[39m"));
         break;
      case TokenType::IDEN:
         file.printf(mcsl::FMT("\033[35mIDENTIFIER:\033[39m %s"), tok.name());
         break;
      case TokenType::KEYWORD:
         file.printf(mcsl::FMT("\033[35mKEYWORD:\033[39m %s"), toString(tok.keywordID()));
         break;
      case TokenType::UINT_NUM:
         file.printf(mcsl::FMT("\033[35mINTEGER:\033[39m %u"), tok.valUint());
         break;
      case TokenType::REAL_NUM:
         file.printf(mcsl::FMT("\033[35mREAL NUMBER:\033[39m %f"), tok.valReal());
         break;
      case TokenType::OP:
         file.printf(mcsl::FMT("\033[35mOPERATOR:\033[39;1m \033[34m%s\033[39;22m \033[3m[%s]\033[23m"), toString(tok.opID()), toString(tok.opProps()));
         break;
      case TokenType::PREPROC_INIT:
         file.write(mcsl::FMT("\033[35mINVOKE PREPROCESSOR\033[39m"));
         break;
      case TokenType::EOS:
         file.write(mcsl::FMT("\033[35mEND OF STATEMENT\033[39m"));
         break;
      case TokenType::BLOCK_DELIM:
         file.printf(mcsl::FMT("\033[35mBLOCK DELIMITER:\033[39m %s \033[3m[%s]\033[23m"), toString(tok.block().type), toString(tok.block().role));
         break;
      
      default: UNREACHABLE;
   }
   return file;
}

uint mcsl::writef(File& file, const clef::Token& tok, char mode, FmtArgs args) {
   using namespace clef;
   const char intMode = (mode | CASE_BIT) == 'b' ? mode : ('i' | (mode & CASE_BIT));
   const char fltMode = (mode | CASE_BIT) == 'b' ? mode : ('f' | (mode & CASE_BIT));
   const char strMode = (mode | CASE_BIT) == 'b' ? mode : ('s' | (mode & CASE_BIT));
   const char charMode = (mode | CASE_BIT) == 'b' ? mode : ('c' | (mode & CASE_BIT));
   switch (tok.type()) {
      case TokenType::NONE:
         UNREACHABLE;
      case TokenType::ATTR: fthru;
      case TokenType::IDEN:
         return writef(file, tok.name(), mode, args);
      case TokenType::KEYWORD:
         return writef(file, toString(tok.keywordID()), mode, args);
      case TokenType::UINT_NUM:
         return writef(file, tok.valUint(), intMode, args);
      case TokenType::REAL_NUM:
         return writef(file, tok.valReal(), fltMode, args);
      case TokenType::BOOL_LIT:
         return writef(file, tok.valReal(), strMode, args);
      case TokenType::CHAR_LIT:
         return writef(file, tok.valChar(), charMode, args);
      case TokenType::STR_LIT:
         return writef(file, tok.valStr(), strMode, args);
      case TokenType::OP:
         return writef(file, toString(tok.opID()), mode, args);
      case TokenType::PREPROC_INIT:
         return file.write(PREPROC_INIT), 1;
      case TokenType::EOS:
         return file.write(EOS), 1;
      case TokenType::BLOCK_DELIM:
         return writef(file, toString(tok.block().role), mode, args) + writef(file, ' ', 'c', {}) + writef(file, toString(tok.block().type), mode, args);
      
      default: UNREACHABLE;
   }
   UNREACHABLE;
}

#endif //TOKEN_CPP