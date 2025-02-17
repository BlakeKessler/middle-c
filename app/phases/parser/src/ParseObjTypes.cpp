#ifndef PARSE_OBJ_TYPES_CPP
#define PARSE_OBJ_TYPES_CPP

#include "Parser.hpp"

//!HACK: clef::Parser::parseStruct temporarily relies on this function
//!TODO: `let` syntax
clef::index<clef::Class> clef::Parser::parseClass() {
   index<Identifier> name = parseIdentifier();
   
   if (tryConsumeEOS()) { //forward declaration
      return tree.remake<Class>(name, tree.allocObjTypeSpec(), tree[(index<Type>)name]);
   }

   //inheritance (including implemented interfaces)
   index<ObjTypeSpec> spec = tree.allocObjTypeSpec();
   if (tryConsumeOperator(OpID::LABEL_DELIM)) {
      do {
         index<Type> parentType = parseTypename();
         tree[spec].inheritedTypes().push_back(parentType);
      } while (tryConsumeOperator(OpID::COMMA));
   }

   //definition
   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad CLASS definition");
   while (!tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::CLOSE)) {
      if (tokIt->type() != TokenType::KEYWORD) {
         logError(ErrCode::BAD_STMT, "invalid statement in CLASS definition");
      }
      switch (tokIt->keywordID()) {
         case KeywordID::CLASS    : ++tokIt; {auto tmp = parseClass(); tree[spec].memberTypes().push_back(tmp);} break;
         case KeywordID::STRUCT   : ++tokIt; {auto tmp = parseStruct(); tree[spec].memberTypes().push_back(tmp);} break;
         case KeywordID::INTERFACE: ++tokIt; {auto tmp = parseInterface(); tree[spec].memberTypes().push_back(tmp);} break;
         case KeywordID::UNION    : ++tokIt; {auto tmp = parseUnion(); tree[spec].memberTypes().push_back(tmp);} break;
         case KeywordID::ENUM     : ++tokIt; {auto tmp = parseEnum(); tree[spec].memberTypes().push_back(tmp);} break;
         case KeywordID::MASK     : ++tokIt; {auto tmp = parseMask(); tree[spec].memberTypes().push_back(tmp);} break;
         case KeywordID::NAMESPACE: ++tokIt; {auto tmp = parseNamespace(); tree[spec].memberTypes().push_back(tmp);} break;
         case KeywordID::FUNC     : ++tokIt; {auto tmp = parseFunction(); tree[spec].methods().push_back(tmp);} break; //!TODO: does not account for static functions
         case KeywordID::LET      : ++tokIt; {auto tmp = parseLetStmt().second; tree[spec].members().push_back(tmp); } //!TODO: does not account for static members
         default: logError(ErrCode::BAD_STMT, "invalid statement in CLASS definition");
      }
   }

   //EOS
   consumeEOS("CLASS without EOS");

   //return
   return tree.remake<Class>(name, spec, tree[(index<Type>)name]);
}

//!HACK: temporarily relies on clef::Parser::parseClass
clef::index<clef::Struct> clef::Parser::parseStruct() {
   index<astNode> classptr = +parseClass();
   tree[classptr].anyCast(NodeType::STRUCT);
   return +classptr;
}

clef::index<clef::Interface> clef::Parser::parseInterface() {
   index<Identifier> name = parseIdentifier();
   
   if (tryConsumeEOS()) { //forward declaration
      return tree.remake<Interface>(name, tree[(index<Type>)name]);
   }
   
   //inheritance
   index<InterfaceSpec> spec = tree.allocInterfaceSpec();
   if (tryConsumeOperator(OpID::LABEL_DELIM)) {
      do {
         index<Interface> parentType = (index<Interface>)parseIdentifier();
            tree[(index<astNode>)parentType].upCast(NodeType::INTERFACE);
         tree[spec].inheritedInterfaces().push_back(parentType);
      } while (tryConsumeOperator(OpID::COMMA));
   }

   //definition
   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad INTERFACE definition");
   while (!tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::CLOSE)) {
      // TypeQualMask quals = parseQuals();
      consumeKeyword(KeywordID::FUNC, "INTERFACE can only contain functions");
      index<Function> func = parseFunction();
      // if (+(quals & TypeQualMask::STATIC)) {
      //    tree[spec].staticFuncs().push_back(func);
      // } else {
         tree[spec].methods().push_back(func);
      // }
   }

   //EOS
   consumeEOS("INTERFACE without EOS");

   //return
   return tree.remake<Interface>(name, spec, tree[(index<Type>)name]);
}

