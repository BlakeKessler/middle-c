#ifndef PARSE_LOOPS_CPP
#define PARSE_LOOPS_CPP

#include "Parser.hpp"

clef::Loop* clef::Parser::parseForLoop() {
   //open parens
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN, "FOR loop without opening parens for condition");

   //params
   Stmt* decl = parseStmt();
   Stmt* condition = parseStmt();
   Expr* inc = parseExpr();

   //closing parens
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::CLOSE, "FOR loop without closing parens for condition");

   //procedure
   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad FOR block");
   Scope* proc = parseProcedure();

   //EOS
   consumeEOS("FOR statement without EOS token");

   //return
   ForLoopParams* params = new (tree.allocNode(NodeType::FOREACH_LOOP_PARAMS)) ForLoopParams{decl, condition, inc};
   Loop* loop = new (tree.allocNode(NodeType::LOOP)) Loop{params, proc};

   return loop;
}

clef::Loop* clef::Parser::parseForeachLoop() {
   //open parens
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN, "FOREACH loop without opening parens for condition");

   //iterator declaration
   TypeQualMask quals = parseQuals();
   Type* itType = parseTypename();
   Identifier* itName = parseIdentifier();

   //IN operator
   consumeOperator(OpID::LABEL_DELIM, "bad FOREACH params");

   //FOREACH target
   Expr* target = parseExpr();

   //closing parens
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::CLOSE, "FOREACH loop without closing parens for condition");

   //procedure
   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad FOREACH block");
   Scope* proc = parseProcedure();

   //EOS
   consumeEOS("FOREACH statement without EOS token");

   //return
   Decl* decl = new (tree.allocNode(NodeType::DECL)) Decl{itType, itName};
   ForeachLoopParams* params = new (tree.allocNode(NodeType::FOREACH_LOOP_PARAMS)) ForeachLoopParams{decl, target};
   Loop* loop = new (tree.allocNode(NodeType::LOOP)) Loop{params, proc};

   return loop;
}

clef::Loop* clef::Parser::parseWhileLoop() {
   //open parens
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN, "WHILE loop without opening parens for condition");

   //condition
   Expr* condition = parseExpr();

   //closing parens
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::CLOSE, "WHILE loop without closing parens for condition");

   //procedure
   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad WHILE block");
   Scope* proc = parseProcedure();

   //EOS
   consumeEOS("WHILE statement without EOS token");

   //return
   return new (tree.allocNode(NodeType::LOOP)) Loop{OpID::WHILE, condition, proc};
}

clef::Loop* clef::Parser::parseDoWhileLoop() {
   //procedure
   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad DO WHILE block");
   Scope* proc = parseProcedure();

   //WHILE keyword
   consumeKeyword(KeywordID::WHILE, "DO WHILE without WHILE");

   //condition
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN, "DO WHILE loop without opening parens for condition");
   Expr* condition = parseExpr();
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::CLOSE, "DO WHILE loop without closing parens for condition");

   //EOS
   consumeEOS("DO WHILE statement without EOS token");

   //return
   return new (tree.allocNode(NodeType::LOOP)) Loop{OpID::DO_WHILE, condition, proc};
}

#endif //PARSE_LOOPS