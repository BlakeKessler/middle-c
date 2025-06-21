#ifndef PARSE_OBJ_TYPES_CPP
#define PARSE_OBJ_TYPES_CPP

#include "Parser.hpp"

clef::index<clef::TypeDecl> clef::Parser::__parseObjTypeImpl(clef::SymbolType symbolType, const mcsl::str_slice metatypeName) {
   index<Identifier> name = tryParseIdentifier(symbolType, nullptr);
   
   if (tryConsumeEOS()) { //forward declaration
      if (!name) {
         logError(ErrCode::BAD_DECL, "cannot forward-declare an anonymous class/struct");
      }
      return tree.make<TypeDecl>(name);
   }
   
   SymbolNode* symbol = tree[name].symbol(); debug_assert(symbol);
   TypeSpec* spec = symbol->type();          debug_assert(spec);
   if (spec->metaType() != TypeSpec::COMPOSITE) {
      logError(ErrCode::BAD_TYPE_DECL, "redeclaration of %s `%s`", metatypeName, *symbol);
   }
   PUSH_SCOPE;

   //implemented interfaces
   if (tryConsumeOperator(OpID::LABEL_DELIM)) {
      do {
         index<Identifier> parentType = parseTypename(SymbolType::INTERFACE, false);
         if (!spec->composite().impls.insert(tree[parentType].symbol())) {
            logError(ErrCode::BAD_TYPE_DECL, "%s `%s` already implements interface `%s`", metatypeName, *tree[parentType].symbol());
         }
      } while (tryConsumeOperator(OpID::COMMA));
   }

   //definition
   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "class/struct definition");
   QualMask scope = QualMask::PRIVATE;
   while (!tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::CLOSE)) {
      if (tryConsumeKeyword(KeywordID::PUBLIC)) {
         scope = QualMask::PUBLIC;
         consumeOperator(OpID::LABEL_DELIM, "invalid PUBLIC label");
         continue;
      }
      if (tryConsumeKeyword(KeywordID::PRIVATE)) {
         scope = QualMask::PRIVATE;
         consumeOperator(OpID::LABEL_DELIM, "invalid PRIVATE label");
         continue;
      }
      if (tryConsumeKeyword(KeywordID::PROTECTED)) {
         scope = QualMask::PROTECTED;
         consumeOperator(OpID::LABEL_DELIM, "invalid PROTECTED label");
         continue;
      }

      bool isStatic = tryConsumeKeyword(KeywordID::STATIC);
      if (currTok.type() != TokenType::KEYWORD) {
         logError(ErrCode::BAD_STMT, "invalid statement in %s definition", metatypeName);
      }
      switch (currTok.keywordID()) {
         #define KW_CASE(kw, parsingFunc) case KeywordID::kw: getNextToken(); if (isStatic) { logError(ErrCode::BAD_KEYWORD, "cannot qualify a " #kw " as static"); } { auto tmp = parsingFunc(); tree[(index<Identifier>)tmp].addQuals(scope); spec->composite().subtypes.insert(tree[tree[tmp].name()].symbol()); symbol->insert(tree[tree[tmp].name()].symbol()); } break
         KW_CASE(CLASS, parseClass);
         KW_CASE(STRUCT, parseStruct);
         KW_CASE(INTERFACE, parseInterface);
         KW_CASE(UNION, parseUnion);
         KW_CASE(ENUM, parseEnum);
         KW_CASE(MASK, parseMask);
         KW_CASE(NAMESPACE, parseNamespace);
         #undef KW_CASE
         case KeywordID::FUNC: getNextToken(); {auto tmp = parseFunction(); tree[(index<Identifier>)tmp].addQuals(scope); (isStatic ? spec->composite().staticFuncs : spec->composite().methods).insert(tree[tmp].symbol()); symbol->insert(tree[tmp].symbol());} break;
         case KeywordID::LET : getNextToken(); {auto tmp = parseDecl(); tree[(index<Identifier>)(tree[tmp].name())].addQuals(scope); (isStatic ? spec->composite().staticMembs : spec->composite().dataMembs).push_back(tree[tree[tmp].name()].symbol()); symbol->insert(tree[tree[tmp].name()].symbol());} break;
         default: logError(ErrCode::BAD_STMT, "invalid statement in class/struct definition");
      }
   }

   //EOS
   consumeEOS("class/struct declaration without EOS");

   //return
   POP_SCOPE;
   return tree.make<TypeDecl>(name, name);
}

