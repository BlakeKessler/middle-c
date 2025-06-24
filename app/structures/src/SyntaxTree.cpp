#ifndef SYNTAX_TREE_CPP
#define SYNTAX_TREE_CPP

#include "SyntaxTree.hpp"

#include "pretty-print.hpp"
#include "io.hpp"

#include "MAP_MACRO.h"

clef::index<clef::astNode> clef::SyntaxTree::getValueKeyword(const KeywordID keyword) {
   switch (keyword) {
      case KeywordID::THIS    : return +make<Identifier>(KeywordID::THIS, nullptr);
      case KeywordID::SELF    : return +make<Identifier>(KeywordID::SELF, nullptr);
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
      case Literal::nodeType():
         return make<Expr>(op, node.nodeType(), index);

      case Expression::nodeType():
      case Declaration::nodeType():
      case FunctionDefinition::nodeType():
      case MacroDefinition::nodeType():
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
         return +index;


      case Scope::nodeType():
      case ForLoopParams::nodeType():
      case ForeachLoopParams::nodeType():
      case SwitchCases::nodeType():
      case MatchCases::nodeType():
      case StatementSequence::nodeType():
      case ArgumentList::nodeType():
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
   if (SymbolNode* entry = parentScope->get(name)) {
      return entry;
   }
   return nullptr;
}
clef::SymbolNode* clef::SyntaxTree::registerSymbol(const mcsl::str_slice name, SymbolNode* parentScope) {
   if (parentScope == nullptr) {
      parentScope = &_globalScope;
   }
   SymbolNode& table = *parentScope;
   if (SymbolNode* entry = table.get(name)) {
      return entry;
   }
   SymbolNode* entry = _symbolBuf.emplace_back(name, parentScope, nullptr, SymbolType::null);
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

clef::TypeSpec* clef::SyntaxTree::registerType(SymbolNode* name, TypeSpec::MetaType metatype, SymbolType symbolType) {
   debug_assert(!name || !name->type());
   TypeSpec* spec = _typeTable.emplace_back(metatype);
   if (name) {
      name->setType(spec);
      name->setSymbolType(symbolType);
   }
   return spec;
}

clef::TypeSpec* clef::SyntaxTree::makeIndirType(index<Identifier> targetNode, TypeSpec* pointee, QualMask quals, IndirTable::Entry firstEntry) {
   TypeSpec* spec = _typeTable.push_back(TypeSpec::makeIndir(pointee, quals, firstEntry));
   SymbolNode* symbol = _symbolBuf.push_back(SymbolNode::makeIndir(self[targetNode].symbol(), spec));
   self[targetNode].symbol() = symbol;

   debug_assert(symbol->symbolType() == SymbolType::INDIR);

   return spec;
}

#endif //SYNTAX_TREE_CPP