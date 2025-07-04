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
   _strings.emplace(name);
   mcsl::str_slice canonName = _strings.find(name)->slice();
   SymbolNode* entry = _symbolBuf.emplace_back(canonName, parentScope, nullptr, SymbolType::EXTERN_IDEN);
   debug_assert(entry->parentScope() == parentScope);
   table.insert(entry);
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
      if (symbolType != SymbolType::null && (name->symbolType() == SymbolType::null || name->symbolType() == SymbolType::EXTERN_IDEN || name->symbolType() == SymbolType::EXTERN_TYPE)) {
         name->setSymbolType(symbolType);
      }
      spec->canonName() = name;
   }
   return spec;
}

clef::TypeSpec* clef::SyntaxTree::makeIndirType(index<Identifier> targetNode, TypeSpec* pointee, QualMask quals, IndirTable&& table) {
   TypeSpec* spec = _indirTypes.get_or_insert(pointee, quals, std::forward<IndirTable&&>(table), _typeTable);
   if (targetNode) { [[likely]];
      SymbolNode* symbol = _symbolBuf.push_back(SymbolNode::makeIndir(self[targetNode].symbol(), spec));
      self[targetNode].symbol() = symbol;

      debug_assert(symbol->symbolType() == SymbolType::INDIR);
   }

   return spec;
}

mcsl::str_slice clef::SyntaxTree::storeString(const mcsl::str_slice str) {
   _strings.insert(str);
   return *_strings.find(str);
}
mcsl::str_slice clef::SyntaxTree::storeString(mcsl::string&& str) {
   bool didInsert = _strings.insert(str);
   mcsl::str_slice slice = *_strings.find(str);
   if (!didInsert) {
      std::destroy_at(&str);
   }
   return slice;
}