clef::index<clef::TypeDecl> clef::Parser::parseInterface() {
   index<Identifier> name = parseIdentifier(SymbolType::INTERFACE, nullptr);
   
   if (tryConsumeEOS()) { //forward declaration
      return tree.make<TypeDecl>(name);
   }
   
   SymbolNode* symbol = tree[name].symbol(); debug_assert(symbol);
   TypeSpec* spec = symbol->type();          debug_assert(spec);
   if (spec->metaType() != TypeSpec::COMPOSITE) {
      logError(ErrCode::BAD_TYPE_DECL, "redeclaration of interface `%s`", *symbol);
   }
   PUSH_SCOPE;
   
   //inheritance
   if (tryConsumeOperator(OpID::LABEL_DELIM)) {
      do {
         index<Identifier> parentType = parseTypename(SymbolType::INTERFACE, false);
         if (!spec->composite().impls.insert(tree[parentType].symbol())) {
            logError(ErrCode::BAD_TYPE_DECL, "interface `%s` already extends interface `%s`", *tree[parentType].symbol());
         }
      } while (tryConsumeOperator(OpID::COMMA));
   }

   //definition
   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad INTERFACE definition");
   QualMask scope = QualMask::PUBLIC;
   while (!tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::CLOSE)) {
      if (tryConsumeKeyword(KeywordID::PUBLIC)) {
         scope = QualMask::PUBLIC;
         consumeOperator(OpID::LABEL_DELIM, "invalid PUBLIC label");
         continue;
      }
      if (tryConsumeKeyword(KeywordID::PRIVATE)) {
         scope = QualMask::PRIVATE;
         consumeOperator(OpID::LABEL_DELIM, "invalid PRIVATE label");
         continue;
      }
      if (tryConsumeKeyword(KeywordID::PROTECTED)) {
         scope = QualMask::PROTECTED;
         consumeOperator(OpID::LABEL_DELIM, "invalid PROTECTED label");
         continue;
      }
      bool isStatic = tryConsumeKeyword(KeywordID::STATIC);
      consumeKeyword(KeywordID::FUNC, "interfaces can only contain functions and methods");
      index<Identifier> func = parseFunction();
      tree[func].addQuals(scope);
      if (isStatic) {
         spec->composite().staticFuncs.insert(tree[func].symbol());
      } else {
         spec->composite().methods.insert(tree[func].symbol());
      }
      symbol->insert(tree[func].symbol());
   }

   //EOS
   consumeEOS("interface without EOS");

   //return
   POP_SCOPE;
   return tree.make<TypeDecl>(name, name);
}

clef::index<clef::TypeDecl> clef::Parser::parseUnion() {
   index<Identifier> name = tryParseIdentifier(SymbolType::UNION, nullptr);

   if (tryConsumeEOS()) { //forward declaration
      if (!name) {
         logError(ErrCode::BAD_DECL, "cannot forward-declare an anonymous union");
      }
      return tree.make<TypeDecl>(name);
   }

   SymbolNode* symbol = tree[name].symbol(); debug_assert(symbol);
   TypeSpec* spec = symbol->type();          debug_assert(spec);
   if (spec->metaType() != TypeSpec::COMPOSITE) {
      logError(ErrCode::BAD_TYPE_DECL, "redeclaration of union `%s`", *symbol);
   }
   PUSH_SCOPE;

   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad union definition");

   while (!tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::CLOSE)) {
      //parse member
      index<Decl> member = parseParam();
      consumeEOS("bad union member");
      
      //push to members list
      spec->composite().dataMembs.push_back(tree[tree[member].name()].symbol());
      symbol->insert(tree[tree[member].name()].symbol());
   }

   //EOS
   consumeEOS("union without EOS");

   //return
   POP_SCOPE;
   return tree.make<TypeDecl>(name, name);
}

