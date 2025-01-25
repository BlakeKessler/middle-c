#ifndef PARSE_OBJ_TYPES_CPP
#define PARSE_OBJ_TYPES_CPP

#include "Parser.hpp"

//!NOTE: clef::Parser::parseStruct temporarily relies on this function
clef::Class* clef::Parser::parseClass() {
   Identifier* name = parseIdentifier();
   
   if (tryConsumeEOS()) { //forward declaration
      ((astNode*)name)->upCast(NodeType::CLASS);
      return new (name) Class{(Type*)name};
   }

   //inheritance (including implemented interfaces)
   ObjTypeSpec* spec = new (tree.allocObjTypeSpec()) ObjTypeSpec{};
   if (tryConsumeOperator(OpID::LABEL_DELIM)) {
      do {
         Type* parentType = parseTypename();
         spec->inheritedTypes().push_back(parentType);
      } while (tryConsumeOperator(OpID::COMMA));
   }

   //definition
   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad CLASS definition");
   while (!tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::CLOSE)) {
      if (tokIt->type() == TokenType::KEYWORD) {
         switch (tokIt->keywordID()) {
            case KeywordID::CLASS    : ++tokIt; spec->memberTypes().push_back(parseClass()); break;
            case KeywordID::STRUCT   : ++tokIt; spec->memberTypes().push_back(parseStruct()); break;
            case KeywordID::INTERFACE: ++tokIt; spec->memberTypes().push_back(parseInterface()); break;
            case KeywordID::UNION    : ++tokIt; spec->memberTypes().push_back(parseUnion()); break;
            case KeywordID::ENUM     : ++tokIt; spec->memberTypes().push_back(parseEnum()); break;
            case KeywordID::MASK     : ++tokIt; spec->memberTypes().push_back(parseMask()); break;
            case KeywordID::NAMESPACE: ++tokIt; spec->memberTypes().push_back(parseNamespace()); break;
            case KeywordID::FUNC     : ++tokIt; spec->methods().push_back(parseFunction()); break; //!NOTE: does not account for static functions
            default: goto MEMB_VAR_DECL;
         }
         continue;
      }
      MEMB_VAR_DECL:
      spec->members().push_back(parseVariable()); //!NOTE: does not yet account for static functions
   }

   //EOS
   consumeEOS("CLASS without EOS");

   //return
   ((astNode*)name)->upCast(NodeType::CLASS);
   return new (name) Class{spec, (Type*)name};
}

//!NOTE: temporarily relies on clef::Parser::parseClass
clef::Struct* clef::Parser::parseStruct() {
   Class* classptr = parseClass();
   ((astNode*)classptr)->anyCast(NodeType::STRUCT);
   return (Struct*)classptr;
}

clef::Interface* clef::Parser::parseInterface() {
   Identifier* name = parseIdentifier();
   
   if (tryConsumeEOS()) { //forward declaration
      ((astNode*)name)->upCast(NodeType::INTERFACE);
      return new (name) Interface{(Type*)name};
   }
   
   //inheritance
   InterfaceSpec* spec = new (tree.allocInterfaceSpec()) InterfaceSpec{};
   if (tryConsumeOperator(OpID::LABEL_DELIM)) {
      do {
         Interface* parentType = (Interface*)parseIdentifier();
            ((astNode*)parentType)->upCast(NodeType::INTERFACE);
         spec->inheritedInterfaces().push_back(parentType);
      } while (tryConsumeOperator(OpID::COMMA));
   }

   //definition
   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad INTERFACE definition");
   while (!tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::CLOSE)) {
      TypeQualMask quals = parseQuals();
      consumeKeyword(KeywordID::FUNC, "INTERFACE can only contain functions");
      Function* func = parseFunction();
      if (+(quals & TypeQualMask::STATIC)) {
         spec->staticFuncs().push_back(func);
      } else {
         spec->methods().push_back(func);
      }
   }

   //EOS
   consumeEOS("INTERFACE without EOS");

   //return
   ((astNode*)name)->upCast(NodeType::INTERFACE);
   return new (name) Interface{spec, (Type*)name};
}

