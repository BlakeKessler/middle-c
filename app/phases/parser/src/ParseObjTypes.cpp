#ifndef PARSE_OBJ_TYPES_CPP
#define PARSE_OBJ_TYPES_CPP

#include "Parser.hpp"

clef::index<clef::TypeDecl> clef::Parser::__parseObjTypeImpl(index<Expr> attrs, clef::SymbolType symbolType, const mcsl::str_slice metatypeName) {
   index<Identifier> name = tryParseIdentifier(symbolType, nullptr, true);
   SymbolNode* symbol = tree[name].symbol(); debug_assert(symbol);
   TypeSpec* spec = tree.registerType(symbol, TypeSpec::COMPOSITE, symbolType);
   debug_assert(spec->canonName());
   if (attrs) { TODO; }
   
   if (tryConsumeEOS()) { //forward declaration
      if (!name) {
         logError(ErrCode::BAD_DECL, "cannot forward-declare an anonymous class/struct");
      }
      return tree.make<TypeDecl>(name);
   }
   
   if (spec->metaType() != TypeSpec::COMPOSITE) {
      logError(ErrCode::BAD_TYPE_DECL, "redeclaration of %s `%s`", metatypeName, *symbol);
   }
   PUSH_SCOPE;

   //implemented traits
   if (tryConsumeOperator(OpID::LABEL_DELIM)) {
      do {
         index<Identifier> parentType = parseTypename(SymbolType::TRAIT, false);
         if (!spec->composite().impls.insert(tree[parentType].symbol())) {
            logError(ErrCode::BAD_TYPE_DECL, "%s `%s` already implements trait `%s`", metatypeName, *tree[parentType].symbol());
         }
      } while (tryConsumeOperator(OpID::COMMA));
   }

   //definition
   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "class/struct definition");
   QualMask scope = QualMask::PRIVATE;
   index<Expr> fieldAttrs;
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

      fieldAttrs = tryParseAttrs();
      QualMask quals = parseQuals();
      bool isStatic = tryConsumeKeyword(KeywordID::STATIC);
      quals |= parseQuals();
      isStatic |= tryConsumeKeyword(KeywordID::STATIC);
      if (currTok.type() != TokenType::KEYWORD) {
         logError(ErrCode::BAD_STMT, "invalid statement in %s definition", metatypeName);
      }
      switch (currTok.keywordID()) {
         #define KW_CASE(kw, parsingFunc) case KeywordID::kw: getNextToken(); if (isStatic || +quals) { logError(ErrCode::BAD_KEYWORD, "cannot qualify a " #kw " as static"); } { auto tmp = parsingFunc(fieldAttrs); tree[(index<Identifier>)tmp].addQuals(scope); spec->composite().subtypes.insert(tree[tree[tmp].name()].symbol()); symbol->insert(tree[tree[tmp].name()].symbol()); } break
         KW_CASE(CLASS, parseClass);
         KW_CASE(STRUCT, parseStruct);
         KW_CASE(TRAIT, parseTrait);
         KW_CASE(UNION, parseUnion);
         KW_CASE(ENUM, parseEnum);
         KW_CASE(MASK, parseMask);
         KW_CASE(NAMESPACE, parseNamespace);
         #undef KW_CASE
         case KeywordID::FUNC: getNextToken(); {auto tmp = parseFunction(fieldAttrs); (isStatic ? spec->composite().staticFuncs : spec->composite().methods).emplace(tree[tree[tmp].name()].symbol(), scope | quals); symbol->insert(tree[tree[tmp].name()].symbol());} break;
         case KeywordID::LET : getNextToken(); {auto tmp = parseDecl(fieldAttrs); (isStatic ? spec->composite().staticMembs : spec->composite().dataMembs).emplace_back(tree[tree[tmp].name()].symbol(), scope | quals); symbol->insert(tree[tree[tmp].name()].symbol());} break;
         default: logError(ErrCode::BAD_STMT, "invalid statement in class/struct definition");
      }
   }

   //EOS
   consumeEOS("class/struct declaration without EOS");

   //return
   POP_SCOPE;
   return tree.make<TypeDecl>(name, name);
}