clef::index<clef::TypeDecl> clef::Parser::__parseEnumlikeImpl(SymbolType symbolType, const mcsl::str_slice metatypeName) {
   index<Identifier> name = tryParseIdentifier(symbolType, nullptr);

   if (tryConsumeEOS()) { //forward declaration
      if (!name) {
         logError(ErrCode::BAD_DECL, "cannot forward-declare an anonymous enum/mask");
      }
      return tree.make<TypeDecl>(name);
   }

   SymbolNode* symbol = tree[name].symbol(); debug_assert(symbol);
   TypeSpec* spec = symbol->type();          debug_assert(spec);
   if (spec->metaType() != TypeSpec::COMPOSITE) {
      logError(ErrCode::BAD_TYPE_DECL, "redeclaration of %s `%s`", metatypeName, *symbol);
   }
   PUSH_SCOPE;

   index<Identifier> baseType;
   if (tryConsumeOperator(OpID::LABEL_DELIM)) {
      baseType = parseTypename(SymbolType::FUND_TYPE, false);
   } else { baseType = 0; }


   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad enum/mask definition");
   if (!tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::CLOSE)) {
      do {
         index<Identifier> enumerator = parseIdentifier(SymbolType::VAR, symbol);
         index<Expr> val;
         if (tryConsumeOperator(OpID::ASSIGN)) {
            val = parseExprNoPrimaryComma();
            TODO;
         } else { val = 0; }

         spec->composite().staticMembs.push_back(tree[enumerator].symbol());
         symbol->insert(tree[enumerator].symbol());
      } while (tryConsumeOperator(OpID::COMMA));
      consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::CLOSE, "bad ENUM enumerator");
   }
   
   //EOS
   consumeEOS("enum/mask without EOS");

   //return
   POP_SCOPE;
   return tree.make<TypeDecl>(name, name);
}

//!TODO: implement enumunions
clef::index<clef::TypeDecl> clef::Parser::parseEnumUnion() {
   logError(ErrCode::PARSER_NOT_IMPLEMENTED, "enumunions are not yet supported");
}

clef::index<clef::TypeDecl> clef::Parser::parseNamespace() {
   index<Identifier> name = tryParseIdentifier(SymbolType::NAMESPACE, nullptr);
   
   if (tryConsumeEOS()) { //forward declaration
      if (!name) {
         logError(ErrCode::BAD_DECL, "cannot forward-declare an anonymous namespace");
      }
      return tree.make<TypeDecl>(name);
   }
   
   SymbolNode* symbol = tree[name].symbol(); debug_assert(symbol);
   TypeSpec* spec = symbol->type();          debug_assert(spec);
   if (spec->metaType() != TypeSpec::COMPOSITE) {
      logError(ErrCode::BAD_TYPE_DECL, "redeclaration of namespace `%s`", *symbol);
   }
   PUSH_SCOPE;

   //definition
   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "namespace definition");
   while (!tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::CLOSE)) {
      if (currTok.type() != TokenType::KEYWORD) {
         logError(ErrCode::BAD_STMT, "invalid statement in namespace definition");
      }
      switch (currTok.keywordID()) {
         #define KW_CASE(kw, parsingFunc) case KeywordID::kw: getNextToken(); { auto tmp = parsingFunc(); spec->composite().subtypes.insert(tree[tree[tmp].name()].symbol()); symbol->insert(tree[tree[tmp].name()].symbol()); } break
         KW_CASE(CLASS, parseClass);
         KW_CASE(STRUCT, parseStruct);
         KW_CASE(INTERFACE, parseInterface);
         KW_CASE(UNION, parseUnion);
         KW_CASE(ENUM, parseEnum);
         KW_CASE(MASK, parseMask);
         KW_CASE(NAMESPACE, parseNamespace);
         #undef KW_CASE
         case KeywordID::FUNC: getNextToken(); {auto tmp = parseFunction(); spec->composite().staticFuncs.insert(tree[tmp].symbol()); symbol->insert(tree[tmp].symbol());} break;
         case KeywordID::LET : getNextToken(); {auto tmp = parseDecl(); spec->composite().staticMembs.push_back(tree[tree[tmp].name()].symbol()); symbol->insert(tree[tree[tmp].name()].symbol());} break;
         default: logError(ErrCode::BAD_STMT, "invalid statement in namespace definition");
      }
   }

   //EOS
   consumeEOS("object type declaration without EOS");

   //return
   POP_SCOPE;
   return tree.make<TypeDecl>(name, name);
}

#endif