clef::TypeSpec* clef::SyntaxTree::evalType(index<astNode> i) {
   if (!i) {
      return nullptr;
   }
   switch (self[i].nodeType()) {
      case NodeType::NONE: [[fallthrough]];
      case NodeType::ERROR:
         return nullptr;

      case NodeType::IDEN:
         return self[(index<Identifier>)i].symbol()->type();
         
      case NodeType::SCOPE: return nullptr; //!NOTE: this might not be right

      case NodeType::LITERAL: {
         Literal& lit = self[(index<Literal>)i];
         switch (lit.type()) {
            case LitType::NONE: return nullptr;
            
            case LitType::POINTER: TODO;

            case LitType::UINT: return getFundType(KeywordID::UINT)->type();
            case LitType::SINT: return getFundType(KeywordID::SINT)->type();
            case LitType::FLOAT: return getFundType(KeywordID::FLONG)->type();

            case LitType::BOOL: return getFundType(KeywordID::BOOL)->type();
            case LitType::CHAR: return getFundType(KeywordID::CHAR)->type();

            case LitType::STRING: { //const char[]
               SymbolNode* charType = getFundType(KeywordID::CHAR);
               return makeIndirType(0, charType->type(), QualMask::CONST, IndirTable(IndirTable::Entry(IndirTable::Entry::SLICE, true, false, false)));
            }
            case LitType::INTERP_STR: TODO;
            case LitType::FORMAT: TODO;
            case LitType::REGEX: TODO;

            case LitType::TYPEID: return static_cast<TypeSpec*>(lit);
         }
         UNREACHABLE;
      }

      #pragma region exprs
      case NodeType::EXPR: [[fallthrough]];
      case NodeType::STMT: [[fallthrough]];
      case NodeType::DECL: [[fallthrough]];
      case NodeType::FUNC_DEF: [[fallthrough]];
      case NodeType::MACRO_DEF: [[fallthrough]];
      case NodeType::MAKE_TYPE: [[fallthrough]];
      case NodeType::FOR_LOOP: [[fallthrough]];
      case NodeType::FOREACH_LOOP: [[fallthrough]];
      case NodeType::WHILE_LOOP: [[fallthrough]];
      case NodeType::DO_WHILE_LOOP: [[fallthrough]];
      case NodeType::IF: [[fallthrough]];
      case NodeType::SWITCH: [[fallthrough]];
      case NodeType::MATCH: [[fallthrough]];
      case NodeType::ASM: [[fallthrough]];
      case NodeType::TRY_CATCH:
         return self[(index<Expr>)i].evalType();
      #pragma endregion exprs

      case NodeType::SWITCH_CASES: [[fallthrough]];
      case NodeType::MATCH_CASES: [[fallthrough]];
      case NodeType::STMT_SEQ: [[fallthrough]];
      case NodeType::ARG_LIST: return nullptr;
   }
}
void clef::SyntaxTree::updateEvalType(index<Expr> i) {
   debug_assert(i);

   Expr& expr = self[i];

   if (expr.opID() == OpID::CALL_INVOKE || expr.opID() == OpID::LIST_INVOKE) {
      if (expr.lhsType() == NodeType::IDEN) {
         Identifier& lhs = self[(index<Identifier>)expr.lhs()];
         SymbolNode* symbol = lhs.symbol();
         if (isType(symbol->symbolType())) {
            expr.evalType() = symbol->type();
         } else if (symbol->symbolType() == SymbolType::FUNC) {
            //!TODO: deduce and update overload index
            expr.evalType() = symbol->getOverload(lhs.overloadIndex()).first->funcSig().retType;
         }
         else {
            TODO;
         }
         return;
      } else {
         TODO;
      }
   }

   #define IS_PRIM(name) (!expr.name() || !evalType(+expr.name()) || evalType(+expr.name())->metaType() == TypeSpec::FUND_TYPE)
   bool isAllPrimitive =  IS_PRIM(lhs) && IS_PRIM(rhs) && IS_PRIM(extra) && IS_PRIM(extra2);
   #undef IS_PRIM

   if (isAllPrimitive) {
      switch (expr.opID()) {
            case OpID::TERNARY_INVOKE: 
               if (!(expr.evalType() = commonType(evalType(+expr.rhs()), evalType(+expr.extra())))) { //intentionally uses assignment
                  throwError(ErrCode::TYPECHECK_ERR, FMT("operands of ternary expression must have a common type"));
               }
               return;


            case OpID::PREPROCESSOR: TODO;

            case OpID::SCOPE_RESOLUTION: UNREACHABLE;

               TODO;

            case OpID::RANGE: TODO;
            case OpID::SPREAD: TODO;

            #pragma region commons
            //unary identity
            case OpID::INC: [[fallthrough]];
            case OpID::DEC: [[fallthrough]];
            case OpID::BIT_NOT: [[fallthrough]];
            //bitwise binary identity
            case OpID::BIT_AND: [[fallthrough]];
            case OpID::BIT_OR: [[fallthrough]];
            case OpID::BIT_XOR: [[fallthrough]];
            case OpID::SHIFT_LEFT: [[fallthrough]];
            case OpID::SHIFT_RIGHT: [[fallthrough]];
            //binary identity
            case OpID::ADD: [[fallthrough]];
            case OpID::SUB: [[fallthrough]];
            case OpID::MUL: [[fallthrough]];
            case OpID::DIV: [[fallthrough]];
            case OpID::MOD: [[fallthrough]];
            case OpID::EXP: [[fallthrough]];
            case OpID::COALESCE:
               expr.evalType() = commonTypeOfOperands(i);
               return;
            #pragma endregion commons

            #pragma region bools
            //unary boolean
            case OpID::LOGICAL_NOT: [[fallthrough]];
            //binary boolean
            case OpID::LOGICAL_AND: [[fallthrough]];
            case OpID::LOGICAL_OR: [[fallthrough]];
            case OpID::LESSER: [[fallthrough]];
            case OpID::GREATER: [[fallthrough]];
            case OpID::LESSER_OR_EQ: [[fallthrough]];
            case OpID::GREATER_OR_EQ: [[fallthrough]];
            case OpID::IS_EQUAL: [[fallthrough]];
            case OpID::IS_UNEQUAL: [[fallthrough]];
            //case OpID::IS_EQUAL_STRICT: [[fallthrough]];
            //case OpID::IS_UNEQUAL_STRICT: [[fallthrough]];
            case OpID::THREE_WAY_COMP:
               expr.evalType() = getFundType(KeywordID::BOOL)->type();
               return;
            #pragma endregion bools

            #pragma region lhss
            //null
            case OpID::NULL:
            //assignments
            case OpID::ASSIGN: [[fallthrough]];
            //case OpID::CONST_ASSIGN: [[fallthrough]];
            case OpID::ADD_ASSIGN: [[fallthrough]];
            case OpID::SUB_ASSIGN: [[fallthrough]];
            case OpID::MUL_ASSIGN: [[fallthrough]];
            case OpID::DIV_ASSIGN: [[fallthrough]];
            case OpID::MOD_ASSIGN: [[fallthrough]];
            case OpID::EXP_ASSIGN: [[fallthrough]];
            case OpID::SHL_ASSIGN: [[fallthrough]];
            case OpID::SHR_ASSIGN: [[fallthrough]];
            case OpID::AND_ASSIGN: [[fallthrough]];
            case OpID::XOR_ASSIGN: [[fallthrough]];
            case OpID::OR_ASSIGN: [[fallthrough]];
            case OpID::COALESCE_ASSIGN:
               expr.evalType() = evalType(+expr.lhs());
               return;
            #pragma endregion lhss

            #pragma region rhss
            case OpID::COMMA: [[fallthrough]];
            case OpID::LET: [[fallthrough]];
            case OpID::MAKE_TYPE:
               expr.evalType() = evalType(+expr.rhs());
               return;
            #pragma endregion rhss

            #pragma region typelesses
            //pseudo-operators
            case OpID::FOR: [[fallthrough]];
            case OpID::FOREACH: [[fallthrough]];
            case OpID::WHILE: [[fallthrough]];
            case OpID::DO_WHILE: [[fallthrough]];
            case OpID::GOTO: [[fallthrough]];
            case OpID::GOTO_CASE: [[fallthrough]];
            case OpID::IF: [[fallthrough]];
            case OpID::SWITCH: [[fallthrough]];
            case OpID::MATCH: [[fallthrough]];
            case OpID::TRY_CATCH: [[fallthrough]];
            case OpID::ASM: [[fallthrough]];
            case OpID::BREAK: [[fallthrough]];
            case OpID::CONTINUE: [[fallthrough]];
            case OpID::THROW: [[fallthrough]];
            case OpID::ASSERT: [[fallthrough]];
            case OpID::DEBUG_ASSERT: [[fallthrough]];
            case OpID::STATIC_ASSERT: [[fallthrough]];
            case OpID::ASSUME: [[fallthrough]];
            case OpID::RETURN: [[fallthrough]];
            case OpID::ALIAS: [[fallthrough]];
            case OpID::DEF_FUNC_PARAMS: [[fallthrough]];
            case OpID::DEF_MACRO_PARAMS: [[fallthrough]];
            //labels
            case OpID::LABEL_DELIM:
               expr.evalType() = nullptr;
               return;
            #pragma endregion typelesses

            #pragma region errs
            //dereferences
            case OpID::MEMBER_ACCESS: [[fallthrough]];
            case OpID::PTR_MEMBER_ACCESS: [[fallthrough]];
            case OpID::METHOD_PTR: [[fallthrough]];
            case OpID::ARROW_METHOD_PTR: [[fallthrough]];
            //invokes
            case OpID::CALL_INVOKE: [[fallthrough]];
            case OpID::SUBSCRIPT_INVOKE: [[fallthrough]];
            case OpID::LIST_INVOKE: [[fallthrough]];
            case OpID::SPECIALIZER_INVOKE: [[fallthrough]];
            case OpID::INTERP_STR_INVOKE: throwError(ErrCode::BAD_EXPR, FMT("invalid operator for types (%s)"), toString(expr.opID()));
            #pragma endregion errs

            #pragma region unreachables
            case OpID::INLINE_IF: [[fallthrough]];
            case OpID::UNIQUE_PTR: [[fallthrough]];
            case OpID::SHARED_PTR: [[fallthrough]];
            case OpID::WEAK_PTR: [[fallthrough]];
            case OpID::ITERATOR: [[fallthrough]];
            case OpID::PREPROC_IMPORT: [[fallthrough]];
            case OpID::PREPROC_LINK: [[fallthrough]];
            case OpID::PREPROC_EMBED: [[fallthrough]];
            case OpID::ESCAPE: [[fallthrough]];
            case OpID::EOS: [[fallthrough]];
            case OpID::STRING: [[fallthrough]];
            case OpID::CHAR: [[fallthrough]];
            case OpID::INTERP_STRING: [[fallthrough]];
            case OpID::LINE_CMNT: [[fallthrough]];
            case OpID::BLOCK_CMNT: [[fallthrough]];
            case OpID::BLOCK_CMNT_OPEN: [[fallthrough]];
            case OpID::BLOCK_CMNT_CLOSE: [[fallthrough]];
            case OpID::CALL_OPEN: [[fallthrough]];
            case OpID::CALL_CLOSE: [[fallthrough]];
            case OpID::SUBSCRIPT_OPEN: [[fallthrough]];
            case OpID::SUBSCRIPT_CLOSE: [[fallthrough]];
            case OpID::LIST_OPEN: [[fallthrough]];
            case OpID::LIST_CLOSE: [[fallthrough]];
            case OpID::SPECIALIZER_OPEN: [[fallthrough]];
            case OpID::SPECIALIZER_CLOSE: [[fallthrough]];
            case OpID::CHAR_INVOKE: [[fallthrough]];
            case OpID::STR_INVOKE: UNREACHABLE;
            #pragma endregion unreachables
         }
   }

   switch (expr.opID()) {
      #define __TYPELESS expr.evalType() = nullptr; return
      case OpID::NULL:
         debug_assert(!expr.rhs() && !expr.extra() && !expr.extra2());
         expr.evalType() = evalType(+expr.lhs());
         return;

      case OpID::ESCAPE: UNREACHABLE;
      case OpID::EOS: UNREACHABLE;

      case OpID::STRING: UNREACHABLE;
      case OpID::CHAR: UNREACHABLE;
      case OpID::INTERP_STRING: UNREACHABLE;

      case OpID::LINE_CMNT: UNREACHABLE;
      case OpID::BLOCK_CMNT: UNREACHABLE;
      case OpID::BLOCK_CMNT_OPEN: UNREACHABLE;
      case OpID::BLOCK_CMNT_CLOSE: UNREACHABLE;

      case OpID::CALL_OPEN: UNREACHABLE;
      case OpID::CALL_CLOSE: UNREACHABLE;
      case OpID::SUBSCRIPT_OPEN: UNREACHABLE;
      case OpID::SUBSCRIPT_CLOSE: UNREACHABLE;
      case OpID::LIST_OPEN: UNREACHABLE;
      case OpID::LIST_CLOSE: UNREACHABLE;
      case OpID::SPECIALIZER_OPEN: UNREACHABLE;
      case OpID::SPECIALIZER_CLOSE: UNREACHABLE;

      case OpID::CALL_INVOKE:
      case OpID::SUBSCRIPT_INVOKE:
      case OpID::LIST_INVOKE:
      case OpID::SPECIALIZER_INVOKE:

         TODO;

      case OpID::CHAR_INVOKE: UNREACHABLE;
      case OpID::STR_INVOKE: UNREACHABLE;
      case OpID::INTERP_STR_INVOKE: TODO;
      case OpID::TERNARY_INVOKE: 
         if (!(expr.evalType() = commonType(evalType(+expr.rhs()), evalType(+expr.extra())))) {
            throwError(ErrCode::TYPECHECK_ERR, FMT("operands of ternary expression must have a common type"));
         }
         return;


      case OpID::PREPROCESSOR: TODO;

      case OpID::SCOPE_RESOLUTION: UNREACHABLE;

         TODO;

      case OpID::MEMBER_ACCESS: [[fallthrough]];
      case OpID::PTR_MEMBER_ACCESS: [[fallthrough]];
      case OpID::METHOD_PTR: [[fallthrough]];
      case OpID::ARROW_METHOD_PTR:
         expr.evalType() = evalType(+expr.rhs());
         return;

      case OpID::RANGE: TODO;
      case OpID::SPREAD: TODO;

      //unary identity
      case OpID::INC:
      case OpID::DEC:
      case OpID::BIT_NOT:

      //bitwise binary identity
      case OpID::BIT_AND:
      case OpID::BIT_OR:
      case OpID::BIT_XOR:
      case OpID::SHIFT_LEFT:
      case OpID::SHIFT_RIGHT:

      //bitwise binary identity
      case OpID::ADD:
      case OpID::SUB:
      case OpID::MUL:
      case OpID::DIV:
      case OpID::MOD:
      case OpID::EXP:

      case OpID::THREE_WAY_COMP: TODO;

      //unary boolean
      case OpID::LOGICAL_NOT:

      //binary boolean
      case OpID::LOGICAL_AND:
      case OpID::LOGICAL_OR:
      case OpID::LESSER:
      case OpID::GREATER:
      case OpID::LESSER_OR_EQ:
      case OpID::GREATER_OR_EQ:
      case OpID::IS_EQUAL:
      case OpID::IS_UNEQUAL:
      //case OpID::IS_EQUAL_STRICT:
      //case OpID::IS_UNEQUAL_STRICT:


      case OpID::UNIQUE_PTR: TODO;
      case OpID::SHARED_PTR: TODO;
      case OpID::WEAK_PTR: TODO;
      case OpID::ITERATOR: TODO;



      case OpID::COALESCE: TODO;

      case OpID::INLINE_IF: UNREACHABLE;
      case OpID::LABEL_DELIM: __TYPELESS;

      case OpID::ASSIGN:
         expr.evalType() = evalType(+expr.lhs());
         return;
      //case OpID::CONST_ASSIGN: TODO;
      case OpID::ADD_ASSIGN:
      case OpID::SUB_ASSIGN:
      case OpID::MUL_ASSIGN:
      case OpID::DIV_ASSIGN:
      case OpID::MOD_ASSIGN:
      case OpID::EXP_ASSIGN:
      case OpID::SHL_ASSIGN:
      case OpID::SHR_ASSIGN:
      case OpID::AND_ASSIGN:
      case OpID::XOR_ASSIGN:
      case OpID::OR_ASSIGN:
      case OpID::COALESCE_ASSIGN:


         TODO;

      case OpID::COMMA:
         expr.evalType() = evalType(+expr.rhs());
         return;

      case OpID::FOR: [[fallthrough]];
      case OpID::FOREACH: [[fallthrough]];
      case OpID::WHILE: [[fallthrough]];
      case OpID::DO_WHILE: [[fallthrough]];

      case OpID::GOTO: [[fallthrough]];
      case OpID::GOTO_CASE: [[fallthrough]];

      case OpID::IF: [[fallthrough]];

      case OpID::SWITCH: [[fallthrough]];
      case OpID::MATCH: [[fallthrough]];

      case OpID::TRY_CATCH: [[fallthrough]];
      case OpID::ASM: [[fallthrough]];

      case OpID::BREAK: [[fallthrough]];
      case OpID::CONTINUE: [[fallthrough]];

      case OpID::THROW: [[fallthrough]];
      case OpID::ASSERT: [[fallthrough]];
      case OpID::DEBUG_ASSERT: [[fallthrough]];
      case OpID::STATIC_ASSERT: [[fallthrough]];
      case OpID::ASSUME: [[fallthrough]];
      case OpID::RETURN: [[fallthrough]];

      case OpID::ALIAS:
         __TYPELESS;

      case OpID::LET: [[fallthrough]];
      case OpID::MAKE_TYPE:
         expr.evalType() = evalType(+expr.rhs());
         return;

      case OpID::DEF_FUNC_PARAMS: [[fallthrough]];
      case OpID::DEF_MACRO_PARAMS: __TYPELESS;


      case OpID::PREPROC_IMPORT: TODO;
      case OpID::PREPROC_LINK: TODO;
      case OpID::PREPROC_EMBED: TODO;
      #undef __TYPELESS
   }
}
void clef::SyntaxTree::updateEvalType_r(index<Expr> i) {
   debug_assert(i);

   //recursively update types
   Expr& expr = self[i];
   #define RECURSE(name, type) \
   if (expr.name() && canDownCastTo(expr.type(), NodeType::EXPR)) { \
      updateEvalType_r(+expr.name()); \
   }
   RECURSE(lhs, lhsType);
   RECURSE(rhs, rhsType);
   RECURSE(extra, extraType);
   RECURSE(extra2, extraType2);
   #undef RECURSE

   //update own type
   updateEvalType(i);
}

