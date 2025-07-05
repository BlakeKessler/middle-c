#ifndef CLEF_ERR_CPP
#define CLEF_ERR_CPP

#include "CLEF_ERR.hpp"
#include "CLEF.hpp"

//!array of CLEF error message prefix strings
const mcsl::str_slice clef::ERR_MSG_ARR[] {
   mcsl::FMT("UNSPECIFIED ERROR"),

   mcsl::FMT("LEXER ERROR"),
   mcsl::FMT("UN-IMPLEMENTED LEXER FEATURE"),

   mcsl::FMT("PARSER ERROR"),
   mcsl::FMT("UN-IMPLEMENTED PARSER FEATURE"),
   //syntax errors
   mcsl::FMT("BAD PARAMTER BLOCK"),
   mcsl::FMT("BAD LITERAL"),
   mcsl::FMT("BAD COMMENT TOKEN"),
   mcsl::FMT("BAD IDENTIFIER"),
   mcsl::FMT("ILLEGAL USE OF KEYWORD"),

   mcsl::FMT("MISSING KEYWORD"),
   mcsl::FMT("MISSING OPERATOR"),
   mcsl::FMT("MISSING BLOCK DELIMITER"),
   mcsl::FMT("MISSING END-OF-STATEMENT TOKEN"),
   
   mcsl::FMT("BAD STATEMENT"),
   mcsl::FMT("BAD EXPRESSION"),
   mcsl::FMT("BAD BLOCK DELIMITER"),
   mcsl::FMT("UNCLOSED BLOCK"),
   mcsl::FMT("BAD FUNCTION DECLARATION"),
   mcsl::FMT("BAD DECLARATION"),
   mcsl::FMT("BAD TYPE DECLARATION"),

   mcsl::FMT("BAD PREPROCESSOR STATEMENT"),

   mcsl::FMT("BAD ATTRIBUTE"),
   //ast/ast node errors
   mcsl::FMT("BAD LITERAL CAST"),
   mcsl::FMT("BAD AST NODE CAST"),
   
   //typechecking errors
   mcsl::FMT("TYPE ERROR")
};

#endif //CLEF_ERR_CPP