clef::Union* clef::Parser::parseUnion() {
   Identifier* name = tryParseIdentifier();

   if (tryConsumeEOS()) { //forward declaration
      if (!name) {
         logError(ErrCode::BAD_DECL, "cannot forward-declare an anonymous UNION");
      }
      ((astNode*)name)->upCast(NodeType::UNION);
      return new (name) Union{name};
   }

   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad UNION definition");

   ParameterList* members = new (tree.allocNode(NodeType::PARAM_LIST)) ParameterList{tree.allocBuf<Variable*>()};
   while (!tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::CLOSE)) {
      //parse member
      Type* memberType = parseTypename();
      Identifier* memberName = parseIdentifier();
      consumeEOS("bad UNION member");
      
      //push to members list
      Variable* member = (Variable*)memberName;
      ((astNode*)member)->upCast(NodeType::VAR);
      member->type() = memberType;
      members->push_back(member);
   }

   //EOS
   consumeEOS("UNION without EOS");

   //return
   if (name) { ((astNode*)name)->upCast(NodeType::UNION); }
   return new (name) Union{name, members};
}

//!NOTE: clef::Parser::parseMask relies on this function - be careful changing observable behavior
clef::Enum* clef::Parser::parseEnum() {
   Identifier* name = tryParseIdentifier();
   Type* baseType;
   if (tryConsumeOperator(OpID::LABEL_DELIM)) {
      baseType = parseTypename();
   } else { baseType = nullptr; }

   if (tryConsumeEOS()) { //forward declaration
      if (!name) {
         logError(ErrCode::BAD_DECL, "cannot forward-declare an anonymous ENUM");
      }
      ((astNode*)name)->anyCast(NodeType::ENUM);
      return new (name) Enum{baseType};
   }

   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad ENUM definition");
   ParamList* enumerators = new (tree.allocNode(NodeType::PARAM_LIST)) ParamList{tree.allocBuf<Variable*>()};
   if (!tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::CLOSE)) {
      do {
         Identifier* enumerator = parseIdentifier();
         Expr* val;
         if (tryConsumeOperator(OpID::ASSIGN)) {
            val = parseExprNoPrimaryComma();
         } else { val = nullptr; }

         ((astNode*)enumerator)->upCast(NodeType::VAR);
         enumerators->push_back(new (enumerator) Variable{baseType, enumerator, val});

      } while (tryConsumeOperator(OpID::COMMA));
      consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::CLOSE, "bad ENUM enumerator");
   }
   
   //EOS
   consumeEOS("ENUM without EOS");

   //return
   ((astNode*)name)->upCast(NodeType::ENUM);
   return new (name) Enum{(Type*)name, baseType, enumerators};
}

//!NOTE: ASSUMES THAT THE SYNTAX AND MEMORY LAYOUT OF THE AST NODES FOR MASKS AND ENUMS ARE IDENTICAL
clef::Mask* clef::Parser::parseMask() {
   astNode* mask = (astNode*)parseEnum();
   mask->anyCast(NodeType::MASK);
   return (Mask*)mask;
}

clef::Namespace* clef::Parser::parseNamespace() {
   Identifier* name = parseIdentifier();
   
   if (tryConsumeEOS()) { //forward declaration
      ((astNode*)name)->upCast(NodeType::NAMESPACE);
      return new (name) Namespace{(Type*)name};
   }

   //definition
   NamespaceSpec* spec = new (tree.allocNamespaceSpec()) NamespaceSpec{};
   consumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::OPEN, "bad CLASS definition");
   while (!tryConsumeBlockDelim(BlockType::INIT_LIST, BlockDelimRole::CLOSE)) {
      if (tokIt->type() == TokenType::KEYWORD) {
         switch (tokIt->keywordID()) {
            case KeywordID::CLASS    : ++tokIt; spec->types().push_back(parseClass()); break;
            case KeywordID::STRUCT   : ++tokIt; spec->types().push_back(parseStruct()); break;
            case KeywordID::INTERFACE: ++tokIt; spec->types().push_back(parseInterface()); break;
            case KeywordID::UNION    : ++tokIt; spec->types().push_back(parseUnion()); break;
            case KeywordID::ENUM     : ++tokIt; spec->types().push_back(parseEnum()); break;
            case KeywordID::MASK     : ++tokIt; spec->types().push_back(parseMask()); break;
            case KeywordID::NAMESPACE: ++tokIt; spec->types().push_back(parseNamespace()); break;
            case KeywordID::FUNC     : ++tokIt; spec->funcs().push_back(parseFunction()); break; //!NOTE: does not account for static functions
            default: goto MEMB_VAR_DECL;
         }
         continue;
      }
      MEMB_VAR_DECL:
      spec->vars().push_back(parseVariable()); //!NOTE: does not yet account for static functions
   }

   //EOS
   consumeEOS("NAMESPACE without EOS");

   //return
   ((astNode*)name)->upCast(NodeType::NAMESPACE);
   return new (name) Namespace{spec, (Type*)name};
}

#endif