clef::index<clef::TypeDecl> clef::Parser::parseTrait(index<Expr> attrs) {
   index<Identifier> name = parseIdentifier(SymbolType::TRAIT, nullptr, true);
   SymbolNode* symbol = tree[name].symbol(); debug_assert(symbol);
   TypeSpec* spec = tree.registerType(symbol, TypeSpec::COMPOSITE, SymbolType::TRAIT);
   if (spec->metaType() != TypeSpec::COMPOSITE) {
      logError(ErrCode::BAD_TYPE_DECL, "redeclaration of trait `%s`", *symbol);
   }
   if (attrs) { TODO; }

   if (tryConsumeEOS()) { //forward declaration
      return tree.make<TypeDecl>(name);
   }
   
   PUSH_SCOPE;
   
   //inheritance
   if (tryConsumeOperator(OpID::LABEL_DELIM)) {
      do {
         index<Identifier> parentType = parseTypename(SymbolType::TRAIT, false);
         if (!spec->composite().impls.insert(tree[parentType].symbol())) {
            logError(ErrCode::BAD_TYPE_DECL, "trait `%s` already extends trait `%s`", *tree[parentType].symbol());
         }
      } while (tryConsumeOperator(OpID::COMMA));
   }

   //definition
   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad TRAIT definition");
   QualMask scope = QualMask::PUBLIC;
   index<Expr> fieldAttrs;
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
      fieldAttrs = tryParseAttrs();
      QualMask quals = parseQuals();
      bool isStatic = tryConsumeKeyword(KeywordID::STATIC);
      quals |= parseQuals();
      isStatic |= tryConsumeKeyword(KeywordID::STATIC);
      consumeKeyword(KeywordID::FUNC, "traits can only contain functions and methods");
      index<Identifier> func = tree[parseFunction(fieldAttrs)].name();
      tree[func].addQuals(scope);
      if (isStatic) {
         spec->composite().staticFuncs.emplace(tree[func].symbol(), scope | quals);
      } else {
         spec->composite().methods.emplace(tree[func].symbol(), scope | quals);
      }
      symbol->insert(tree[func].symbol());
   }

   //EOS
   consumeEOS("trait without EOS");

   //return
   POP_SCOPE;
   return tree.make<TypeDecl>(name, name);
}

