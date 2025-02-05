#ifndef SYNTAX_TREE_HPP
#define SYNTAX_TREE_HPP

#include "CLEF.hpp"
#include "SymbolTable.hpp"
#include "astNode.hpp"
#include "allocator.hpp"

#include "dyn_arr.hpp"

class clef::SyntaxTree {
   private:
      // SymbolTable _names;

      mcsl::dyn_arr<astNode> _buf; //owning storage for member nodes
      mcsl::dyn_arr<InterfaceSpec> _ifaceSpecBuf;
      mcsl::dyn_arr<NamespaceSpec> _nsSpecBuf;
      mcsl::dyn_arr<ObjTypeSpec> _objSpecBuf;

      allocator _alloc;
   public:
      SyntaxTree():/*_names{},*/_buf{astNode{NodeType::ERROR}/*make null indexes act like null pointers*/},_alloc{} {}
      SyntaxTree(const SyntaxTree& other):/*_names{other._names},*/_buf{other._buf},_ifaceSpecBuf{other._ifaceSpecBuf},_alloc{other._alloc} {}
      SyntaxTree(SyntaxTree&& other);

      astNode& getNode(const uint i) { return _buf[i]; }
      const astNode& getNode(const uint i) const { return _buf[i]; }

      index<FundType> getFundType(const KeywordID fundTypeKeyword);
      index<astNode> getValueKeyword(const KeywordID valueKeyword);
      
      index<Expr> makeExpr(const OpID, index<astNode>, index<astNode>);
      template<astNode_t T> index<Expr> makeExpr(const OpID, index<T>, index<astNode>);
      index<Expr> makeExpr(const OpID, index<astNode>);
      template<astNode_t T> index<Expr> makeExpr(const OpID, index<T>);
      template<astNode_t T> index<Expr> makeExpr(const OpID, index<astNode>, index<T>);
      template<astNode_t T1, astNode_t T2> index<Expr> makeExpr(const OpID op, index<T1> lhs, index<T2> rhs) { return +make<Expr>(op, lhs, rhs); }

      void printf() const;
      void print() const;

      astNode& getNode(const uint i) { safe_mode_assert(i); return _buf[i]; }
      const astNode& getNode(const uint i) const { safe_mode_assert(i); return _buf[i]; }

      template<typename T> const T& operator[](index<const T> i) { safe_mode_assert(i); return *(self + i); }
      template<typename T> T& operator[](index<T> i) { safe_mode_assert(i); return *(self + i); }

      template<astNode_t T> const T* operator+(index<const T> i) { return _buf + i; }
      template<astNode_t T> T* operator+(index<T> i) { return _buf + i; }
      const astNode* operator+(index<const astNode> i) { return _buf + i; }
      astNode* operator+(index<astNode> i) { return _buf + i; }

      const InterfaceSpec* operator+(index<const InterfaceSpec> i) { return _ifaceSpecBuf + i; }
      const NamespaceSpec* operator+(index<const NamespaceSpec> i) { return _nsSpecBuf + i; }
      const ObjTypeSpec* operator+(index<const ObjTypeSpec> i) { return _objSpecBuf + i; }
      InterfaceSpec* operator+(index<InterfaceSpec> i) { return _ifaceSpecBuf + i; }
      NamespaceSpec* operator+(index<NamespaceSpec> i) { return _nsSpecBuf + i; }
      ObjTypeSpec* operator+(index<ObjTypeSpec> i) { return _objSpecBuf + i; }


      template<astNode_ptr_t asT, astNode_ptr_t T = asT, typename... Argv_t> asT make(Argv_t... argv) requires mcsl::valid_ctor<mcsl::remove_ptr<T>, Argv_t...>;
      template<astNode_t asT, astNode_t T = asT, typename... Argv_t> index<asT> make(Argv_t... argv) requires mcsl::valid_ctor<T, Argv_t...> { return (index<asT>)(make<T>(std::forward<Argv_t>(argv)...) - _buf.begin()); }

      template<astNode_ptr_t newT, astNode_ptr_t oldT, typename... Argv_t> newT remake(index<oldT> i, Argv_t... argv) requires mcsl::is_t<mcsl::remove_ptr<newT>, mcsl::remove_ptr<oldT>> && mcsl::valid_ctor<mcsl::remove_ptr<newT>, Argv_t...>;
      template<astNode_t newT, astNode_t oldT, typename... Argv_t> index<newT> remake(index<oldT> i, Argv_t... argv) requires mcsl::is_t<newT, oldT> && mcsl::valid_ctor<newT, Argv_t...> { return (index<asT>)(remake<T>(i, std::forward<Argv_t>(argv)...) - _buf.begin()); }



      template<typename T> mcsl::dyn_arr<T>& allocBuf() { return _alloc.at(_alloc.alloc<T>()); }
      index<InterfaceSpec> allocInterfaceSpec() { return _ifaceSpecBuf.emplace_back() - _ifaceSpecBuf.begin(); }
      index<NamespaceSpec> allocNamespaceSpec() { return _nsSpecBuf.emplace_back() - _nsSpecBuf.begin(); }
      index<ObjTypeSpec> allocObjTypeSpec() { return _objSpecBuf.emplace_back() - _objSpecBuf.begin(); }
};

#pragma region inlinesrc

template<clef::astNode_ptr_t asT, clef::astNode_ptr_t T = asT, typename... Argv_t> asT clef::SyntaxTree::make(Argv_t... argv) requires mcsl::valid_ctor<mcsl::remove_ptr<T>, Argv_t...> {
   astNode* tmp = _buf.emplace_back(mcsl::remove_ptr<T>{std::forward<Argv_t>(argv)...});
   if constexpr (!mcsl::is_t<mcsl::remove_ptr<asT>, mcsl::remove_ptr<T>>) {
      tmp->anyCast(mcsl::remove_ptr<asT>::nodeType());
   }
   return (asT)tmp;
}

template<clef::astNode_ptr_t newT, clef::astNode_ptr_t oldT, typename... Argv_t> newT clef::SyntaxTree::remake(index<oldT> i, Argv_t... argv) requires mcsl::is_t<mcsl::remove_ptr<newT>, mcsl::remove_ptr<oldT>> && mcsl::valid_ctor<mcsl::remove_ptr<newT>, Argv_t...> {
   debug_assert(i);
   astNode* tmp = _buf.emplace(i, mcsl::remove_ptr<newT>{std::forward<Argv_t>(argv)...});
   if constexpr (!mcsl::same_t<mcsl::remove_ptr<oldT>, mcsl::remove_ptr<newT>>) {
      tmp->anyCast(mcsl::remove_ptr<newT>::nodeType());
   }
   return (newT)tmp;
}

#pragma endregion inlinesrc

#endif //SYNTAX_TREE_HPP