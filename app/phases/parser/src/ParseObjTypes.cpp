#ifndef PARSE_OBJ_TYPES_CPP
#define PARSE_OBJ_TYPES_CPP

#include "Parser.hpp"

#define ACCESS_MOD(name) \
   if (tryConsumeKeyword(KeywordID::name)) {                         \
      scope = QualMask::name;                                        \
      consumeOperator(OpID::LABEL_DELIM, "invalid " #name " label"); \
      continue;                                                      \
   } (void)0
#define ACCESS_MODS \
   ACCESS_MOD(PUBLIC);  \
   ACCESS_MOD(PRIVATE); \
   ACCESS_MOD(PROTECTED)

//helper function used to implement parsing of classes and structs
clef::index<clef::TypeDecl> clef::Parser::__parseObjTypeImpl(index<Expr> attrs, clef::SymbolType symbolType, const mcsl::str_slice metatypeName) {
   index<Identifier> name = tryParseSymbol(symbolType, nullptr, true);
   SymbolNode* symbol = tree[name].symbol(); debug_assert(symbol);
   TypeSpec* spec = tree.registerType(symbol, TypeSpec::COMPOSITE, symbolType);
   debug_assert(spec->canonName());
   if (spec->metaType() != TypeSpec::COMPOSITE) {
      logError(ErrCode::BAD_TYPE_DECL, "redeclaration of %s `%s`", metatypeName, astTSB{tree, symbol, 0});
   }

   if (attrs) { TODO; }
   
   if (tryConsumeEOS()) { //forward declaration
      if (!name) {
         logError(ErrCode::BAD_DECL, "cannot forward-declare an anonymous class/struct");
      }
      return make<TypeDecl>(name);
   }

   auto& def = spec->composite();
   
   PUSH_SCOPE;

   //implemented traits
   if (tryConsumeOperator(OpID::LABEL_DELIM)) {
      do {
         index<Identifier> parentType = parseTypename(SymbolType::TRAIT, false);
         if (!def.impls.insert(tree[parentType].symbol())) {
            logError(ErrCode::BAD_TYPE_DECL, "%s `%s` already implements trait `%s`", metatypeName, astTSB{tree, tree[parentType].symbol(), 0});
         }
      } while (tryConsumeOperator(OpID::COMMA));
   }

   //definition
   consumeBlockDelim(BlockType::LIST, BlockDelimRole::OPEN, "class/struct definition");
   QualMask scope = QualMask::PRIVATE;
   index<Expr> fieldAttrs;
   while (!tryConsumeBlockDelim(BlockType::LIST, BlockDelimRole::CLOSE)) {
      //access modifiers
      ACCESS_MODS;

      fieldAttrs = tryParseAttrs();
      QualMask quals = parseQuals();
      bool isStatic = tryConsumeKeyword(KeywordID::STATIC);
      quals |= parseQuals();
      isStatic = isStatic || tryConsumeKeyword(KeywordID::STATIC);
      if (currTok.type() != TokenType::KEYWORD) {
         logError(ErrCode::BAD_STMT, "invalid statement in %s definition", metatypeName);
      }
      switch (currTok.keywordID()) {
         #define KW_CASE(kw, parsingFunc) \
            case KeywordID::kw: {                               \
               getNextToken();                                  \
               if (isStatic || +quals) {                        \
                  logError(ErrCode::BAD_KEYWORD,                \
                     "cannot qualify a " #kw " as%s% s",        \
                     FMT(isStatic ? " static" : ""), quals);    \
               }                                                \
               auto tmp = parsingFunc(fieldAttrs);              \
               SymbolNode* s = tree[tree[tmp].name()].symbol(); \
               tree[(index<Identifier>)tmp].addQuals(scope);    \
               def.subtypes.insert(s);                          \
               s->insert(s);                                    \
            } break
         KW_CASE(CLASS, parseClass);
         KW_CASE(STRUCT, parseStruct);
         KW_CASE(TRAIT, parseTrait);
         KW_CASE(UNION, parseUnion);
         KW_CASE(ENUM, parseEnum);
         KW_CASE(MASK, parseMask);
         KW_CASE(NAMESPACE, parseNamespace);
         #undef KW_CASE
         case KeywordID::FUNC: getNextToken(); {
            index<FuncDef> f = parseFunction(fieldAttrs, !isStatic);
            SymbolNode* funcSymbol = tree[tree[f].name()].symbol();
            tree[tree[f].name()].addQuals(scope);
            (isStatic ? def.staticFuncs : def.methods).emplace(funcSymbol, scope | quals);
            symbol->insert(funcSymbol);
         } break;
         case KeywordID::LET : getNextToken(); {
            auto decl = parseDecl(fieldAttrs);
            SymbolNode* declSymbol = tree[tree[decl].name()].symbol();
            tree[tree[decl].name()].addQuals(scope);
            (isStatic ? def.staticMembs : def.dataMembs).emplace_back(declSymbol, scope | quals);
            symbol->insert(declSymbol);
         } break;
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
   index<Identifier> name = parseSymbol(SymbolType::TRAIT, nullptr, true);
   SymbolNode* symbol = tree[name].symbol(); debug_assert(symbol);
   TypeSpec* spec = tree.registerType(symbol, TypeSpec::COMPOSITE, SymbolType::TRAIT);
   if (spec->metaType() != TypeSpec::COMPOSITE) {
      logError(ErrCode::BAD_TYPE_DECL, "redeclaration of trait `%s`", astTSB{tree, symbol, 0});
   }
   if (attrs) { TODO; }

   if (tryConsumeEOS()) { //forward declaration
      return make<TypeDecl>(name);
   }

   auto& def = spec->composite();
   
   PUSH_SCOPE;
   
   //inheritance
   if (tryConsumeOperator(OpID::LABEL_DELIM)) {
      do {
         index<Identifier> parentType = parseTypename(SymbolType::TRAIT, false);
         if (!def.impls.insert(tree[parentType].symbol())) {
            logError(ErrCode::BAD_TYPE_DECL, "trait `%s` already extends trait `%s`", astTSB{tree, tree[parentType].symbol(), 0});
         }
      } while (tryConsumeOperator(OpID::COMMA));
   }

   //definition
   consumeBlockDelim(BlockType::LIST, BlockDelimRole::OPEN, "bad TRAIT definition");
   QualMask scope = QualMask::PUBLIC;
   index<Expr> fieldAttrs;
   while (!tryConsumeBlockDelim(BlockType::LIST, BlockDelimRole::CLOSE)) {
      //acess modifiers
      ACCESS_MODS;

      //attributes
      fieldAttrs = tryParseAttrs();
      //qualifiers
      QualMask quals = parseQuals();
      bool isStatic = tryConsumeKeyword(KeywordID::STATIC);
      quals |= parseQuals();
      isStatic |= tryConsumeKeyword(KeywordID::STATIC);
      //function declaration
      consumeKeyword(KeywordID::FUNC, "traits can only contain functions and methods");
      index<Identifier> func = tree[parseFunction(fieldAttrs, !isStatic)].name(); //function definition (optional)
      SymbolNode* funcSymbol = tree[func].symbol();
      tree[func].addQuals(scope);
      (isStatic ? def.staticFuncs : def.methods).emplace(funcSymbol, scope | quals);
      symbol->insert(funcSymbol);
   }

   //EOS
   consumeEOS("trait without EOS");

   //return
   POP_SCOPE;
   return make<TypeDecl>(name, name);
}

//parse a union declaration/definition
clef::index<clef::TypeDecl> clef::Parser::parseUnion(index<Expr> attrs) {
   index<Identifier> name = tryParseSymbol(SymbolType::UNION, nullptr, true);
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

   auto& def = spec->composite();

   PUSH_SCOPE;

   //definition
   consumeBlockDelim(BlockType::LIST, BlockDelimRole::OPEN, "bad union definition");
   
   while (!tryConsumeBlockDelim(BlockType::LIST, BlockDelimRole::CLOSE)) {
      //parse member
      index<Decl> member = parseParam(tryParseAttrs());
      consumeEOS("bad union member");
      
      //push to members list
      SymbolNode* memberSymbol = tree[tree[member].name()].symbol();
      def.dataMembs.push_back(memberSymbol);
      symbol->insert(memberSymbol);
   }

   //EOS
   consumeEOS("union without EOS");

   //return
   POP_SCOPE;
   return make<TypeDecl>(name, name);
}

//helper function used to implement parsing of enums and masks
clef::index<clef::TypeDecl> clef::Parser::__parseEnumlikeImpl(index<Expr> attrs, SymbolType symbolType, const mcsl::str_slice metatypeName) {
   index<Identifier> name = tryParseSymbol(symbolType, nullptr, true);
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

   auto& def = spec->composite();

   PUSH_SCOPE;

   index<Identifier> baseType;
   if (tryConsumeOperator(OpID::LABEL_DELIM)) {
      baseType = parseTypename(SymbolType::FUND_TYPE, false);
   } else { baseType = 0; }


   consumeBlockDelim(BlockType::LIST, BlockDelimRole::OPEN, "bad enum/mask definition");
   if (!tryConsumeBlockDelim(BlockType::LIST, BlockDelimRole::CLOSE)) {
      do {
         index<Identifier> enumerator = parseSymbol(SymbolType::VAR, symbol, true);
         index<Expr> val;
         if (tryConsumeOperator(OpID::ASSIGN)) {
            val = parseExprNoPrimaryComma();
         } else { val = 0; }

         SymbolNode* enumeratorSymbol = tree[enumerator].symbol();
         def.staticMembs.push_back(enumeratorSymbol);
         symbol->insert(enumeratorSymbol);
      } while (tryConsumeOperator(OpID::COMMA));
      consumeBlockDelim(BlockType::LIST, BlockDelimRole::CLOSE, "bad ENUM enumerator");
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
   index<Identifier> name = tryParseSymbol(SymbolType::NAMESPACE, nullptr, true);
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

   auto& def = spec->composite();
   
   PUSH_SCOPE;

   //definition
   consumeBlockDelim(BlockType::LIST, BlockDelimRole::OPEN, "namespace definition");
   QualMask scope = QualMask::PUBLIC;
   index<Expr> fieldAttrs;
   while (!tryConsumeBlockDelim(BlockType::LIST, BlockDelimRole::CLOSE)) {
      //access modifiers
      ACCESS_MODS;

      //member attributes
      fieldAttrs = tryParseAttrs();
      if (currTok.type() != TokenType::KEYWORD) {
         logError(ErrCode::BAD_STMT, "invalid statement in namespace definition");
      }
      //parse member
      switch (currTok.keywordID()) {
         #define KW_CASE(kw, parsingFunc) \
            case KeywordID::kw: {                               \
               getNextToken();                                  \
               auto tmp = parsingFunc(fieldAttrs);              \
               SymbolNode* s = tree[tree[tmp].name()].symbol(); \
               tree[tree[tmp].name()].addQuals(scope);          \
               def.subtypes.insert(s);                          \
               s->insert(s);                                    \
            } break
         KW_CASE(CLASS, parseClass);
         KW_CASE(STRUCT, parseStruct);
         KW_CASE(TRAIT, parseTrait);
         KW_CASE(UNION, parseUnion);
         KW_CASE(ENUM, parseEnum);
         KW_CASE(MASK, parseMask);
         KW_CASE(NAMESPACE, parseNamespace);
         #undef KW_CASE
         case KeywordID::FUNC: getNextToken(); {
            index<FuncDef> f = parseFunction(fieldAttrs, false);
            SymbolNode* funcSymbol = tree[tree[f].name()].symbol();
            tree[tree[f].name()].addQuals(scope);
            def.staticFuncs.emplace(funcSymbol, scope);
            symbol->insert(funcSymbol);
         } break;
         case KeywordID::LET : getNextToken(); {
            index<Decl> decl = parseDecl(fieldAttrs);
            SymbolNode* declSymbol = tree[tree[decl].name()].symbol();
            tree[tree[decl].name()].addQuals(scope);
            def.staticMembs.emplace_back(declSymbol, scope);
            symbol->insert(declSymbol);
         } break;
         default: logError(ErrCode::BAD_STMT, "invalid statement in namespace definition");
      }
   }

   //EOS
   consumeEOS("object type declaration without EOS");

   //return
   POP_SCOPE;
   return make<TypeDecl>(name, name);
}

#undef ACCESS_MODS
#undef ACCESS_MOD

#endif