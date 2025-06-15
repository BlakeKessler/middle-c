#ifndef SYNTAX_TREE_CPP
#define SYNTAX_TREE_CPP

#include "SyntaxTree.hpp"

#include "pretty-print.hpp"
#include "io.hpp"

#include "MAP_MACRO.h"

#define ID(kw, fund) case KeywordID::kw: id = FundTypeID::fund; break
//!TODO: support data models besides LP64
clef::index<clef::FundType> clef::SyntaxTree::getFundType(const KeywordID keyword) {
   FundTypeID id;
   switch (keyword) {
      ID(VOID, VOID);
      ID(AUTO, AUTO);


      ID(CHAR, CHAR);
      ID(CHAR_UTF_8, CHAR8);
      ID(CHAR_UTF_16, CHAR16);
      ID(CHAR_UTF_32, CHAR32);


      ID(BOOL, BOOL);
      ID(UBYTE, UINT8);
      ID(USHORT, UINT16);
      ID(UINT, UINT32);
      ID(ULONG, UINT64);
      ID(UINT_PTR, UINT64);
      ID(UWORD, UINT64);

      ID(SIGN_T, SIGN_T);
      ID(SBYTE, SINT8);
      ID(SSHORT, SINT16);
      ID(SINT, SINT32);
      ID(SLONG, SINT64);
      ID(SINT_PTR, SINT64);
      ID(SWORD, SINT64);


      ID(UINT_8, UINT8);
      ID(UINT_16, UINT16);
      ID(UINT_32, UINT32);
      ID(UINT_64, UINT64);
      ID(UINT_128, UINT128);
      ID(UINT_256, UINT256);

      ID(SINT_8, SINT8);
      ID(SINT_16, SINT16);
      ID(SINT_32, SINT32);
      ID(SINT_64, SINT64);
      ID(SINT_128, SINT128);
      ID(SINT_256, SINT256);

      ID(FLOAT, FLOAT32);

      ID(FLOAT_16, FLOAT16);
      ID(FLOAT_32, FLOAT32);
      ID(FLOAT_64, FLOAT64);
      ID(FLOAT_128, FLOAT128);
      ID(FLOAT_256, FLOAT256);


      ID(ENUM, ENUM);
      ID(MASK, MASK);
      ID(UNION, UNION);
      ID(NAMESPACE, NAMESPACE);
      ID(INTERFACE, INTERFACE);
      ID(STRUCT, STRUCT);
      ID(CLASS, CLASS);

      default: UNREACHABLE;
   }
   return make<FundType>(id);
}
#undef ID

clef::index<clef::astNode> clef::SyntaxTree::getValueKeyword(const KeywordID keyword) {
   switch (keyword) {
      case KeywordID::THIS    : return +make<Identifier>(KeywordID::THIS);
      case KeywordID::SELF    : return +make<Identifier>(KeywordID::SELF);
      case KeywordID::TRUE    : return +make<Literal>(true);
      case KeywordID::FALSE   : return +make<Literal>(false);
      case KeywordID::NULLPTR : return +make<Literal>((void*)nullptr);

      default: UNREACHABLE;
   }
}

clef::index<clef::Expr> clef::SyntaxTree::makeExpr(const OpID op, index<astNode> index) {
   if (!index) {
      return make<Expr>(op, NodeType::NONE, index);
   }
   astNode& node = self[index];
   switch (node.nodeType()) {
      case Identifier::nodeType():
      case Variable::nodeType():
      case Function::nodeType():
      case Macro::nodeType():
      case Type::nodeType():
      case VariadicParameter::nodeType():
      case FundamentalType::nodeType():
      case FunctionSignature::nodeType():
      case Enum::nodeType():
      case Mask::nodeType():
      case Union::nodeType():
      case Namespace::nodeType():
      case Interface::nodeType():
      case Struct::nodeType():
      case Class::nodeType():
      case GenericType::nodeType():
      case Literal::nodeType():
         return make<Expr>(op, node.nodeType(), index);

      case Expression::nodeType():
      case Declaration::nodeType():
      case TypeDeclaration::nodeType():
      case Statement::nodeType():
      case ForLoop::nodeType():
      case ForeachLoop::nodeType():
      case WhileLoop::nodeType():
      case DoWhileLoop::nodeType():
      case If::nodeType():
      case Switch::nodeType():
      case Match::nodeType():
      case TryCatch::nodeType():
      case Asm::nodeType():
      case ObjTypeSpec::nodeType():
      case InterfaceSpec::nodeType():
      case NamespaceSpec::nodeType():
         return +index;


      case Scope::nodeType():
      case ForLoopParams::nodeType():
      case ForeachLoopParams::nodeType():
      case SwitchCases::nodeType():
      case MatchCases::nodeType():
      case StatementSequence::nodeType():
      case ArgumentList::nodeType():
      case ParameterList::nodeType():
      case SpecializerList::nodeType():
      case NodeType::NONE:
      case NodeType::ERROR:
         UNREACHABLE;
   }
}

clef::index<clef::Expr> clef::SyntaxTree::makeExpr(const OpID op, index<astNode> lhsI, index<astNode> rhsI) {
   return make<Expr>(op, lhsI ? self[lhsI].nodeType() : NodeType::NONE, rhsI ? self[rhsI].nodeType() : NodeType::NONE, lhsI, rhsI);
}

clef::index<clef::Expr> clef::SyntaxTree::remakeTernary(index<astNode> cond, index<Expr> vals) {
   return remake<Expr>(vals, Expr::makeTernary(self, cond, +self[vals].lhs(), +self[vals].rhs()));
}

clef::SymbolNode* clef::SyntaxTree::findSymbol(const mcsl::str_slice name, SymbolNode* parentScope) {
   if (parentScope == nullptr) { parentScope = &_globalScope; }
   if (SymbolNode** entry = parentScope->find(name)) {
      return *entry;
   }
   return nullptr;
}
clef::SymbolNode* clef::SyntaxTree::registerSymbol(const mcsl::str_slice name, SymbolNode* parentScope) {
   if (parentScope == nullptr) {
      parentScope = &_globalScope;
   }
   SymbolNode& table = *parentScope;
   if (SymbolNode** entry = table.find(name)) {
      return *entry;
   }
   SymbolNode* entry = _symbolBuf.emplace_back(name, parentScope);
   table[name] = entry;
   return entry;
}
clef::SymbolNode* clef::SyntaxTree::registerAlias(SymbolNode* alias, SymbolNode* target) {
   debug_assert(alias);
   debug_assert(target);

   if (*alias) { //prevent illegal redeclarations from causing memory leaks
      throwError(ErrCode::BAD_IDEN, FMT("identifier `%s` redefined as alias"), alias->name());
      //!TODO: maybe just return a nullptr and let the parser handle it
   }

   (*alias->parentScope())[alias->name()] = target;
   std::destroy_at(alias);
   return target;
}

#endif //SYNTAX_TREE_CPP