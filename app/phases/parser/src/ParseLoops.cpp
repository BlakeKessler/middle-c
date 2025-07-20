#ifndef PARSE_LOOPS_CPP
#define PARSE_LOOPS_CPP

#include "Parser.hpp"

//parse a for loop (assumes `for` keyword has already been consumed)
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
   consumeBlockDelim(BlockType::LIST, BlockDelimRole::OPEN, "bad FOR block");
   index<Scope> proc = parseProcedure();

   //EOS
   consumeEOS("FOR statement without EOS token");

   //return
   index<ForLoop> loop = make<ForLoop>(decl, condition, inc, proc);

   return loop;
}

//parse a foreach loop (assumes `foreach` keyword has already been consumed)
clef::index<clef::ForeachLoop> clef::Parser::parseForeachLoop() {
   //open parens
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN, "FOREACH loop without opening parens for condition");

   //iterator declaration
   index<Decl> decl = parseParam(tryParseAttrs());

   //IN operator
   consumeOperator(OpID::LABEL_DELIM, "bad FOREACH params");

   //FOREACH target
   index<Expr> target = parseExpr();

   //closing parens
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::CLOSE, "FOREACH loop without closing parens for condition");

   //procedure
   consumeBlockDelim(BlockType::LIST, BlockDelimRole::OPEN, "bad FOREACH block");
   index<Scope> proc = parseProcedure();

   //EOS
   consumeEOS("FOREACH statement without EOS token");

   //return
   index<ForeachLoop> loop = make<ForeachLoop>(decl, target, proc);

   return loop;
}

//parse a while loop (assumes `while` keyword has already been consumed)
clef::index<clef::WhileLoop> clef::Parser::parseWhileLoop() {
   //open parens
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::OPEN, "WHILE loop without opening parens for condition");

   //condition
   index<Expr> condition = parseExpr();

   //closing parens
   consumeBlockDelim(BlockType::CALL, BlockDelimRole::CLOSE, "WHILE loop without closing parens for condition");

   //procedure
   consumeBlockDelim(BlockType::LIST, BlockDelimRole::OPEN, "bad WHILE block");
   index<Scope> proc = parseProcedure();

   //EOS
   consumeEOS("WHILE statement without EOS token");

   //return
   return make<WhileLoop>(condition, proc);
}

//parse a do-while loop (assumes `do` keyword has already been consumed)
clef::index<clef::DoWhileLoop> clef::Parser::parseDoWhileLoop() {
   //procedure
   consumeBlockDelim(BlockType::LIST, BlockDelimRole::OPEN, "bad DO WHILE block");
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
   return make<DoWhileLoop>(condition, proc);
}

#endif //PARSE_LOOPS