#ifndef PARSER_HPP
#define PARSER_HPP

#include "CLEF.hpp"
#include "SyntaxTree.hpp"

class clef::Parser {
   private:
      SyntaxTree& _tree;
      Lexer& _toks;

      Parser(SyntaxTree& tree, Lexer& toks):_tree{tree},_toks{toks} {}
   protected:
      res<void> consumeKeyword(KeywordID kw);
      res<void> consumeOp(OpID);
      
      res<Expr*> parseExpr();
      res<Expr*> parseExprImpl(ubyte prec, bool isLeftAssoc);

      res<Attr*> parseAttr();
      res<Attr*> parseAttrs();
      
      res<Label> parseLabel();
      res<Identifier> parseIden();
      res<mcsl::pair<Identifier, TypeSpec*>> parseType();
      res<mcsl::pair<Identifier, TypeSpec*>> parseTypeDef();

      res<Proc*> parseProc();
      
      res<Expr*> parseIf();
      res<Expr*> parseWhile();
      res<Expr*> parseDoWhile();
      res<Expr*> parseFor();
      res<Expr*> parseForeach();
      res<Expr*> parseSwitch();
      res<Expr*> parseMatch();
      
      res<mcsl::pair<Func*, Overload*>> parseFunc();
      res<mcsl::pair<Method*, Overload*>> parseMethod();
      res<mcsl::pair<Macro*, Overload*>> parseMacro();

      template<typename T> T expect(res<T> val, Token badTok, const mcsl::str_slice fmt, mcsl::Printable auto... argv) {
         if (val.is_ok()) {
            return val.ok();
         } else {
            TODO;
         }
      }
   public:
      static Parser parseSource(Source&&);
      static Parser parseFile(mcsl::File&);
};

#endif