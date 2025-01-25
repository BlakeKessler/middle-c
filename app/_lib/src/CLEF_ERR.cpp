#ifndef CLEF_ERR_CPP
#define CLEF_ERR_CPP

#include "CLEF_ERR.hpp"
#include "CLEF.hpp"

//!array of CLEF error message prefix strings
const char* clef::ERR_MSG_ARR[] {
   "UNSPECIFIED ERROR: ",

   "LEXER ERROR: ",
   "UN-IMPLEMENTED LEXER FEATURE: ",

   "PARSER ERROR: ",
   "UN-IMPLEMENTED PARSER FEATURE: ",
   //syntax errors
   "BAD PARAMTER BLOCK: ",
   "BAD LITERAL: ",
   "BAD COMMENT TOKEN: ",
   "BAD IDENTIFIER: ",
   "ILLEGAL USE OF KEYWORD: ",

   "MISSING KEYWORD: ",
   "MISSING OPERATOR: ",
   "MISSING BLOCK DELIMITER: ",
   "MISSING END-OF-STATEMENT TOKEN: ",
   
   "BAD STATEMENT: ",
   "BAD EXPRESSION: ",
   "BAD BLOCK DELIMITER: ",
   "UNCLOSED BLOCK: ",
   "BAD FUNCTION DECLARATION: ",
   "BAD DECLARATION: ",
   //ast/ast node errors
   "BAD LITERAL CAST: ",
   "BAD AST NODE CAST: "
};

#endif //CLEF_ERR_CPP