clef::index<clef::TypeDecl> clef::Parser::parseUnion(index<Expr> attrs) {
   index<Identifier> name = tryParseIdentifier(SymbolType::UNION, nullptr, true);
   SymbolNode* symbol = tree[name].symbol(); debug_assert(symbol);
   TypeSpec* spec = tree.registerType(symbol, TypeSpec::COMPOSITE, SymbolType::UNION);
   if (spec->metaType() != TypeSpec::COMPOSITE) {
      logError(ErrCode::BAD_TYPE_DECL, "redeclaration of union `%s`", *symbol);
   }
   if (attrs) { TODO; }

   if (tryConsumeEOS()) { //forward declaration
      if (!name) {
         logError(ErrCode::BAD_DECL, "cannot forward-declare an anonymous union");
      }
      return tree.make<TypeDecl>(name);
   }

   PUSH_SCOPE;

   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad union definition");
   
   while (!tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::CLOSE)) {
      //parse member
      index<Decl> member = parseParam(tryParseAttrs());
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

clef::index<clef::TypeDecl> clef::Parser::__parseEnumlikeImpl(index<Expr> attrs, SymbolType symbolType, const mcsl::str_slice metatypeName) {
   index<Identifier> name = tryParseIdentifier(symbolType, nullptr, true);
   SymbolNode* symbol = tree[name].symbol(); debug_assert(symbol);
   TypeSpec* spec = tree.registerType(symbol, TypeSpec::COMPOSITE, symbolType);
   if (spec->metaType() != TypeSpec::COMPOSITE) {
      logError(ErrCode::BAD_TYPE_DECL, "redeclaration of %s `%s`", metatypeName, *symbol);
   }
   if (attrs) { TODO; }

   if (tryConsumeEOS()) { //forward declaration
      if (!name) {
         logError(ErrCode::BAD_DECL, "cannot forward-declare an anonymous enum/mask");
      }
      return tree.make<TypeDecl>(name);
   }

   PUSH_SCOPE;

   index<Identifier> baseType;
   if (tryConsumeOperator(OpID::LABEL_DELIM)) {
      baseType = parseTypename(SymbolType::FUND_TYPE, false);
   } else { baseType = 0; }


   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad enum/mask definition");
   if (!tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::CLOSE)) {
      do {
         index<Identifier> enumerator = parseIdentifier(SymbolType::VAR, symbol, true);
         index<Expr> val;
         if (tryConsumeOperator(OpID::ASSIGN)) {
            val = parseExprNoPrimaryComma();
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
clef::index<clef::TypeDecl> clef::Parser::parseEnumUnion(index<Expr> attrs) {
   logError(ErrCode::PARSER_NOT_IMPLEMENTED, "enumunions are not yet supported");
}

clef::index<clef::TypeDecl> clef::Parser::parseNamespace(index<Expr> attrs) {
   index<Identifier> name = tryParseIdentifier(SymbolType::NAMESPACE, nullptr, true);
   SymbolNode* symbol = tree[name].symbol(); debug_assert(symbol);
   TypeSpec* spec = tree.registerType(symbol, TypeSpec::COMPOSITE, SymbolType::NAMESPACE);
   if (spec->metaType() != TypeSpec::COMPOSITE) {
      logError(ErrCode::BAD_TYPE_DECL, "redeclaration of namespace `%s`", *symbol);
   }
   if (attrs) { TODO; }

   if (tryConsumeEOS()) { //forward declaration
      if (!name) {
         logError(ErrCode::BAD_DECL, "cannot forward-declare an anonymous namespace");
      }
      return tree.make<TypeDecl>(name);
   }
   
   PUSH_SCOPE;

   //definition
   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "namespace definition");
   index<Expr> fieldAttrs;
   while (!tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::CLOSE)) {
      fieldAttrs = tryParseAttrs();
      if (currTok.type() != TokenType::KEYWORD) {
         logError(ErrCode::BAD_STMT, "invalid statement in namespace definition");
      }
      switch (currTok.keywordID()) {
         #define KW_CASE(kw, parsingFunc) case KeywordID::kw: getNextToken(); { auto tmp = parsingFunc(fieldAttrs); spec->composite().subtypes.insert(tree[tree[tmp].name()].symbol()); symbol->insert(tree[tree[tmp].name()].symbol()); } break
         KW_CASE(CLASS, parseClass);
         KW_CASE(STRUCT, parseStruct);
         KW_CASE(TRAIT, parseTrait);
         KW_CASE(UNION, parseUnion);
         KW_CASE(ENUM, parseEnum);
         KW_CASE(MASK, parseMask);
         KW_CASE(NAMESPACE, parseNamespace);
         #undef KW_CASE
         case KeywordID::FUNC: getNextToken(); {auto tmp = parseFunction(fieldAttrs); spec->composite().staticFuncs.insert(tree[tree[tmp].name()].symbol()); symbol->insert(tree[tree[tmp].name()].symbol());} break;
         case KeywordID::LET : getNextToken(); {auto tmp = parseDecl(fieldAttrs); spec->composite().staticMembs.push_back(tree[tree[tmp].name()].symbol()); symbol->insert(tree[tree[tmp].name()].symbol());} break;
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