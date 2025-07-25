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

clef::index<clef::Expr> clef::SyntaxTree::makeExpr(const OpID op, index<astNode> lhs) {
   if (!lhs) {
      return make<Expr>(op, NodeType::NONE, lhs);
   }
   astNode& node = self[lhs];
   switch (node.nodeType()) {
      case RawIdentifier::nodeType():
      case Identifier::nodeType():
      case Literal::nodeType():
         return make<Expr>(op, node.nodeType(), lhs);

      case Expr::nodeType():
      case Decl::nodeType():
      case FuncDef::nodeType():
      case MacroDef::nodeType():
      case TypeDecl::nodeType():
      case Stmt::nodeType():
      case ForLoop::nodeType():
      case ForeachLoop::nodeType():
      case WhileLoop::nodeType():
      case DoWhileLoop::nodeType():
      case If::nodeType():
      case Switch::nodeType():
      case Match::nodeType():
      case Asm::nodeType():
         return +lhs;


      case Scope::nodeType():
      case SwitchCases::nodeType():
      case MatchCases::nodeType():
      case StmtSeq::nodeType():
      case ArgList::nodeType():
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
      return nullptr;
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
   auto [spec, isNew] = _indirTypes.get_or_insert(pointee, quals, std::forward<IndirTable&&>(table), _typeTable);
   if (isNew) {
      SymbolNode* symbol = _symbolBuf.emplace_back(std::move(SymbolNode::makeIndir(spec->pointee()->canonName(), spec)));
      spec->canonName() = symbol;
      if (targetNode) {
         self[targetNode].symbol() = symbol;
      }

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

      case NodeType::RAW_IDEN:
         TODO;

      case NodeType::IDEN:
         return self[(index<Identifier>)i].symbol()->type();
         
      case NodeType::SCOPE: return nullptr; //!NOTE: this might not be right

      case NodeType::LITERAL: {
         Literal& lit = self[(index<Literal>)i];
         switch (lit.type()) {
            case LitType::NONE: return nullptr;
            
            case LitType::POINTER: TODO;

            case LitType::UINT: [[fallthrough]];
            case LitType::SINT: [[fallthrough]];
            case LitType::FLOAT: [[fallthrough]];

            case LitType::BOOL: [[fallthrough]];
            case LitType::CHAR: return getFundType(lit.typeName())->type();

            case LitType::STRING: { //const char[]
               SymbolNode* charType = getFundType(lit.typeName());
               return makeIndirType(0, charType->type(), QualMask::CONST, IndirTable(IndirTable::Entry(IndirTable::Entry::SLICE, false, false, false)));
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
      case NodeType::ASM:
         return self[(index<Expr>)i].evalType();
      #pragma endregion exprs

      case NodeType::SWITCH_CASES: [[fallthrough]];
      case NodeType::MATCH_CASES: [[fallthrough]];
      case NodeType::STMT_SEQ: [[fallthrough]];
      case NodeType::ARG_LIST: return nullptr;
   }
}
clef::res<void> clef::SyntaxTree::updateEvalType(index<Expr> i, SymbolNode* currScope) {
   debug_assert(i);

   Expr& expr = self[i];
   
   //constructors and function calls
   if (expr.opID() == OpID::CALL_INVOKE || expr.opID() == OpID::LIST_INVOKE) {
      if (canDownCastTo(expr.lhsType(), NodeType::IDEN)) {
         Identifier& lhs = self[(index<Identifier>)expr.lhs()];
         SymbolNode* symbol = lhs.symbol();
         if (isType(symbol->symbolType())) { //constructors
            expr.evalType() = symbol->type();
         } else if (symbol->symbolType() == SymbolType::FUNC) { //functions
            debug_assert(canDownCastTo(expr.rhsType(), NodeType::ARG_LIST));
            auto tmp = deduceOverload(symbol, +expr.rhs());
            if (!std::get<0>(tmp)) {
               return ErrCode::TYPECHECK_ERR; //!NOTE: error message: "no matching function signature found for func `%s(%s)`", astTNB{self, (index<Identifier>)expr.lhs(), 0}, astTNB{self, (index<ArgList>)expr.rhs(), 0}
            }
            lhs.overloadIndex() = std::get<1>(tmp);
            debug_assert(std::get<2>(tmp)->metaType() == TypeSpec::FUNC_SIG);
            expr.evalType() = std::get<2>(tmp)->funcSig().retType;
            return {};
         }
         else {
            TODO;
         }
         return {};
      }
      else if (canDownCastTo(expr.lhsType(), NodeType::EXPR)) {
         Expr& lhs = self[(index<Expr>)expr.lhs()];
         switch (lhs.evalType()->metaType()) {
            case TypeSpec::null: UNREACHABLE;
            case TypeSpec::FUND_TYPE: TODO;
            case TypeSpec::INDIR: TODO;
            case TypeSpec::COMPOSITE: TODO;
            case TypeSpec::FUNC_SIG: TODO;
            case TypeSpec::FUNC: {
               debug_assert(canDownCastTo(expr.rhsType(), NodeType::ARG_LIST));
               auto tmp = deduceOverload(lhs.evalType()->func(), +expr.rhs());
               if (!std::get<0>(tmp)) {
                  TODO;
               }
               debug_assert(std::get<2>(tmp)->metaType() == TypeSpec::FUNC_SIG);
               expr.evalType() = std::get<2>(tmp)->funcSig().retType;
               lhs.evalType() = std::get<2>(tmp); //!NOTE: might be a bad idea to alter another node's evalType
               break;
            }
         }
         return {};
      }
      else {
         //!NOTE: these print statements all for debugging
         mcsl::printf(FMT("%s\n"), astTNB{self, i, 0});
         mcsl::printf(FMT("op: %s   lhs: %s   rhs: %s"), toString(expr.opID()), toString(expr.lhsType()), toString(expr.rhsType()));
         mcsl::flush();
         TODO;
      }
   }

   //memory-related operators
   else if (!expr.lhs() && expr.rhs()) { //filter out non-prefix expressions
      if (expr.opID() == OpID::ADDRESS_OF) { //address of
         TypeSpec* pointee = evalType(+expr.rhs());
         TypeSpec* indir;
         if (pointee->metaType() == TypeSpec::INDIR) {
            indir = makeIndirType(0, pointee->pointee(), pointee->pointeeQuals(), IndirTable{pointee->indirTable(), IndirTable::Entry{IndirTable::Entry::PTR, false, false, false}});
         } else {
            indir = makeIndirType(0, pointee, QualMask::_no_quals, IndirTable::Entry{IndirTable::Entry::PTR, false, false, false});
         }
         expr.evalType() = indir;
         return {};
      }
      else if (expr.opID() == OpID::DEREF) { //dereference
         TypeSpec* ptr = evalType(+expr.rhs());
         TypeSpec* type;
         if (ptr->metaType() == TypeSpec::INDIR && ptr->indirTable().back().decaysToPtr()) {
            debug_assert(ptr->indirTable().size());
            if (ptr->indirTable().size() == ptr->indirTable().entryByteCount(0)) { //pointer to non-pointer type
               type = ptr->pointee();
            } else { //pointer to pointer-like type
               type = makeIndirType(0, ptr->pointee(), ptr->pointeeQuals(), IndirTable::makeDeref(ptr->indirTable()));
            }
         } else {
            //!TODO: references to pointers
            //!TODO: use return type of overloaded dereference operator
            TODO;
         }
         expr.evalType() = type;
         return {};
      }
   }
   

   //anything else
   TypeSpec* __tmp;
   #define IS_PRIM(name) (!expr.name() || !(__tmp = evalType(+expr.name())) || __tmp->metaType() == TypeSpec::FUND_TYPE || (__tmp->metaType() == TypeSpec::INDIR && __tmp->indirTable().back().decaysToFund()))
   bool isAllPrimitive = IS_PRIM(lhs) && IS_PRIM(rhs) && IS_PRIM(extra) && IS_PRIM(extra2);
   #undef IS_PRIM
   bool lhsIsPtrlike = expr.lhs() && (__tmp = evalType(+expr.lhs())) && __tmp->metaType() == TypeSpec::INDIR && __tmp->indirTable().back().decaysToFund();

   if (isAllPrimitive) {
      switch (expr.opID()) {
         case OpID::TERNARY_INVOKE:
            if (!(expr.evalType() = commonType(evalType(+expr.rhs()), evalType(+expr.extra())))) { //intentionally uses assignment
               return ErrCode::TYPECHECK_ERR; //!NOTE: error message: `operands of ternary expression must have a common type`
            }
            return {};


         case OpID::PREPROCESSOR: TODO;

         case OpID::SCOPE_RESOLUTION: UNREACHABLE;

         case OpID::RANGE: TODO;
         case OpID::SPREAD: TODO;

         case OpID::ATTRIBUTE: 
            expr.evalType() = evalType(+expr.extra2());
            return {};

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
         case OpID::COALESCE: {
            res<TypeSpec*> tmp = commonTypeOfOperands(i);
            if (tmp.is_err()) {
               return tmp.err();
            }
            expr.evalType() = tmp.ok();
            return {};
         }
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
            return {};
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
         case OpID::COALESCE_ASSIGN: [[fallthrough]];
         //casts
         case OpID::CAST: [[fallthrough]];
         case OpID::UP_CAST: [[fallthrough]];
         case OpID::DYN_CAST: [[fallthrough]];
         case OpID::BIT_CAST: [[fallthrough]];
         case OpID::CONST_CAST:
            expr.evalType() = evalType(+expr.lhs());
            return {};
         #pragma endregion lhss

         #pragma region rhss
         case OpID::COMMA: [[fallthrough]];
         case OpID::LET: [[fallthrough]];
         case OpID::MAKE_TYPE:
            expr.evalType() = evalType(+expr.rhs());
            return {};
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
            return {};
         #pragma endregion typelesses

         #pragma region indirOnly
         case OpID::PTR_MEMBER_ACCESS: [[fallthrough]];
         case OpID::CALL_INVOKE: [[fallthrough]];
         case OpID::SUBSCRIPT_INVOKE:
            if (lhsIsPtrlike) {
               expr.evalType() = evalType(+expr.lhs())->pointee();
               return {};
            }
            [[fallthrough]];
         
         #pragma endregion indirOnly

         #pragma region errs
         //dereferences
         case OpID::MEMBER_ACCESS: [[fallthrough]];
         case OpID::METHOD_PTR: [[fallthrough]];
         case OpID::ARROW_METHOD_PTR: [[fallthrough]];
         //invokes
         case OpID::LIST_INVOKE: [[fallthrough]];
         case OpID::SPECIALIZER_INVOKE: [[fallthrough]];
         case OpID::INTERP_STR_INVOKE: internalError(ErrCode::BAD_EXPR, FMT("invalid operator for types (%s)"), toString(expr.opID())); //!NOTE: should return a res
         #pragma endregion errs

         #pragma region unreachables
         case OpID::INLINE_IF: [[fallthrough]];
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
      #define __TYPELESS expr.evalType() = nullptr; return {}
      case OpID::NULL:
         debug_assert(!expr.rhs() && !expr.extra() && !expr.extra2());
         expr.evalType() = evalType(+expr.lhs());
         return {};

      case OpID::ATTRIBUTE: TODO;

      case OpID::INTERP_STR_INVOKE: TODO;
      case OpID::TERNARY_INVOKE:
         if (!(expr.evalType() = commonType(evalType(+expr.rhs()), evalType(+expr.extra())))) { //intentionally uses assignment
            return ErrCode::TYPECHECK_ERR; //!NOTE: error message: `operands of ternary expression must have a common type`
         }
         return {};


      case OpID::PREPROCESSOR: TODO;

      case OpID::MEMBER_ACCESS: [[fallthrough]];
      case OpID::PTR_MEMBER_ACCESS: [[fallthrough]];
      case OpID::METHOD_PTR: [[fallthrough]];
      case OpID::ARROW_METHOD_PTR:
         expr.evalType() = evalType(+expr.rhs());
         return {};

      #pragma region standards
      #pragma region mem
      case OpID::BIT_AND: static_assert(OpID::BIT_AND == OpID::ADDRESS_OF);
      case OpID::MUL: static_assert(OpID::MUL == OpID::DEREF);
         if (!expr.lhs() && expr.rhs()) {
            TODO;
            return {};
         }
         [[fallthrough]];
      #pragma endregion mem
      case OpID::RANGE:
      case OpID::SPREAD:

      case OpID::INC:
      case OpID::DEC:
      case OpID::BIT_NOT:

      case OpID::BIT_OR:
      case OpID::BIT_XOR:
      case OpID::SHIFT_LEFT:
      case OpID::SHIFT_RIGHT:

      case OpID::ADD:
      case OpID::SUB:
      case OpID::DIV:
      case OpID::MOD:
      case OpID::EXP:

      case OpID::THREE_WAY_COMP:

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

      //compound assignment
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

      //misc
      case OpID::COALESCE: {
         auto lhs = evalType(+expr.lhs());
         auto rhs = evalType(+expr.rhs());

         auto tmp = currScope->deduceOpOverload(expr.opID(), lhs, rhs);
         if (!tmp) {
            tmp = lhs->canonName()->deduceOpOverload(expr.opID(), lhs, rhs);
         }
         if (!tmp) {
            return {ErrCode::TYPECHECK_ERR}; //!NOTE: error message: `operator \`%s\` is not defined for types \`%s\` and \`%s\``
         }
         expr.evalType() = tmp.function->getOverload(tmp.overload).first->funcSig().retType;
         debug_assert(expr.evalType());
         return {};
      }
      #pragma endregion standards
      #pragma region invoke
      case OpID::CALL_INVOKE:
         TODO;
      case OpID::SUBSCRIPT_INVOKE: {
         auto lhs = evalType(+expr.lhs());
         auto rhs = evalType(+self[(index<ArgList>)expr.rhs()][0]);
         auto tmp = currScope->deduceOpOverload(expr.opID(), lhs, rhs);
         if (!tmp) {
            tmp = lhs->canonName()->deduceOpOverload(expr.opID(), lhs, rhs);
         }
         if (!tmp) {
            return {ErrCode::TYPECHECK_ERR}; //!NOTE: error message: `operator \`%s\` is not defined for types \`%s\` and \`%s\``
         }
         expr.evalType() = tmp.function->getOverload(tmp.overload).first->funcSig().retType;
         debug_assert(expr.evalType());
         return {};
      }
         TODO;
      #pragma endregion invoke

      #pragma region lhss
      //assignments
      case OpID::ASSIGN: [[fallthrough]];
      //case OpID::CONST_ASSIGN: TODO;
      //casts
      case OpID::CAST: [[fallthrough]];
      case OpID::UP_CAST: [[fallthrough]];
      case OpID::DYN_CAST: [[fallthrough]];
      case OpID::BIT_CAST: [[fallthrough]];
      case OpID::CONST_CAST:
         expr.evalType() = evalType(+expr.lhs());
         return {};
      #pragma endregion lhss

      #pragma region rhss
      case OpID::COMMA: [[fallthrough]];
      case OpID::LET: [[fallthrough]];
      case OpID::MAKE_TYPE:
         expr.evalType() = evalType(+expr.rhs());
         return {};
      #pragma endregion rhss


      case OpID::PREPROC_IMPORT: TODO;
      case OpID::PREPROC_LINK: TODO;
      case OpID::PREPROC_EMBED: TODO;
         
      #pragma region typelesses
      case OpID::LABEL_DELIM: [[fallthrough]];
      case OpID::FOR: [[fallthrough]];
      case OpID::FOREACH: [[fallthrough]];
      case OpID::WHILE: [[fallthrough]];
      case OpID::DO_WHILE: [[fallthrough]];

      case OpID::GOTO: [[fallthrough]];
      case OpID::GOTO_CASE: [[fallthrough]];

      case OpID::IF: [[fallthrough]];

      case OpID::SWITCH: [[fallthrough]];
      case OpID::MATCH: [[fallthrough]];

      case OpID::ASM: [[fallthrough]];

      case OpID::BREAK: [[fallthrough]];
      case OpID::CONTINUE: [[fallthrough]];

      case OpID::THROW: [[fallthrough]];
      case OpID::ASSERT: [[fallthrough]];
      case OpID::DEBUG_ASSERT: [[fallthrough]];
      case OpID::STATIC_ASSERT: [[fallthrough]];
      case OpID::ASSUME: [[fallthrough]];
      case OpID::RETURN: [[fallthrough]];

      case OpID::ALIAS:[[fallthrough]];

      case OpID::DEF_FUNC_PARAMS: [[fallthrough]];
      case OpID::DEF_MACRO_PARAMS: __TYPELESS;
      #pragma endregion typelesses

      #pragma region unreachables
      case OpID::LIST_INVOKE:
      case OpID::SPECIALIZER_INVOKE:

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

      case OpID::CHAR_INVOKE: UNREACHABLE;
      case OpID::STR_INVOKE: UNREACHABLE;

      case OpID::SCOPE_RESOLUTION: UNREACHABLE;
      
      case OpID::INLINE_IF: UNREACHABLE;
      #pragma endregion unreachables
      #undef __TYPELESS
   }
}

mcsl::tuple<bool, clef::index<void>, clef::TypeSpec*> clef::SyntaxTree::deduceOverload(SymbolNode* symbol, index<ArgList> argv) {
   //validate argv
   auto args = self[argv].span();
   for (index<Expr> arg : args) {
      if (!arg || !self[arg].evalType()) {
         TODO;
      }
   }

   //deduce overload
   debug_assert(symbol);
   debug_assert(symbol->symbolType() == SymbolType::FUNC);
   auto overloads = symbol->overloads();
   for (uint i = 0; i < overloads.size(); ++i) {
      auto [sig, def] = overloads[i];
      debug_assert(sig);
      debug_assert(sig->metaType() == TypeSpec::FUNC_SIG);
      auto& params = sig->funcSig().params;

      uint len;
      //special case for variadics
      if (params.size() && +(params.back().second & QualMask::VARIADIC)) {
         if (args.size() < params.size() - 1) {
            goto DEDUCE_CONTINUE;
         }
         len = mcsl::min(args.size(), params.size() - 1);
      }
      //check argc
      else if (args.size() != params.size()) {
         goto DEDUCE_CONTINUE;
      } else {
         len = args.size();
      }
      //check that the arguments match the parameters
      for (uint j = 0; j < len; ++j) {
         if (self[args[j]].evalType() != params[j].first) {
            goto DEDUCE_CONTINUE;
         }
         //!TODO: check implicit casts
         //!TODO: generic parameters
         //!TODO: check type qualifiers
      }
      //!TODO: check constraints for variadic parameters
      //all arguments match parameters
      return {true, i, sig};

      //emulate labeled loops
      DEDUCE_CONTINUE:
   }

   //no valid overload found
   return {false, 0, nullptr};
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

clef::res<clef::TypeSpec*> clef::SyntaxTree::commonTypeOfOperands(index<Expr> i) {
   if (!i) {
      return nullptr;
   }

   Expr& expr = self[i];

   TypeSpec* spec = nullptr;
   #define UPDATE(name) \
   if (expr.name()) { \
      spec = commonType(spec, evalType(+expr.name())); \
      if (!spec) { \
         return ErrCode::TYPECHECK_ERR; /*//!NOTE: error message: `no common type`*/ \
      } \
   }
   UPDATE(lhs);
   UPDATE(rhs);
   UPDATE(extra);
   UPDATE(extra2);
   #undef UPDATE

   return spec;
}

mcsl::str_slice clef::SyntaxTree::extractStrLit(index<Expr> strLitExpr) {
   Expr& expr = self[strLitExpr];
   if (expr.opID() != OpID::NULL || expr.lhsType() != NodeType::LITERAL) {
      TODO; //!TODO: return an error res
   }
   Literal& lit = self[(index<Literal>)expr.lhs()];
   if (lit.type() != LitType::STRING) {
      TODO; //!TODO: return an error res
   }
   return lit;
}

clef::index<clef::Expr> clef::SyntaxTree::findAttr(index<Expr> attrs, SymbolNode* name) {
   if (!attrs || !name) { return 0; }
   do {
      Expr& expr = self[attrs];
      if (expr.opID() == OpID::ATTRIBUTE && expr.lhs() && expr.lhsType() == NodeType::IDEN && self[(index<Identifier>)expr.lhs()].symbol() == name) {
         return attrs;
      }
      if (!canDownCastTo(expr.extraType2(), NodeType::EXPR)) {
         return 0;
      }
      attrs = +expr.extra2();
   } while (attrs);
   return attrs;
}

#endif //SYNTAX_TREE_CPP