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

      res<void> readKeyword(KeywordID kw);
      res<void> readOp(Oplike);
      res<void> readBlockDelim(BlockType, BlockDelimRole);
      res<void> readEOS();

      bool isKeyword(KeywordID kw);
      bool isOp(Oplike);
      bool isBlockDelim(BlockType, BlockDelimRole);
      bool isEOS();

      res<Expr*> parseCast(KeywordID);
      res<Args*> parseArgList(BlockType, bool isDecl);
      
      Expr* parseExpr();
      Expr* parseCoreExpr();

      Attr* parseAttr();
      Attr* parseAttrs();
      
      res<Label> parseLabel();
      res<Identifier> parseIden(Identifier type);
      Expr* parseDecl();
      Expr* parseParam();
      mcsl::pair<Identifier, TypeSpec*> parseType();
      mcsl::pair<Identifier, TypeSpec*> parseTypeDef();
      mcsl::pair<Identifier, TypeSpec*> parseTypeDef(KeywordID);
         mcsl::pair<Identifier, TypeSpec*> parseClass();
         mcsl::pair<Identifier, TypeSpec*> parseStruct();
         mcsl::pair<Identifier, TypeSpec*> parseTrait();
         mcsl::pair<Identifier, TypeSpec*> parseUnion();
         mcsl::pair<Identifier, TypeSpec*> parseEnum();
         mcsl::pair<Identifier, TypeSpec*> parseEnumunion();
         mcsl::pair<Identifier, TypeSpec*> parseMask();
         mcsl::pair<Identifier, TypeSpec*> parseNamespace();
         mcsl::pair<Identifier, TypeSpec*> parseTuple();
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
      static Parser parseSource(Source&&, SyntaxTree&);
      static Parser parseFile(mcsl::File&, SyntaxTree&);
      static Parser parseFile(const mcsl::str_slice, SyntaxTree&);
};

#endif