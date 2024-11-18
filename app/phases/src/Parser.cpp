#ifndef PARSER_CPP
#define PARSER_CPP

#include "Parser.hpp"

#define line src.row(lineNum)
clef::SyntaxTree clef::Parser::parse(const SourceTokens& src) {
   const Token* endtok = src.end();

   SyntaxTree tree;
   astNode* curr = tree.root();

   Statement* currStmt;

   for (uint lineNum = 0; lineNum < src.lineCount();) {
      const Token* const lineEnd = line.end();

      //process line
      for (const Token* tokIt = line.begin(); tokIt < lineEnd; ++tokIt) {
         switch (tokIt->type()) {
            case TokenType::EOS:
               //!NOTE: assumes that the EOS token is always a single character
               //!NOTE: UNFINISHED


               break;
            case TokenType::PTXT:{
               const OpGroup op = PTXT_DELIMS[*tokIt];
               //find closing token
               for (const Token* it = tokIt+1; it < lineEnd; ++it) {
                  //non-matching tokens
                  if (it->type() != TokenType::PTXT) { continue; }
                  if (PTXT_DELIMS[*it].toString() != op.toString()) { continue; }
                  //check that the token is not escaped
                  if ((it-1)->type() == TokenType::ESC) { continue; }
                  
                  //found end of segment
                  //!NOTE: UNFINISHED
                  if (op.toString() == CHAR_OP.toString()) {         //character literal
                     



                  } else if (op.toString() == STR_OP.toString()) {   //string literal
                     const mcsl::raw_str_span strLitContents{const_cast<char*>(tokIt->end()), const_cast<char*>(it->begin())};
                     



                  } else {
                     //!SYNTAX ERROR - bad plaintext literal
                  }
               }
               //!SYNTAX ERROR - bad plaintext literal

               break;
            }
            case TokenType::CMNT:{
               const OpGroup cmntOp = PTXT_DELIMS[*tokIt];
               if (cmntOp.toString() == LINE_CMNT.toString()) { //!single-line comment
                  goto endofline;
               } else if (cmntOp.toString() == BLOCK_CMNT_OPEN.toString()) { //!multi-line comment
                  while (++tokIt < endtok) {
                     //check for block comment closer
                     if (tokIt->type() == TokenType::CMNT && PTXT_DELIMS[*tokIt].toString() == BLOCK_CMNT_CLOSE.toString()) {
                        //update line number if necessary
                        if (tokIt >= lineEnd) {
                           do {
                              ++lineNum;
                           } while (tokIt >= line.end());
                           const_cast<const Token*&>(lineEnd) = line.end();   //NOTE: INTENTIONALLY DOES SOME HACKY GARBAGE TO MODIFY A CONST VARIABLE
                        }
                        //skip the comment
                        goto endoftok;
                     }
                  }

                  //SYNTAX ERROR - UNCLOSED BLOCK COMMENT
               } else {
                  //SYNTAX ERROR
               }
               break;
            }
            case TokenType::BLOC:
               break;
            case TokenType::ESC:
               break;
            case TokenType::OP:
               break;
               
            case TokenType::IDEN: //RELIES ON FALLTHROUGH
               //check first char
               if (!+(tokType((*tokIt)[0]) & TokenType::STRT)) {
                  //SYNTAX ERROR
               }
            case TokenType::STRT:
            case TokenType::CHAR:
               break;

            case TokenType::NUM:
            case TokenType::DGIT:
               break;
            case TokenType::XDGT:
               break;
               
            default:
               //SYNTAX ERROR - ILLEGAL TOKEN
               break;
         }
         endoftok:
      }

      ++lineNum;
      endofline: //clean up after processing line
   }

   endoffile:
}
#undef line

#endif //PARSER_CPP