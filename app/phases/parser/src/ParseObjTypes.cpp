#ifndef PARSE_OBJ_TYPES_CPP
#define PARSE_OBJ_TYPES_CPP

#include "Parser.hpp"

//!HACK: clef::Parser::parseStruct temporarily relies on this function
clef::index<clef::TypeDecl> clef::Parser::parseClass() {
   index<Identifier> name = parseIdentifier(SymbolType::CLASS, nullptr);
   
   if (tryConsumeEOS()) { //forward declaration
      TODO;
      index<Class> classptr = tree.remake<Class>(name, tree[(index<Type>)name]);
      return tree.make<TypeDecl>(tree.getFundType(KeywordID::CLASS), classptr);
   }
   
   index<ObjTypeSpec> specIndex = tree.allocObjTypeSpec();
   ObjTypeSpec& spec = tree[specIndex];
   index<Class> classptr = tree.remake<Class>(name, specIndex, tree[(index<Type>)name]);

   //inheritance (including implemented interfaces)
   if (tryConsumeOperator(OpID::LABEL_DELIM)) {
      do {
         TODO;
         index<Type> parentType = parseTypename();
         spec.inheritedTypes().push_back(parentType);
      } while (tryConsumeOperator(OpID::COMMA));
   }

   //definition
   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad CLASS definition");
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
         logError(ErrCode::BAD_STMT, "invalid statement in CLASS definition");
      }
      switch (currTok.keywordID()) {
         #define KW_CASE(kw, parsingFunc) case KeywordID::kw: getNextToken(); if (isStatic) { logError(ErrCode::BAD_KEYWORD, "cannot qualify a " #kw " as static"); } { auto tmp = parsingFunc(); tree[(index<Identifier>)tmp].addQuals(scope); spec.memberTypes().push_back(tmp); } break
         KW_CASE(CLASS, parseClass);
         KW_CASE(STRUCT, parseStruct);
         KW_CASE(INTERFACE, parseInterface);
         KW_CASE(UNION, parseUnion);
         KW_CASE(ENUM, parseEnum);
         KW_CASE(MASK, parseMask);
         KW_CASE(NAMESPACE, parseNamespace);
         #undef KW_CASE
         case KeywordID::FUNC: getNextToken(); {auto tmp = parseFunction(); tree[(index<Identifier>)tmp].addQuals(scope); (isStatic ? spec.staticFuncs() : spec.methods()).push_back(tmp);} break;
         case KeywordID::LET : getNextToken(); {auto tmp = parseLetStmt(); tree[(index<Identifier>)(tree[tmp].name())].addQuals(scope); (isStatic ? spec.staticVars() : spec.members()).push_back(tmp);} break;
         default: logError(ErrCode::BAD_STMT, "invalid statement in CLASS definition");
      }
   }

   //EOS
   consumeEOS("CLASS without EOS");

   //return
   return tree.make<TypeDecl>(tree.getFundType(KeywordID::CLASS), classptr, specIndex);
}

//!HACK: temporarily assumes that the syntax and memory layout of the AST nodes for structs and classes are identical and depends on clef::Parser::parseClass()
clef::index<clef::TypeDecl> clef::Parser::parseStruct() {
   index<TypeDecl> decl = parseClass();
   tree[decl].objType() = tree.getFundType(KeywordID::STRUCT);
   tree[(index<astNode>)tree[decl].name()].anyCast(NodeType::STRUCT);
   return +decl;
}

clef::index<clef::TypeDecl> clef::Parser::parseInterface() {
   index<Identifier> name = parseIdentifier<true>();
   
   if (tryConsumeEOS()) { //forward declaration
      index<Interface> classptr = tree.remake<Interface>(name, tree[(index<Type>)name]);
      return tree.make<TypeDecl>(tree.getFundType(KeywordID::INTERFACE), classptr);
   }

   index<InterfaceSpec> specIndex = tree.allocInterfaceSpec();
   InterfaceSpec& spec = tree[specIndex];
   index<Interface> ifaceptr = tree.remake<Interface>(name, specIndex, tree[(index<Type>)name]);
   
   //inheritance
   if (tryConsumeOperator(OpID::LABEL_DELIM)) {
      do {
         index<Interface> parentType = [&](index<Type> iden){ return tree.remake<Interface>(iden, tree[iden]); }(parseTypename());
         spec.inheritedInterfaces().push_back(parentType);
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
      consumeKeyword(KeywordID::FUNC, "INTERFACE can only contain functions");
      index<Function> func = parseFunction();
      tree[func].addQuals(scope);
      if (isStatic) {
         spec.staticFuncs().push_back(func);
      } else {
         spec.methods().push_back(func);
      }
   }

   //EOS
   consumeEOS("INTERFACE without EOS");

   //return
   return tree.make<TypeDecl>(tree.getFundType(KeywordID::INTERFACE), ifaceptr, specIndex);
}

clef::index<clef::TypeDecl> clef::Parser::parseUnion() {
   index<Identifier> name = parseIdentifier<true>();

   if (tryConsumeEOS()) { //forward declaration
      if (!name) {
         logError(ErrCode::BAD_DECL, "cannot forward-declare an anonymous UNION");
      }
      return tree.make<TypeDecl>(tree.getFundType(KeywordID::UNION), name);
   }

   index<Union> unionptr = tree.remake<Union>(name, tree[(index<Type>)name]);
   Union& spec = tree[unionptr];

   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad UNION definition");

   spec.members() = tree.make<ParameterList>(&tree.allocBuf<index<Variable>>());
   while (!tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::CLOSE)) {
      //!TODO: maybe use parseLetStmt?
      //parse member
      index<Type> memberType = parseTypename();
      index<Identifier> memberName = parseIdentifier();
      consumeEOS("bad UNION member");
      
      //push to members list
      index<Variable> member = tree.remake<Variable>(memberName, memberType, tree[memberName]);
      tree[spec.members()].push_back(member);
   }

   //EOS
   consumeEOS("UNION without EOS");

   //return
   return tree.make<TypeDecl>(tree.getFundType(KeywordID::UNION), unionptr, spec.members());
}

