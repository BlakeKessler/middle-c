#ifndef PARSE_OBJ_TYPES_CPP
#define PARSE_OBJ_TYPES_CPP

#include "Parser.hpp"

//helper function used to implement parsing of classes and structs
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
      return make<TypeDecl>(name);
   }
   
   if (spec->metaType() != TypeSpec::COMPOSITE) {
      logError(ErrCode::BAD_TYPE_DECL, "redeclaration of %s `%s`", metatypeName, astTSB{tree, symbol, 0});
   }
   PUSH_SCOPE;

   //implemented traits
   if (tryConsumeOperator(OpID::LABEL_DELIM)) {
      do {
         index<Identifier> parentType = parseTypename(SymbolType::TRAIT, false);
         if (!spec->composite().impls.insert(tree[parentType].symbol())) {
            logError(ErrCode::BAD_TYPE_DECL, "%s `%s` already implements trait `%s`", metatypeName, astTSB{tree, tree[parentType].symbol(), 0});
         }
      } while (tryConsumeOperator(OpID::COMMA));
   }

   //definition
   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "class/struct definition");
   QualMask scope = QualMask::PRIVATE;
   index<Expr> fieldAttrs;
   while (!tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::CLOSE)) {
      //access modifiers
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
      isStatic = isStatic || tryConsumeKeyword(KeywordID::STATIC);
      if (currTok.type() != TokenType::KEYWORD) {
         logError(ErrCode::BAD_STMT, "invalid statement in %s definition", metatypeName);
      }
      switch (currTok.keywordID()) {
         #define KW_CASE(kw, parsingFunc) case KeywordID::kw: getNextToken(); if (isStatic || +quals) { logError(ErrCode::BAD_KEYWORD, "cannot qualify a " #kw " as%s% s", isStatic ? FMT(" static") : FMT(""), quals); } { auto tmp = parsingFunc(fieldAttrs); tree[(index<Identifier>)tmp].addQuals(scope); spec->composite().subtypes.insert(tree[tree[tmp].name()].symbol()); symbol->insert(tree[tree[tmp].name()].symbol()); } break
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
   return make<TypeDecl>(name, name);
}

//parse a trait declaration/definition
clef::index<clef::TypeDecl> clef::Parser::parseTrait(index<Expr> attrs) {
   index<Identifier> name = parseIdentifier(SymbolType::TRAIT, nullptr, true);
   SymbolNode* symbol = tree[name].symbol(); debug_assert(symbol);
   TypeSpec* spec = tree.registerType(symbol, TypeSpec::COMPOSITE, SymbolType::TRAIT);
   if (spec->metaType() != TypeSpec::COMPOSITE) {
      logError(ErrCode::BAD_TYPE_DECL, "redeclaration of trait `%s`", astTSB{tree, symbol, 0});
   }
   if (attrs) { TODO; }

   if (tryConsumeEOS()) { //forward declaration
      return make<TypeDecl>(name);
   }
   
   PUSH_SCOPE;
   
   //inheritance
   if (tryConsumeOperator(OpID::LABEL_DELIM)) {
      do {
         index<Identifier> parentType = parseTypename(SymbolType::TRAIT, false);
         if (!spec->composite().impls.insert(tree[parentType].symbol())) {
            logError(ErrCode::BAD_TYPE_DECL, "trait `%s` already extends trait `%s`", astTSB{tree, tree[parentType].symbol(), 0});
         }
      } while (tryConsumeOperator(OpID::COMMA));
   }

   //definition
   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad TRAIT definition");
   QualMask scope = QualMask::PUBLIC;
   index<Expr> fieldAttrs;
   while (!tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::CLOSE)) {
      //acess control
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
      //attributes
      fieldAttrs = tryParseAttrs();
      //qualifiers
      QualMask quals = parseQuals();
      bool isStatic = tryConsumeKeyword(KeywordID::STATIC);
      quals |= parseQuals();
      isStatic |= tryConsumeKeyword(KeywordID::STATIC);
      //function declaration
      consumeKeyword(KeywordID::FUNC, "traits can only contain functions and methods");
      index<Identifier> func = tree[parseFunction(fieldAttrs)].name(); //function definition (optional)
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
   return make<TypeDecl>(name, name);
}

//parse a union declaration/definition
clef::index<clef::TypeDecl> clef::Parser::parseUnion(index<Expr> attrs) {
   index<Identifier> name = tryParseIdentifier(SymbolType::UNION, nullptr, true);
   SymbolNode* symbol = tree[name].symbol(); debug_assert(symbol);
   TypeSpec* spec = tree.registerType(symbol, TypeSpec::COMPOSITE, SymbolType::UNION);
   if (spec->metaType() != TypeSpec::COMPOSITE) {
      logError(ErrCode::BAD_TYPE_DECL, "redeclaration of union `%s`", astTSB{tree, symbol, 0});
   }
   if (attrs) { TODO; }

   if (tryConsumeEOS()) { //forward declaration
      if (!name) {
         logError(ErrCode::BAD_DECL, "cannot forward-declare an anonymous union");
      }
      return make<TypeDecl>(name);
   }

   PUSH_SCOPE;

   //definition
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
   return make<TypeDecl>(name, name);
}

//helper function used to implement parsing of enums and masks
clef::index<clef::TypeDecl> clef::Parser::__parseEnumlikeImpl(index<Expr> attrs, SymbolType symbolType, const mcsl::str_slice metatypeName) {
   index<Identifier> name = tryParseIdentifier(symbolType, nullptr, true);
   SymbolNode* symbol = tree[name].symbol(); debug_assert(symbol);
   TypeSpec* spec = tree.registerType(symbol, TypeSpec::COMPOSITE, symbolType);
   if (spec->metaType() != TypeSpec::COMPOSITE) {
      logError(ErrCode::BAD_TYPE_DECL, "redeclaration of %s `%s`", metatypeName, astTSB{tree, symbol, 0});
   }
   if (attrs) { TODO; }

   if (tryConsumeEOS()) { //forward declaration
      if (!name) {
         logError(ErrCode::BAD_DECL, "cannot forward-declare an anonymous enum/mask");
      }
      return make<TypeDecl>(name);
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
   return make<TypeDecl>(name, name);
}

//parse an enumunion declaration/definition
clef::index<clef::TypeDecl> clef::Parser::parseEnumUnion(index<Expr> attrs) {
   TODO;
   (void)attrs;
}

//parse a namespace declaration/definition
clef::index<clef::TypeDecl> clef::Parser::parseNamespace(index<Expr> attrs) {
   index<Identifier> name = tryParseIdentifier(SymbolType::NAMESPACE, nullptr, true);
   SymbolNode* symbol = tree[name].symbol(); debug_assert(symbol);
   TypeSpec* spec = tree.registerType(symbol, TypeSpec::COMPOSITE, SymbolType::NAMESPACE);
   if (spec->metaType() != TypeSpec::COMPOSITE) {
      logError(ErrCode::BAD_TYPE_DECL, "redeclaration of namespace `%s`", astTSB{tree, symbol, 0});
   }
   if (attrs) { TODO; }

   if (tryConsumeEOS()) { //forward declaration
      if (!name) {
         logError(ErrCode::BAD_DECL, "cannot forward-declare an anonymous namespace");
      }
      return make<TypeDecl>(name);
   }
   
   PUSH_SCOPE;

   //definition
   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "namespace definition");
   QualMask scope = QualMask::PUBLIC;
   index<Expr> fieldAttrs;
   while (!tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::CLOSE)) {
      //access modifiers
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

      //member attributes
      fieldAttrs = tryParseAttrs();
      if (currTok.type() != TokenType::KEYWORD) {
         logError(ErrCode::BAD_STMT, "invalid statement in namespace definition");
      }
      //parse member
      switch (currTok.keywordID()) {
         #define KW_CASE(kw, parsingFunc) case KeywordID::kw: getNextToken(); { auto tmp = parsingFunc(fieldAttrs); tree[tree[tmp].name()].addQuals(scope); spec->composite().subtypes.insert(tree[tree[tmp].name()].symbol()); symbol->insert(tree[tree[tmp].name()].symbol()); } break
         KW_CASE(CLASS, parseClass);
         KW_CASE(STRUCT, parseStruct);
         KW_CASE(TRAIT, parseTrait);
         KW_CASE(UNION, parseUnion);
         KW_CASE(ENUM, parseEnum);
         KW_CASE(MASK, parseMask);
         KW_CASE(NAMESPACE, parseNamespace);
         #undef KW_CASE
         case KeywordID::FUNC: getNextToken(); {auto tmp = parseFunction(fieldAttrs); spec->composite().staticFuncs.emplace(tree[tree[tmp].name()].symbol(), scope); symbol->insert(tree[tree[tmp].name()].symbol());} break;
         case KeywordID::LET : getNextToken(); {auto tmp = parseDecl(fieldAttrs); spec->composite().staticMembs.emplace_back(tree[tree[tmp].name()].symbol(), scope); symbol->insert(tree[tree[tmp].name()].symbol());} break;
         default: logError(ErrCode::BAD_STMT, "invalid statement in namespace definition");
      }
   }

   //EOS
   consumeEOS("object type declaration without EOS");

   //return
   POP_SCOPE;
   return make<TypeDecl>(name, name);
}

#endif