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

   if (symbol->symbolType() == SymbolType::FUND_TYPE) {
      __mangleFund(file, symbol->type()->fund().id,  data);
      return data.charsPrinted;
   }

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

uint clef::SyntaxTree::manglePrint(mcsl::File& file, SyntaxTree& tree, SymbolNode* symbol, mcsl::arr_span<index<Expr>> specializer) {
   debug_assert(symbol->symbolType() != SymbolType::FUNC);
   
   __MangleData data;

   if (symbol->symbolType() == SymbolType::FUND_TYPE) {
      __mangleFund(file, symbol->type()->fund().id,  data);
      return data.charsPrinted;
   }

   data.charsPrinted += file.printf(FMT(MANGLE_PREFIX));
   
   //name
   if (symbol->parentScope() != tree.globalScope()) {
      data.charsPrinted += file.printf(FMT(MANGLE_NAMESPACE_OPEN));
      __mangleImpl(file, tree, symbol, specializer, data, 0);
      data.charsPrinted += file.printf(FMT(MANGLE_NAMESPACE_CLOSE));
   } else {
      __mangleImpl(file, tree, symbol, specializer, data, 0);
   }

   //return
   return data.charsPrinted;
}
uint clef::SyntaxTree::manglePrint(mcsl::File& file, SyntaxTree& tree, SymbolNode* funcSymbol, mcsl::arr_span<index<Expr>> specializer, TypeSpec* sigSpec) {
   debug_assert(funcSymbol->symbolType() == SymbolType::FUNC);
   
   __MangleData data;

   data.charsPrinted += file.printf(FMT(MANGLE_PREFIX));
   
   //name
   if (funcSymbol->parentScope() != tree.globalScope()) {
      data.charsPrinted += file.printf(FMT(MANGLE_NAMESPACE_OPEN));
      __mangleImpl(file, tree, funcSymbol, specializer, data, 0);
      data.charsPrinted += file.printf(FMT(MANGLE_NAMESPACE_CLOSE));
   } else {
      __mangleImpl(file, tree, funcSymbol, specializer, data, 0);
   }
   
   //function signature
   auto& sig = sigSpec->funcSig();
   if (specializer.size()) { //return type (only for templated functions)
      if (sig.retType->metaType() == TypeSpec::FUNC_SIG) {
         TODO;
      } else {
         __mangleImpl(file, tree, sig.retType->canonName(), {}, data, 0); //!TODO: specializer
      }
   }
   for (auto [paramSpec, quals] : sig.params.span()) {
      //!TODO: quals
      if (paramSpec->metaType() == TypeSpec::FUNC_SIG) {
         TODO;
      } else {
         __mangleImpl(file, tree, paramSpec->canonName(), {}, data, 0); //!TODO: specializer
      }
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
      __mangleSpecializerImpl(file, tree, tree[name.specializer()].span(), data);
   }
}

void clef::SyntaxTree::__mangleImpl(mcsl::File& file, SyntaxTree& tree, SymbolNode* symbol, mcsl::arr_span<index<Expr>> specializer, __MangleData& data, uint depth) {
   debug_assert(symbol);
   if (symbol->parentScope() != tree.globalScope()) {
      //!TODO: parent scope specializer
      __mangleImpl(file, tree, symbol->parentScope(), {}, data, depth + 1);
   }

   if (symbol->symbolType() == SymbolType::FUND_TYPE) {
      __mangleFund(file, symbol->type()->fund().id, data);
   } else {
      //!TODO: substitutions
      //!TODO: function pointers
      data.charsPrinted += file.printf(FMT("%u%s"), symbol->name().size(), symbol->name());
   }
   
   if (specializer.size()) {
      __mangleSpecializerImpl(file, tree, specializer, data);
   }
}

void clef::SyntaxTree::__mangleFund(mcsl::File& file, FundTypeID fund, __MangleData& data) {
   if (fund == FundTypeID::null || fund == FundTypeID::FUNCTION_SIGNATURE) {
      TODO;
   }
   //!TODO: abbreviations for func sigs
   data.charsPrinted += file.printf(toString(fund));
}

void clef::SyntaxTree::__mangleFuncSigImpl(mcsl::File& file, SyntaxTree& tree, TypeSpec* sig, bool printRetType, __MangleData& data) {
   TODO;
}

