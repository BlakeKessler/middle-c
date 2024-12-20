#ifndef PARSER_CPP
#define PARSER_CPP

#include "Parser.hpp"

#include "type_traits.hpp"

#define advance() if (++tokIt >= endtok) { goto DONE_WITH_FILE; }
#define gotoEOS() \
if (tokIt->type() != TokenType::EOS) {\
   hadErrors |= logError<true>(ErrCode::PARSER_UNSPEC, "missing EOS");\
   goto DONE_WITH_FILE;\
} goto EOS
clef::SyntaxTree clef::Parser::parse(const SourceTokens& src) {
   SyntaxTree tree;

   const Token* endtok = src.end();

   auto lineIt = src.lines().begin(); //will almost never be accurate
   bool hadErrors = false;


   for (const Token* tokIt = src.begin(); tokIt < endtok; ++tokIt) {
      switch (tokIt->type()) {
         //!END OF STATEMENT:
         case TokenType::EOS: EOS:
            tree.endStatement();
            goto DONE_WITH_TOKEN;


         //!PLAINTEXT DELIMITERS:
         case TokenType::PTXT:{
            const OpGroup op = PTXT_DELIMS[*tokIt];
            //find closing token
            for (const Token* it = tokIt+1; it < endtok; ++it) {
               //non-matching tokens
               if (it->type() == TokenType::ESC) { ++it; continue; }
               if ((it->type() != TokenType::PTXT) || (PTXT_DELIMS[*it].toString() != op.toString())) { continue; }
               
               //found end of segment
               if (op.toString() == CHAR_OP.toString()) {         //!character literal
                  const char ch = Parser::parseCharLitASCII(tokIt->begin(), it->begin());
                  if (ch < 0) { hadErrors |= logError<true>(ErrCode::BAD_LITERAL, "bad ASCII character literal"); } //!SYNTAX ERROR - BAD CHAR LITERAL
                  
                  tree.pushNode(Literal{ch});
                  tokIt = it;
                  goto DONE_WITH_TOKEN;

               } else if (op.toString() == STR_OP.toString()) {   //!string literal
                  const mcsl::string strLitContents = parseStrLitASCII(tokIt->end(), it->begin());
                  tree.pushNode(Literal{strLitContents});
                  tokIt = it;
                  goto DONE_WITH_TOKEN;

               } else { //!SYNTAX ERROR - bad plaintext literal
                  hadErrors |= logError<true>(ErrCode::BAD_LITERAL, "bad character literal opener");
                  goto DONE_WITH_FILE;
               }
            }
            //!SYNTAX ERROR - bad plaintext block
            hadErrors |= logError<true>(ErrCode::PARSER_UNSPEC, "un-closed plaintext block opener");
            goto DONE_WITH_FILE;
         }


         //!COMMENT DELIMITERS:
         case TokenType::CMNT:{
            const OpGroup cmntOp = PTXT_DELIMS[*tokIt];
            if (cmntOp.toString() == LINE_CMNT.toString()) { //!single-line comment
               //update lineIt
               const auto linesEnd = src.lines().end();
               for (; lineIt < linesEnd; ++lineIt) {
                  if (lineIt->begin() > tokIt) { //found start of new line
                     tokIt = lineIt->begin();
                     goto DONE_WITH_TOKEN;
                  }
               }
               //didn't find a next line to go to -> must be a line comment on the last line of the file
               goto DONE_WITH_FILE;

            } else if (cmntOp.toString() == BLOCK_CMNT_OPEN.toString()) { //!multi-line comment
               //find block comment closer
               while (++tokIt < endtok) {
                  if (tokIt->type() == TokenType::CMNT && PTXT_DELIMS[*tokIt].toString() == BLOCK_CMNT_CLOSE.toString()) {
                     goto DONE_WITH_TOKEN;
                  }
               }
               //!SYNTAX ERROR - UNCLOSED BLOCK COMMENT
               hadErrors |= logError<true>(ErrCode::BAD_CMNT, "unclosed block comment");
               goto DONE_WITH_FILE;
            }
            //!SYNTAX ERROR - invalid comment - SHOULD NOT BE POSSIBLE (should be prevented by previous phases)
            hadErrors |= logError<true>(ErrCode::BAD_CMNT, "invalid comment");
            goto DONE_WITH_FILE;
         }


         //!BLOCK DELIMITERS:
         case TokenType::BLOC:{
            BlockType type = consumeBlock(tree, tokIt, endtok);
            if (type == BlockType::NONE) { //token string is not a block delimiter
               hadErrors |= logError<true>(ErrCode::PARSER_UNSPEC, "block delimiter token string does not correspond to any block delimiter");
               goto DONE_WITH_FILE;
            }
            if (tokIt == nullptr) { //unclosed block
               hadErrors |= logError<true>(ErrCode::UNCLOSED_BLOCK, "unclosed block (block type: %u)", +type);
               goto DONE_WITH_FILE;
            }
            if (tokIt >= endtok) {
               goto DONE_WITH_FILE;
            }
            goto DONE_WITH_TOKEN;
         }

         //!ESCAPE CHARACTERS:
         case TokenType::ESC:
            hadErrors |= logError<true>(ErrCode::PARSER_UNSPEC, "floating escape character");
            goto DONE_WITH_FILE;

         //!OPERATORS:
         case TokenType::OP:
            //!NOTE: UNFINISHED


            goto DONE_WITH_TOKEN;
            

         //!IDENTIFIERS:
         case TokenType::IDEN: //RELIES ON FALLTHROUGH
            //check first char
            if (!+(tokType((*tokIt)[0]) & TokenType::STRT)) {
               //!SYNTAX ERROR - identifier starting with illegal first char of identifier
               hadErrors |= logError<true>(ErrCode::BAD_IDEN, "identifier must start with letter or underscore (%.*s)", tokIt->size(),tokIt->begin());
               goto DONE_WITH_FILE;
            }
            [[fallthrough]];
         case TokenType::STRT: //RELIES ON FALLTHROUGH
            [[fallthrough]];
         case TokenType::CHAR:{
            //!NOTE: NONE OF THESE ACTUALLY PUSH THE NODE FOR THE KEYWORD YET!
            const KeywordID keyword = decodeKeyword(*tokIt);
            switch (keyword) {
               //!CONTROL FLOW KEYWORDS
               #pragma region controlflow
               case KeywordID::GOTO:
                  advance();
                  if (!consumeIdentifier(tree, tokIt, endtok)) { //!NOTE: probably rework to not allow scoped identifers
                     hadErrors |= logError<true>(ErrCode::PARSER_UNSPEC, "GOTO without label");
                     goto DONE_WITH_FILE;
                  }
                  advance();

                  gotoEOS();

               case KeywordID::TRY:
                  advance();
                  consumeEnclosedStmts(tree, tokIt, endtok); //contents
                  
                  advance();
                  if (tokIt->type() != TokenType::CHAR || decodeKeyword(*tokIt) != KeywordID::CATCH) {
                     hadErrors |= logError<true>(ErrCode::PARSER_UNSPEC, "TRY without CATCH");
                     goto DONE_WITH_FILE;
                  }
                  [[fallthrough]];
               case KeywordID::FOR: [[fallthrough]];
               case KeywordID::WHILE: [[fallthrough]];
               case KeywordID::SWITCH: [[fallthrough]];
               case KeywordID::MATCH:
                  advance();
                  consumeSpecificBlock(BlockType::PARENS, tree, tokIt, endtok); //condition
                  advance();
                  consumeEnclosedStmts(tree, tokIt, endtok); //contents

                  goto DONE_WITH_TOKEN;

               case KeywordID::DO:
                  advance();
                  consumeEnclosedStmts(tree, tokIt, endtok); //contents
                  
                  //!WHILE
                  advance();
                  if (tokIt->type() != TokenType::CHAR || decodeKeyword(*tokIt) != KeywordID::WHILE) {
                     hadErrors |= logError<true>(ErrCode::PARSER_UNSPEC, "DO without a WHILE");
                     goto DONE_WITH_FILE;
                  }

                  advance();
                  consumeSpecificBlock(BlockType::PARENS, tree, tokIt, endtok); //condition
                  advance();

                  //EOS
                  gotoEOS();
               
               case KeywordID::IF: HANDLE_IF:
                  advance();
                  consumeSpecificBlock(BlockType::PARENS, tree, tokIt, endtok); //condition
                  advance();
                  consumeEnclosedStmts(tree, tokIt, endtok); //contents
                  advance();

                  //check for else
                  if (tokIt->type() != TokenType::CHAR || decodeKeyword(*tokIt) != KeywordID::ELSE) {
                     goto DONE_WITH_TOKEN;
                  }

                  //!ELSE
                  advance();
                  switch (tokIt->type()) {
                     case TokenType::BLOC: //else
                        consumeEnclosedStmts(tree, tokIt, endtok);
                        advance();
                        goto DONE_WITH_TOKEN;
                     case TokenType::CHAR: //else if
                        if (decodeKeyword(*tokIt) == KeywordID::IF) { //jump to IF handler code
                           goto HANDLE_IF;
                        }
                        [[fallthrough]];
                     default: //other - ERROR
                        hadErrors |= logError<true>(ErrCode::PARSER_UNSPEC, "bad ELSE");
                        goto DONE_WITH_FILE;
                  }

               //LOOP CONTROL - break and continue
               case KeywordID::BREAK: [[fallthrough]];
               case KeywordID::CONTINUE:
                  //check that in a loop or switch statement //!NOTE: UNFINISHED
                  advance();
                  gotoEOS();

               //SWITCH/MATCH CONTROL - case and default
               case KeywordID::CASE:
                  consumeExpression(tree, tokIt, endtok);
                  advance();
                  [[fallthrough]];
               case KeywordID::DEFAULT:
                  if (tokIt->type() != TokenType::OP || OPERATORS[*tokIt] != LABEL_DELIM) {
                     hadErrors |= logError<true>(ErrCode::PARSER_UNSPEC, "bad CASE");
                     goto DONE_WITH_FILE;
                  }
                  advance();
                  goto DONE_WITH_TOKEN;

               case KeywordID::THROW:
                  consumeExpression(tree, tokIt, endtok);
                  advance();
                  gotoEOS();

                  
               //floating keywords
               case KeywordID::CATCH: [[fallthrough]];
               case KeywordID::ELSE:
                  hadErrors |= logError<true>(ErrCode::PARSER_UNSPEC, "floating keyword (%u)", (uint)keyword);
                  goto DONE_WITH_FILE;

               #pragma endregion controlflow


               //!OBJECT TYPE KEYWORDS
               #pragma region objtype
               case KeywordID::CLASS: [[fallthrough]];
               case KeywordID::STRUCT: [[fallthrough]];
               case KeywordID::UNION: [[fallthrough]];
               case KeywordID::ENUM: [[fallthrough]];
               case KeywordID::MASK: [[fallthrough]];
               case KeywordID::NAMESPACE:
                  advance() //consume keyword
                  switch (tokIt->type()) { //handle keyword
                     case TokenType::IDEN: [[fallthrough]];
                     case TokenType::STRT: [[fallthrough]];
                     case TokenType::CHAR: //named object type
                        consumeIdentifier(tree, tokIt, endtok); //name
                        advance();
                        switch (tokIt->type()) {
                           default:
                              hadErrors |= logError<true>(ErrCode::PARSER_UNSPEC, "bad object type declaration");
                              goto DONE_WITH_FILE;

                           case TokenType::EOS: //forward declaration
                              goto EOS;
                           
                           case TokenType::BLOC: //definition
                        }
                        [[fallthrough]];
                     case TokenType::BLOC: //anonymous object type
                        consumeEnclosedStmts(tree, tokIt, endtok);
                        advance();
                        
                        gotoEOS();


                     default:
                        hadErrors |= logError<true>(ErrCode::PARSER_UNSPEC, "bad object type keyword");
                        goto DONE_WITH_FILE;
                  }



               #pragma endregion objtype
               

               //!OTHER
               #pragma region other
               case KeywordID::RETURN:
                  //check if in a function //!NOTE: UNFINISHED

                  advance();

                  //consume return value
                  consumeExpression(tree, tokIt, endtok);
                  advance();

                  //EOS
                  gotoEOS();

               //ASSERT and STATIC_ASSERT
               case KeywordID::ASSERT: [[fallthrough]];
               case KeywordID::STATIC_ASSERT:
                  advance();
                  consumeExpression(tree, tokIt, endtok);
                  advance();

                  gotoEOS();


               //ASM (inline assembly)
               case KeywordID::ASM:
                  //!NOTE: UNFINISHED
                  consumeSpecificBlock(BlockType::PARENS, tree, tokIt, endtok); //parameters
                  advance();
                  consumeSpecificBlock(BlockType::INIT_LIST, tree, tokIt, endtok); //contents //!NOTE: intentionally doesn't use consumeEnclosedStmts
                  advance();
                  gotoEOS();

               //dynamic allocation/deallocation
               case KeywordID::NEW:
                  advance();
                  consumeIdentifier(tree, tokIt, endtok);
                  advance();
                  gotoEOS();
               case KeywordID::DELETE:
                  advance();
                  consumeExpression(tree, tokIt, endtok);
                  advance();
                  gotoEOS();

               //lvalues - treat as identifier
               case KeywordID::THIS: [[fallthrough]];
               case KeywordID::SELF:
                  goto HANDLE_IDENTIFIER;

               //rvalues - log the error and continue as if it was a standard identifier
               case KeywordID::TRUE: [[fallthrough]];
               case KeywordID::FALSE: [[fallthrough]];
               case KeywordID::NULLPTR:
                  hadErrors |= logError<false>(ErrCode::PARSER_UNSPEC, "expression cannot begin with an rvalue");
                  goto HANDLE_IDENTIFIER;

               #pragma endregion other


               //!too many handled identically to be worth that many labels
               //!NOTE: MIGHT BE WORTH THE LABELS ANYWAY
               #pragma region categories
               default:{
                  //!TYPES
                  #pragma region typeKeywords
                  if (isType(keyword)) {
                     //!NOTE: UNFINISHED
                     //check for illegal uses of typename keywords (DO NOT advance())

                     //handle typename
                     goto HANDLE_TYPENAME;
                  }

                  #pragma endregion typeKeywords

                  if (isCast(keyword)) {
                     advance();
                     consumeSpecificBlock(BlockType::SPECIALIZER, tree, tokIt, endtok); //!NOTE: make it consume a type in a specializer
                     advance();
                     consumeSpecificBlock(BlockType::PARENS, tree, tokIt, endtok); //!NOTE: make it consume an expression in parens

                     goto DONE_WITH_TOKEN;
                  }

                  //!NOTE: IF THIS CODE GET REACHED THERE IS AN ERROR IN THE CLEF CODE
                  throwError(ErrCode::PARSER_NOT_IMPLEMENTED, "keyword does not have a case and is not handled as a member of a category (%u)", (uint)keyword);
                  assert(false);
               }
               #pragma endregion categories



               //!NOTE: UNFINISHED (still need to handle most of the keywords)

               

               //!NON-KEYWORDS (IDENTIFIERS)
               case KeywordID::_NOT_A_KEYWORD: { HANDLE_IDENTIFIER:
                  //!NOTE: UNFINISHED

                  #pragma region typename
                  HANDLE_TYPENAME:
                  consumeIdentifier(tree, tokIt, endtok);
                  //!NOTE: UNFINISHED

                  #pragma endregion typename
               }
            }


            goto DONE_WITH_TOKEN;
}

         //!NUMBERS:
         case TokenType::NUM: [[fallthrough]];
         case TokenType::DGIT: [[fallthrough]];
         case TokenType::XDGT:
            // tree.pushNode(Literal{mcsl::str_to_uint(tokIt->begin(), tokIt->size())});
            goto DONE_WITH_TOKEN;
            
         default:
            //SYNTAX ERROR - ILLEGAL TOKEN
            hadErrors |= logError<true>(ErrCode::PARSER_UNSPEC, "illegal token");
            goto DONE_WITH_FILE;
      }


      DONE_WITH_TOKEN:
   }


   //final error checks
   if (!tree.isComplete()) {
      hadErrors |= logError<true>(ErrCode::PARSER_UNSPEC, "end of file reached at illegal spot");
   }


   DONE_WITH_FILE:
   if (hadErrors) {
      throwError(ErrCode::PARSER_UNSPEC, "error earlier in file, compilation continued only until end of file");
      return SyntaxTree{};
   }
   
   //return the tree
   return tree;
}
#undef advance
#undef gotoEOS

#endif //PARSER_CPP