clef::index<clef::Union> clef::Parser::parseUnion() {
   index<Identifier> name = tryParseIdentifier();

   if (tryConsumeEOS()) { //forward declaration
      if (!name) {
         logError(ErrCode::BAD_DECL, "cannot forward-declare an anonymous UNION");
      }
      return tree.remake<Union>(name, tree[(index<Type>)name]);
   }

   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad UNION definition");

   index<ParameterList> members = tree.make<ParameterList>(&tree.allocBuf<index<Variable>>());
   while (!tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::CLOSE)) {
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
   return tree.remake<Union>(name, members, tree[(index<Type>)name]);
}

//!HACK: clef::Parser::parseMask relies on this function - be careful changing observable behavior
clef::index<clef::Enum> clef::Parser::parseEnum() {
   index<Identifier> name = tryParseIdentifier();
   index<Type> baseType;
   if (tryConsumeOperator(OpID::LABEL_DELIM)) {
      baseType = parseTypename();
   } else { baseType = 0; }

   if (tryConsumeEOS()) { //forward declaration
      if (!name) {
         logError(ErrCode::BAD_DECL, "cannot forward-declare an anonymous ENUM");
      }
      return tree.remake<Enum>(name, tree[(index<Type>)name], baseType, tree.make<ParamList>(&tree.allocBuf<index<Variable>>()));
   }

   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad ENUM definition");
   index<ParamList> enumerators = tree.make<ParamList>(&tree.allocBuf<index<Variable>>());
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
   return tree.remake<Enum>(name, tree[(index<Type>)name], baseType, enumerators);
}

//!HACK: ASSUMES THAT THE SYNTAX AND MEMORY LAYOUT OF THE AST NODES FOR MASKS AND ENUMS ARE IDENTICAL
clef::index<clef::Mask> clef::Parser::parseMask() {
   index<astNode> mask = +parseEnum();
   tree[mask].anyCast(NodeType::MASK);
   return +mask;
}

//!TODO: `let` syntax
clef::index<clef::Namespace> clef::Parser::parseNamespace() {
   index<Identifier> name = parseIdentifier();
   
   if (tryConsumeEOS()) { //forward declaration
      return tree.remake<Namespace>(name, tree[(index<Type>)name]);
   }

   //definition
   index<NamespaceSpec> spec = tree.allocNamespaceSpec();
   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad CLASS definition");
   while (!tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::CLOSE)) {
      if (tokIt->type() != TokenType::KEYWORD) {
         logError(ErrCode::BAD_STMT, "invalid statement in NAMESPACE definition");
      }
      switch (tokIt->keywordID()) {
         case KeywordID::CLASS    : ++tokIt; {auto tmp = parseClass(); tree[spec].types().push_back(tmp);} break;
         case KeywordID::STRUCT   : ++tokIt; {auto tmp = parseStruct(); tree[spec].types().push_back(tmp);} break;
         case KeywordID::INTERFACE: ++tokIt; {auto tmp = parseInterface(); tree[spec].types().push_back(tmp);} break;
         case KeywordID::UNION    : ++tokIt; {auto tmp = parseUnion(); tree[spec].types().push_back(tmp);} break;
         case KeywordID::ENUM     : ++tokIt; {auto tmp = parseEnum(); tree[spec].types().push_back(tmp);} break;
         case KeywordID::MASK     : ++tokIt; {auto tmp = parseMask(); tree[spec].types().push_back(tmp);} break;
         case KeywordID::NAMESPACE: ++tokIt; {auto tmp = parseNamespace(); tree[spec].types().push_back(tmp);} break;
         case KeywordID::FUNC     : ++tokIt; {auto tmp = parseFunction(); tree[spec].funcs().push_back(tmp);} break; //!NOTE: does not account for static functions
         case KeywordID::LET      : ++tokIt; {auto tmp = parseLetStmt().second; tree[spec].vars().push_back(tmp); }
         default: logError(ErrCode::BAD_STMT, "invalid statement in NAMESPACE definition");
      }
   }

   //EOS
   consumeEOS("NAMESPACE without EOS");

   //return
   return tree.remake<Namespace>(name, spec, tree[(index<Type>)name]);
}

#endif