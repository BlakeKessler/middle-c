#ifndef PARSE_OBJ_TYPES_CPP
#define PARSE_OBJ_TYPES_CPP

#include "Parser.hpp"

//!HACK: clef::Parser::parseStruct temporarily relies on this function
clef::index<clef::TypeDecl> clef::Parser::parseClass() {
   index<Identifier> name = parseIdentifier<true>();
   
   if (tryConsumeEOS()) { //forward declaration
      index<Class> classptr = tree.remake<Class>(name, tree[(index<Type>)name]);
      return tree.make<TypeDecl>(tree.getFundType(KeywordID::CLASS), classptr);
   }
   
   index<ObjTypeSpec> specIndex = tree.allocObjTypeSpec();
   ObjTypeSpec& spec = tree[specIndex];
   index<Class> classptr = tree.remake<Class>(name, specIndex, tree[(index<Type>)name]);

   //inheritance (including implemented interfaces)
   if (tryConsumeOperator(OpID::LABEL_DELIM)) {
      do {
         index<Type> parentType = parseTypename();
         spec.inheritedTypes().push_back(parentType);
      } while (tryConsumeOperator(OpID::COMMA));
   }

   //definition
   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad CLASS definition");
   while (!tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::CLOSE)) {
      if (tokIt->type() != TokenType::KEYWORD) {
         logError(ErrCode::BAD_STMT, "invalid statement in CLASS definition");
      }
      switch (tokIt->keywordID()) {
         case KeywordID::CLASS    : ++tokIt; {auto tmp = parseClass(); spec.memberTypes().push_back(tmp);} break;
         case KeywordID::STRUCT   : ++tokIt; {auto tmp = parseStruct(); spec.memberTypes().push_back(tmp);} break;
         case KeywordID::INTERFACE: ++tokIt; {auto tmp = parseInterface(); spec.memberTypes().push_back(tmp);} break;
         case KeywordID::UNION    : ++tokIt; {auto tmp = parseUnion(); spec.memberTypes().push_back(tmp);} break;
         case KeywordID::ENUM     : ++tokIt; {auto tmp = parseEnum(); spec.memberTypes().push_back(tmp);} break;
         case KeywordID::MASK     : ++tokIt; {auto tmp = parseMask(); spec.memberTypes().push_back(tmp);} break;
         case KeywordID::NAMESPACE: ++tokIt; {auto tmp = parseNamespace(); spec.memberTypes().push_back(tmp);} break;
         case KeywordID::FUNC     : ++tokIt; {auto tmp = parseFunction(); spec.methods().push_back(tmp);} break; //!TODO: does not account for static functions
         case KeywordID::LET      : ++tokIt; {auto tmp = parseLetStmt(); spec.members().push_back(tmp);} break; //!TODO: does not account for static members
         default: logError(ErrCode::BAD_STMT, "invalid statement in CLASS definition");
      }
   }

   //EOS
   consumeEOS("CLASS without EOS");

   //return
   return tree.make<TypeDecl>(tree.getFundType(KeywordID::CLASS), classptr, specIndex);
}

//!HACK: temporarily relies on clef::Parser::parseClass
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
   while (!tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::CLOSE)) {
      // TypeQualMask quals = parseQuals();
      consumeKeyword(KeywordID::FUNC, "INTERFACE can only contain functions");
      index<Function> func = parseFunction();
      // if (+(quals & TypeQualMask::STATIC)) {
      //    spec.staticFuncs().push_back(func);
      // } else {
         spec.methods().push_back(func);
      // }
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

   index<ParameterList> members = tree.make<ParameterList>(&tree.allocBuf<index<Variable>>());
   while (!tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::CLOSE)) {
      //!TODO: maybe use parseLetStmt?
      //parse member
      index<Type> memberType = parseTypename();
      index<Identifier> memberName = parseIdentifier();
      consumeEOS("bad UNION member");
      
      //push to members list
      index<Variable> member = tree.remake<Variable>(memberName, memberType, tree[memberName]);
      tree[members].push_back(member);
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

//!HACK: ASSUMES THAT THE SYNTAX AND MEMORY LAYOUT OF THE AST NODES FOR MASKS AND ENUMS ARE IDENTICAL
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
      if (tokIt->type() != TokenType::KEYWORD) {
         logError(ErrCode::BAD_STMT, "invalid statement in NAMESPACE definition");
      }
      switch (tokIt->keywordID()) {
         case KeywordID::CLASS    : ++tokIt; {auto tmp = parseClass(); spec.types().push_back(tmp);} break;
         case KeywordID::STRUCT   : ++tokIt; {auto tmp = parseStruct(); spec.types().push_back(tmp);} break;
         case KeywordID::INTERFACE: ++tokIt; {auto tmp = parseInterface(); spec.types().push_back(tmp);} break;
         case KeywordID::UNION    : ++tokIt; {auto tmp = parseUnion(); spec.types().push_back(tmp);} break;
         case KeywordID::ENUM     : ++tokIt; {auto tmp = parseEnum(); spec.types().push_back(tmp);} break;
         case KeywordID::MASK     : ++tokIt; {auto tmp = parseMask(); spec.types().push_back(tmp);} break;
         case KeywordID::NAMESPACE: ++tokIt; {auto tmp = parseNamespace(); spec.types().push_back(tmp);} break;
         case KeywordID::FUNC     : ++tokIt; {auto tmp = parseFunction(); spec.funcs().push_back(tmp);} break; //!NOTE: does not account for static functions
         case KeywordID::LET      : ++tokIt; {auto tmp = parseLetStmt(); spec.vars().push_back(tmp);} break;
         default: logError(ErrCode::BAD_STMT, "invalid statement in NAMESPACE definition");
      }
   }

   //EOS
   consumeEOS("NAMESPACE without EOS");

   //return
   return tree.make<TypeDecl>(tree.getFundType(KeywordID::NAMESPACE), namespaceptr, specIndex);
}

#endif