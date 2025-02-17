#ifndef PARSE_LOOPS_CPP
#define PARSE_LOOPS_CPP

#include "Parser.hpp"

clef::index<clef::ForLoop> clef::Parser::parseForLoop() {
   //open parens
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN, "FOR loop without opening parens for condition");

   //params
   index<Stmt> decl = parseStmt();
   index<Stmt> condition = parseStmt();
   index<Expr> inc = parseExpr();

   //closing parens
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::CLOSE, "FOR loop without closing parens for condition");

   //procedure
   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad FOR block");
   index<Scope> proc = parseProcedure();

   //EOS
   consumeEOS("FOR statement without EOS token");

   //return
   index<ForLoopParams> params = tree.make<ForLoopParams>(decl, condition, inc);
   index<ForLoop> loop = tree.make<ForLoop>(params, proc);

   return loop;
}

clef::index<clef::ForeachLoop> clef::Parser::parseForeachLoop() {
   //open parens
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN, "FOREACH loop without opening parens for condition");

   //iterator declaration
   index<Variable> decl = parseParam();

   //IN operator
   consumeOperator(OpID::LABEL_DELIM, "bad FOREACH params");

   //FOREACH target
   index<Expr> target = parseExpr();

   //closing parens
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::CLOSE, "FOREACH loop without closing parens for condition");

   //procedure
   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad FOREACH block");
   index<Scope> proc = parseProcedure();

   //EOS
   consumeEOS("FOREACH statement without EOS token");

   //return
   index<ForeachLoopParams> params = tree.make<ForeachLoopParams>(decl, target);
   index<ForeachLoop> loop = tree.make<ForeachLoop>(params, proc);

   return loop;
}

clef::index<clef::WhileLoop> clef::Parser::parseWhileLoop() {
   //open parens
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN, "WHILE loop without opening parens for condition");

   //condition
   index<Expr> condition = parseExpr();

   //closing parens
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::CLOSE, "WHILE loop without closing parens for condition");

   //procedure
   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad WHILE block");
   index<Scope> proc = parseProcedure();

   //EOS
   consumeEOS("WHILE statement without EOS token");

   //return
   return tree.make<WhileLoop>(condition, proc);
}

clef::index<clef::DoWhileLoop> clef::Parser::parseDoWhileLoop() {
   //procedure
   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad DO WHILE block");
   index<Scope> proc = parseProcedure();

   //WHILE keyword
   consumeKeyword(KeywordID::WHILE, "DO WHILE without WHILE");

   //condition
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN, "DO WHILE loop without opening parens for condition");
   index<Expr> condition = parseExpr();
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::CLOSE, "DO WHILE loop without closing parens for condition");

   //EOS
   consumeEOS("DO WHILE statement without EOS token");

   //return
   return tree.make<DoWhileLoop>(condition, proc);
}

#endif //PARSE_LOOPS