clef::TypeSpec* clef::SyntaxTree::commonType(TypeSpec* t1, TypeSpec* t2) {
   if (!t1) { return t2; }
   if (!t2) { return t1; }

   if (t1->metaType() == TypeSpec::FUND_TYPE && t2->metaType() == TypeSpec::FUND_TYPE) {
      FundTypeID f1 = t1->fund().id;
      FundTypeID f2 = t2->fund().id;
      FundTypeID common;
      if (f1 == FundTypeID::AUTO) { common = f2; }
      else if (f2 == FundTypeID::AUTO) { common = f1; }
      else if (isNum(f1) && isNum(f2)) { //both numeric types
         if (isFloat(f1)) { //f1 is float
            common = (isFloat(f2) && f2 >= f1) ? f2 : f1;
         } else if (isFloat(f2)) { //f2 is float and f1 is not float
            common = f2;
         } else if (isUint(f1) == isUint(f2)) { //both uint or both sint
            common = f1 >= f2 ? f1 : f2;
         } else { //a uint and an sint
            //use the unsigned version of the larger type
            if (sizeOf(f1, dataModel()) < sizeOf(f2, dataModel())) {
               common = toUint(f1);
            } else {
               common = toUint(f2);
            }
         }
      } else {
         TODO;
      }
      return getFundType(common)->type();
   }

   TODO;
}

clef::TypeSpec* clef::SyntaxTree::commonTypeOfOperands(index<Expr> i) {
   if (!i) {
      return nullptr;
   }

   Expr& expr = self[i];

   TypeSpec* spec = nullptr;
   #define UPDATE(name) \
   if (expr.name()) { \
      spec = commonType(spec, evalType(+expr.name())); \
      if (!spec) { \
         throwError(ErrCode::TYPECHECK_ERR, FMT("no common type")); \
      } \
   }
   UPDATE(lhs);
   UPDATE(rhs);
   UPDATE(extra);
   UPDATE(extra2);
   #undef UPDATE

   return spec;
}

#endif //SYNTAX_TREE_CPP