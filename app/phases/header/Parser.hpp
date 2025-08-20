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
      Token prevTok;
      struct Env {
         Symbol* scope;
         Symbol* fn;
         Symbol* type;
      } env;

      Parser(SyntaxTree& ast, Lexer& toks): _toks{toks}, tree{ast} {}
   protected:
      [[gnu::noreturn]] void logError(Token tok, ErrCode code, const mcsl::str_slice fmt, mcsl::Printable auto... argv) {
         _toks.logError(tok, code, fmt, std::forward<decltype(argv)>(argv)...);
      }

      void nextToken();

      res<void> readKeyword(KeywordID kw);
      res<void> readOp(Oplike);
      res<void> readBlockDelim(BlockType, BlockDelimRole);
      res<void> readEOS();

      bool isKeyword(KeywordID kw);
      bool isOp(Oplike);
      bool isBlockDelim(BlockType, BlockDelimRole);
      bool isEOS();

      Expr* parseCast(KeywordID);
      template<bool isDecl> Args* parseArgList(BlockType);
      
      Expr* parseExpr();
      Expr* parseCoreExpr();

      Attr* parseAttr();
      Attr* parseAttrs();
      
      res<Label> parseLabel();
      template<bool isDecl> res<Identifier> parseIden(Identifier type);
      Expr* parseDecl();
      Expr* parseParam();
      Identifier parseType();
      Identifier parseTypeDef();
      Identifier parseTypeDef(KeywordID);
         Identifier parseClass();
         Identifier parseStruct();
         Identifier parseTrait();
         Identifier parseUnion();
         Identifier parseEnum();
         Identifier parseEnumunion();
         Identifier parseMask();
         Identifier parseNamespace();
         Identifier parseTuple();
      Symbol* registerType(Symbol::Type t, Identifier name);

      Proc* parseProc();
      
      Expr* parseIf();
      Expr* parseWhile();
      Expr* parseDoWhile();
      Expr* parseFor();
      Expr* parseForeach();
      Expr* parseSwitch();
      Expr* parseMatch();
      
      mcsl::pair<Func*, Overload*> parseFunc();
      mcsl::pair<Macro*, Overload*> parseMacro();

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
      static Parser ParseSource(Source&&, SyntaxTree&);
      static Parser ParseFile(mcsl::File&, SyntaxTree&);
      static Parser ParseFile(const mcsl::str_slice, SyntaxTree&);
};

template<> clef::res<clef::Identifier> clef::Parser::parseIden<true>(Identifier);
template<> clef::res<clef::Identifier> clef::Parser::parseIden<false>(Identifier);
template<> clef::Args* clef::Parser::parseArgList<true>(BlockType);
template<> clef::Args* clef::Parser::parseArgList<false>(BlockType);

#endif