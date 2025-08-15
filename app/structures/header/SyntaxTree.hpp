#ifndef AST_HPP
#define AST_HPP

#include "CLEF.hpp"
#include "ast-nodes/Expr.hpp"
#include "DataModel.hpp"

class clef::SyntaxTree {
   private:
      Proc _globalScope;

      buffer<Expr> _exprs;
      buffer<Args> _args;
      buffer<Proc> _procs;
      buffer<Ternary> _terns;
      buffer<If> _ifs;
      buffer<While> _whiles;
      buffer<DoWhile> _doWhiles;
      buffer<For> _fors;
      buffer<Foreach> _foreaches;
      buffer<Switch> _switches;
      buffer<Match> _matches;

      buffer<Attr> _attrs;

      buffer<Symbol> _symbols;
      buffer<TypeSpec> _types;

      DataModel _dataModel;
   public:
      SyntaxTree(DataModel);

      template<SyntaxNode_t T, typename... Argv_t> requires mcsl::valid_ctor<T, Argv_t...> T* make(Argv_t... argv) {
         #define PUSH(T2, buf) \
            if constexpr (mcsl::same_t<T, T2>) {                          \
               return buf.emplace_back(std::forward<Argv_t>(argv)...); \
            }
         PUSH(Expr   , _exprs    ) else
         PUSH(Args   , _args     ) else
         PUSH(Proc   , _procs    ) else
         PUSH(Ternary, _terns    ) else
         PUSH(If     , _ifs      ) else
         PUSH(While  , _whiles   ) else
         PUSH(DoWhile, _doWhiles ) else
         PUSH(For    , _fors     ) else
         PUSH(Foreach, _foreaches) else
         PUSH(Switch , _switches ) else
         PUSH(Match  , _matches  ) else { static_assert(false); }
         #undef PUSH
      }

      inline Proc* globalScope() { return &_globalScope; }
};

#endif