void clef::SyntaxTree::__mangleSpecializerImpl(mcsl::File& file, SyntaxTree& tree, mcsl::arr_span<index<Expr>> spec, __MangleData& data) {
   data.charsPrinted += file.printf(FMT(MANGLE_SPECIALIZER_OPEN));
   
   for (index<Expr> i : spec) {
      debug_assert(i);
      Expr& expr = tree[i];
      if (expr.opID() == OpID::NULL && expr.lhs() && canDownCastTo(expr.lhsType(), NodeType::IDEN) && !expr.rhs() && !expr.extra() && !expr.extra2()) {
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

const mcsl::str_slice clef::SyntaxTree::mangleOp(OpID op, bool hasLHS, bool hasRHS) {
   using enum OpID;
   
   //!TODO: make a single static character blob instead of the switch statement (will improve all performance metrics)
   //       - begin: `2 * INDEX(op, hasLHS, hasRHS)`
   //       - size: 2
   //       - first char ==  0 --> error, second char will have the responsible operator
   //       - first char == -1 --> special case (intended for `??` and `??=`), second char will have the responsible operator
   
   #define INDEX(op, lhs, rhs) ((+op << 2) | (((bool)lhs) << 1) | ((bool)rhs))
   #define CASE(op, lhs, rhs, str) case INDEX(op, lhs, rhs): return FMT(str)
   #define STD_BIN(op, binstr, assignstr) \
      CASE(op, true, true, binstr); \
      CASE(op##_ASSIGN, true, true, assignstr)
   #define STD_UN_BIN(op, unstr, binstr, assignstr) \
      CASE(op, false, true, unstr); \
      STD_BIN(op, binstr, assignstr)

   #define BIT_AND_ASSIGN AND_ASSIGN
   #define BIT_OR_ASSIGN OR_ASSIGN
   #define BIT_XOR_ASSIGN XOR_ASSIGN
   #define SHIFT_LEFT_ASSIGN SHL_ASSIGN
   #define SHIFT_RIGHT_ASSIGN SHR_ASSIGN

   switch (INDEX(op, hasLHS, hasRHS)) {
      STD_UN_BIN(ADD, "ps", "pl", "pL"); //positive, plus
      STD_UN_BIN(SUB, "ng", "mi", "mI"); //negate, minus
      STD_UN_BIN(MUL, "de", "ml", "mL"); //deref, multiply
      STD_UN_BIN(BIT_AND, "ad", "an", "aN"); //address, and

      STD_BIN(DIV, "dv", "dV"); //divide
      STD_BIN(MOD, "rm", "rM"); //remainder
      STD_BIN(BIT_OR, "or", "oR"); //or
      STD_BIN(BIT_XOR, "eo", "eO"); //exclusive or
      STD_BIN(SHIFT_LEFT, "ls", "lS"); //left shift
      STD_BIN(SHIFT_RIGHT, "rs", "rS"); //right shift

      CASE(ASSIGN, true, true, "aS"); //assign

      CASE(BIT_NOT, false, true, "co"); //complement

      CASE(LESSER, true, true, "lt"); //less than
      CASE(GREATER, true, true, "gt"); //greater than
      CASE(LESSER_OR_EQ, true, true, "le"); //less than
      CASE(GREATER_OR_EQ, true, true, "ge"); //greater than
      CASE(THREE_WAY_COMP, true, true, "ss"); //spaceship

      CASE(LOGICAL_AND, true, true, "aa"); //and and
      CASE(LOGICAL_OR, true, true, "oo"); //or or
      CASE(LOGICAL_NOT, false, true, "nt"); //not

      CASE(IS_EQUAL, true, true, "eq"); //equal
      CASE(IS_UNEQUAL, true, true, "ne"); //not equal

      CASE(INC, true, false, "pp"); //plus plus
      CASE(DEC, true, false, "mm"); //minus minus

      CASE(PTR_MEMBER_ACCESS, true, true, "pt"); //pointer
      CASE(ARROW_METHOD_PTR, true, true, "pm"); //pointer to member

      CASE(COMMA, true, true, "cm"); //comma

      CASE(CALL_INVOKE, true, true, "cl"); //call
      CASE(SUBSCRIPT_INVOKE, true, true, "ix"); //index
      
      case INDEX(COALESCE, true, true): TODO;
      case INDEX(COALESCE_ASSIGN, true, true): TODO;

      default: UNREACHABLE;
   }

   #undef SHR
   #undef SHL
   #undef BIT_XOR_ASSIGN
   #undef BIT_OR_ASSIGN
   #undef BIT_AND_ASSIGN

   #undef STD_UN_BIN
   #undef STD_BIN
   #undef CASE
   #undef INDEX
}

#endif //AST_MANGLE_CPP