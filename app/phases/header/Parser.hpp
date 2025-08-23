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
         Identifier scope;
         Identifier fn;
         Identifier type;
      } env;

      Parser(SyntaxTree& ast, Lexer& toks): _toks{toks}, tree{ast} {}
   protected:
      enum IsDecl {
         NO,
         YES,
         MAYBE
      };

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
      template<IsDecl isDecl> Args* parseArgList(BlockType);
      
      Expr* parseExpr();
      Expr* parseExprCore();
      Expr* parseStmt();

      res<Expr*> parseInit(Identifier typeName);

      Attr* parseAttr();
      Attr* parseAttrs();
      
      res<Label> parseLabel();
      template<IsDecl isDecl> res<Identifier> parseIden(Identifier type, Symbol::Type t);
      Expr* parseDecl();
      Expr* parseParam();
      Expr* parseUsing();
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
      res<Symbol*> registerSymbol(const mcsl::str_slice name, Symbol::Type t);
      Symbol* registerSymbolAnon(Symbol::Type t);
      res<TypeSpec*> intoType(Symbol*, TypeSpec::Metatype);

      Proc* parseProc();
      
      Expr* parseIf();
      Expr* parseWhile();
      Expr* parseDoWhile();
      Expr* parseFor();
      Expr* parseForeach();
      Expr* parseSwitch();
      Expr* parseMatch();
      Expr* parseASM() { TODO; }
      
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
      template<typename T> T expect(res<T> result, Token tok, const mcsl::str_slice fmt, mcsl::Printable auto... argv) {
         if (result.is_ok()) {
            return result.ok();
         } else {
            logError(tok, result.err(), fmt, std::forward<decltype(argv)>(argv)...);
         }
      }
      template<typename T> T expect(res<T> result, const mcsl::str_slice fmt, mcsl::Printable auto... argv) {
         if (result.is_ok()) {
            return result.ok();
         } else {
            logError(currTok, result.err(), fmt, std::forward<decltype(argv)>(argv)...);
         }
      }
   public:
      static Parser ParseSource(Source&&, SyntaxTree&);
      static Parser ParseFile(mcsl::File&, SyntaxTree&);
      static Parser ParseFile(const mcsl::str_slice, SyntaxTree&);
};

template<> clef::res<clef::Identifier> clef::Parser::parseIden<clef::Parser::NO>(Identifier, Symbol::Type);
template<> clef::res<clef::Identifier> clef::Parser::parseIden<clef::Parser::YES>(Identifier, Symbol::Type);
template<> clef::res<clef::Identifier> clef::Parser::parseIden<clef::Parser::MAYBE>(Identifier, Symbol::Type);
template<> clef::Args* clef::Parser::parseArgList<clef::Parser::NO>(BlockType);
template<> clef::Args* clef::Parser::parseArgList<clef::Parser::YES>(BlockType);
template<> clef::Args* clef::Parser::parseArgList<clef::Parser::MAYBE>(BlockType);

#endif