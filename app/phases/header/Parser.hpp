#ifndef PARSER_HPP
#define PARSER_HPP

#include "CLEF.hpp"

#include "Lexer.hpp"
#include "SyntaxTree.hpp"
#include "Token.hpp"

class clef::Parser {
   private:
      Lexer& _toks;
      SyntaxTree& tree;
      Token currTok;
      struct {
         Symbol* scope;
         Symbol* func;
         Symbol* type;
      } env;

      Parser(SyntaxTree& ast, Lexer& toks): _toks{toks}, tree{ast} {}
   protected:
      [[gnu::noreturn]] void logError(Token tok, ErrCode code, const mcsl::str_slice fmt, mcsl::Printable auto... argv) {
         _toks.logError(tok, code, fmt, std::forward<decltype(argv)>(argv)...);
      }

      void nextToken();

      res<void> consumeKeyword(KeywordID kw);
      res<void> consumeOp(Oplike);
      res<void> consumeBlockDelim(BlockType, BlockDelimRole);
      res<void> consumeEOS();

      res<Expr*> parseCast(KeywordID);
      res<Args*> parseArgList(BlockType, bool isDecl);
      
      res<Expr*> parseExpr();
      res<Expr*> parseCoreExpr();

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

      template<typename T> T expect(res<T> result, Token tok, ErrCode code, const mcsl::str_slice fmt, mcsl::Printable auto... argv) {
         if (result.is_ok()) {
            return result.ok();
         } else {
            logError(tok, code, fmt, std::forward<decltype(argv)>(argv)...);
         }
      }
      template<typename T> T expect(res<T> result, ErrCode code, const mcsl::str_slice fmt, mcsl::Printable auto... argv) {
         if (result.is_ok()) {
            return result.ok();
         } else {
            logError(currTok, code, fmt, std::forward<decltype(argv)>(argv)...);
         }
      }
   public:
      static Parser parseSource(Source&&, SyntaxTree&);
      static Parser parseFile(mcsl::File&, SyntaxTree&);
      static Parser parseFile(const mcsl::str_slice, SyntaxTree&);
};

#endif