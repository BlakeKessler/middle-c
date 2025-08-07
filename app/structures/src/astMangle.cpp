#ifndef AST_MANGLE_CPP
#define AST_MANGLE_CPP

#include "SyntaxTree.hpp"
#include "pretty-print.hpp"

sint clef::SyntaxTree::__MangleData::substIndex(SymbolNode* symbol) {
   for (uint i = 0; i < substitutions.size(); ++i) {
      if (substitutions[i] == symbol) {
         return i;
      }
   }
   substitutions.push_back(symbol);
   return -1;
}

uint clef::SyntaxTree::manglePrint(mcsl::File& file, SyntaxTree& tree, index<Identifier> i) {
   if (!i) {
      TODO;
   }
   Identifier& name = tree[i];
   SymbolNode* symbol = name.symbol();

   __MangleData data;
   data.charsPrinted += file.printf(FMT(MANGLE_PREFIX));
   
   //name
   if (symbol->parentScope() != tree.globalScope()) {
      data.charsPrinted += file.printf(FMT(MANGLE_NAMESPACE_OPEN));
      __mangleImpl(file, tree, name, data, 0);
      data.charsPrinted += file.printf(FMT(MANGLE_NAMESPACE_CLOSE));
   } else {
      __mangleImpl(file, tree, name, data, 0);
   }
   
   //function signature
   if (symbol->symbolType() == SymbolType::FUNC) {
      __mangleFuncSigImpl(file, tree, symbol->getOverload(name.overloadIndex()).first, false, data);
   }

   //return
   return data.charsPrinted;
}

void clef::SyntaxTree::__mangleImpl(mcsl::File& file, SyntaxTree& tree, Identifier& name, __MangleData& data, uint depth) {
   SymbolNode* symbol = name.symbol();
   debug_assert(symbol);
   if (symbol->parentScope() != tree.globalScope()) {
      __mangleImpl(file, tree, tree[name.scopeName()], data, depth + 1);
   }

   if (+name.fundTypeID()) {
      __mangleFund(file, name.fundTypeID(), data);
   } else {
      //!TODO: substitutions
      //!TODO: function pointers
      data.charsPrinted += file.printf(FMT("%u%s"), symbol->name().size(), symbol->name());
   }
   
   if (name.specializer()) {
      __mangleSpecializerImpl(file, tree, name, data);
   }
}

void clef::SyntaxTree::__mangleFund(mcsl::File& file, FundTypeID fund, __MangleData& data) {
   if (fund == FundTypeID::null || fund == FundTypeID::FUNCTION_SIGNATURE) {
      TODO;
   }
   data.charsPrinted += file.printf(toString(fund));
}

void clef::SyntaxTree::__mangleFuncSigImpl(mcsl::File& file, SyntaxTree& tree, TypeSpec* sig, bool printRetType, __MangleData& data) {
   TODO;
}

void clef::SyntaxTree::__mangleSpecializerImpl(mcsl::File& file, SyntaxTree& tree, Identifier& name, __MangleData& data) {
   debug_assert(name.specializer());

   data.charsPrinted += file.printf(FMT(MANGLE_SPECIALIZER_OPEN));
   
   auto span = tree[name.specializer()].span();
   for (index<Expr> i : span) {
      debug_assert(i);
      Expr& expr = tree[i];
      if (expr.opID() == OpID::NULL && expr.lhs() && canDownCastTo(expr.lhsType(), NodeType::EXPR) && !expr.rhs() && !expr.extra() && !expr.extra2()) {
         Identifier& iden = tree[(index<Identifier>)expr.lhs()];
         if (iden.scopeName()) {
            data.charsPrinted += file.printf(FMT(MANGLE_NAMESPACE_OPEN));
            __mangleImpl(file, tree, iden, data, 0);
            data.charsPrinted += file.printf(FMT(MANGLE_NAMESPACE_CLOSE));
         } else {
            __mangleImpl(file, tree, iden, data, 0);
         }
      } else {
         TODO;
      }
   }

   data.charsPrinted += file.printf(FMT(MANGLE_SPECIALIZER_CLOSE));
}

#endif //AST_MANGLE_CPP