//!HACK: clef::Parser::parseMask relies on this function - be careful changing observable behavior
clef::index<clef::TypeDecl> clef::Parser::parseEnum() {
   index<Identifier> name = parseIdentifier<true>();
   index<Type> baseType;
   if (tryConsumeOperator(OpID::LABEL_DELIM)) {
      baseType = parseTypename();
   } else { baseType = 0; }

   if (tryConsumeEOS()) { //forward declaration
      if (!name) {
         logError(ErrCode::BAD_DECL, "cannot forward-declare an anonymous ENUM");
      }
      return tree.make<TypeDecl>(tree.getFundType(KeywordID::ENUM), name);
   }


   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad ENUM definition");
   index<ParamList> enumerators = tree.make<ParamList>(&tree.allocBuf<index<Variable>>());
   index<Enum> enumptr = tree.remake<Enum>(name, tree[(index<Type>)name], baseType, enumerators);
   if (!tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::CLOSE)) {
      do {
         index<Identifier> enumerator = parseIdentifier();
         index<Expr> val;
         if (tryConsumeOperator(OpID::ASSIGN)) {
            val = parseExprNoPrimaryComma();
         } else { val = 0; }

         tree[enumerators].push_back(tree.remake<Variable>(enumerator, baseType, tree[enumerator], val));

      } while (tryConsumeOperator(OpID::COMMA));
      consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::CLOSE, "bad ENUM enumerator");
   }
   
   //EOS
   consumeEOS("ENUM without EOS");

   //return
   return tree.make<TypeDecl>(tree.getFundType(KeywordID::ENUM), enumptr, enumerators);
}

//!HACK: temporarily assumes that the syntax and memory layout of the AST nodes for masks and enums are identical and depends on clef::Parser::parseEnum()
clef::index<clef::TypeDecl> clef::Parser::parseMask() {
   index<TypeDecl> decl = parseEnum();
   tree[decl].objType() = tree.getFundType(KeywordID::MASK);
   tree[(index<astNode>)tree[decl].name()].anyCast(NodeType::MASK);
   return +decl;
}

//!TODO: implement enumunions
clef::index<clef::TypeDecl> clef::Parser::parseEnumUnion() {
   logError(ErrCode::PARSER_NOT_IMPLEMENTED, "enumunions are not yet supported");
}

clef::index<clef::TypeDecl> clef::Parser::parseNamespace() {
   index<Identifier> name = parseIdentifier<true>();
   
   if (tryConsumeEOS()) { //forward declaration
      if (!name) {
         logError(ErrCode::BAD_DECL, "cannot forward-declare an anonymous NAMESPACE");
      }
      return tree.make<TypeDecl>(tree.getFundType(KeywordID::NAMESPACE), name);
   }
   
   index<NamespaceSpec> specIndex = tree.allocNamespaceSpec();
   NamespaceSpec& spec = tree[specIndex];
   index<Namespace> namespaceptr = tree.remake<Namespace>(name, specIndex, tree[(index<Type>)name]);

   //definition
   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad CLASS definition");
   while (!tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::CLOSE)) {
      if (currTok.type() != TokenType::KEYWORD) {
         logError(ErrCode::BAD_STMT, "invalid statement in NAMESPACE definition");
      }
      switch (currTok.keywordID()) {
         case KeywordID::CLASS    : getNextToken(); {auto tmp = parseClass(); spec.types().push_back(tmp);} break;
         case KeywordID::STRUCT   : getNextToken(); {auto tmp = parseStruct(); spec.types().push_back(tmp);} break;
         case KeywordID::INTERFACE: getNextToken(); {auto tmp = parseInterface(); spec.types().push_back(tmp);} break;
         case KeywordID::UNION    : getNextToken(); {auto tmp = parseUnion(); spec.types().push_back(tmp);} break;
         case KeywordID::ENUM     : getNextToken(); {auto tmp = parseEnum(); spec.types().push_back(tmp);} break;
         case KeywordID::MASK     : getNextToken(); {auto tmp = parseMask(); spec.types().push_back(tmp);} break;
         case KeywordID::NAMESPACE: getNextToken(); {auto tmp = parseNamespace(); spec.types().push_back(tmp);} break;
         case KeywordID::FUNC     : getNextToken(); {auto tmp = parseFunction(); spec.funcs().push_back(tmp);} break;
         case KeywordID::LET      : getNextToken(); {auto tmp = parseLetStmt(); spec.vars().push_back(tmp);} break;
         default: logError(ErrCode::BAD_STMT, "invalid statement in NAMESPACE definition");
      }
   }

   //EOS
   consumeEOS("NAMESPACE without EOS");

   //return
   return tree.make<TypeDecl>(tree.getFundType(KeywordID::NAMESPACE), namespaceptr, specIndex);
}

#endif