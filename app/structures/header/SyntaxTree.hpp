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

      void __indent(mcsl::File&, uint indents) const;
      void __printf(mcsl::File&, index<const astNode>, const uint indents) const;
      template<astNode_t T> void __printf(mcsl::File&, index<const T>, const uint indents) const;
      void __printf(mcsl::File&, index<const InterfaceSpec>, const uint indents) const;
      void __printf(mcsl::File&, index<const NamespaceSpec>, const uint indents) const;
      void __printf(mcsl::File&, index<const ObjTypeSpec>, const uint indents) const;
   public:
      SyntaxTree():_buf{},_ifaceSpecBuf{},_nsSpecBuf{},_objSpecBuf{},_alloc{} {
         _buf.emplace_back(NodeType::ERROR);
         _ifaceSpecBuf.emplace_back();
         _nsSpecBuf.emplace_back();
         _objSpecBuf.emplace_back();
      }
      // SyntaxTree(const SyntaxTree& other):_buf{other._buf},_ifaceSpecBuf{other._ifaceSpecBuf},_nsSpecBuf{},_objSpecBuf{},_alloc{other._alloc} {}
      SyntaxTree(SyntaxTree&& other):
         _buf{std::move(other._buf)},
         _ifaceSpecBuf{std::move(other._ifaceSpecBuf)},
         _nsSpecBuf{std::move(other._nsSpecBuf)},
         _objSpecBuf{std::move(other._objSpecBuf)},
         _alloc{std::move(other._alloc)} {
            if (this != &other) {
               other.release();
            }
      }
      
      inline void release();

      inline void reserve(const uint nodeCount) { _buf.reserve(nodeCount); }

      index<FundType> getFundType(const KeywordID fundTypeKeyword);
      index<astNode> getValueKeyword(const KeywordID valueKeyword);

      void printf(mcsl::File&) const;
      void print(mcsl::File&) const;

      astNode& getNode(const uint i) { assume(i); return _buf[i]; }
      const astNode& getNode(const uint i) const { assume(i); return _buf[i]; }

      template<typename T> const T& operator[](index<const T> i) const { assume(i); return *(self + i); }
      template<typename T> T& operator[](index<T> i) { assume(i); return *(self + i); }

      template<astNode_t T> const T* operator+(index<const T> i) const { return (const T*)(_buf + i); }
      template<astNode_t T> T* operator+(index<T> i) { return (T*)(_buf + i); }
      const astNode* operator+(index<const astNode> i) const { return _buf + i; }
      astNode* operator+(index<astNode> i) { return _buf + i; }

      const InterfaceSpec* operator+(index<const InterfaceSpec> i) const { return _ifaceSpecBuf + i; }
      const NamespaceSpec* operator+(index<const NamespaceSpec> i) const { return _nsSpecBuf + i; }
      const ObjTypeSpec* operator+(index<const ObjTypeSpec> i) const { return _objSpecBuf + i; }
      InterfaceSpec* operator+(index<InterfaceSpec> i) { return _ifaceSpecBuf + i; }
      NamespaceSpec* operator+(index<NamespaceSpec> i) { return _nsSpecBuf + i; }
      ObjTypeSpec* operator+(index<ObjTypeSpec> i) { return _objSpecBuf + i; }


      template<astNode_ptr_t asT, astNode_ptr_t T, typename... Argv_t> asT make(Argv_t... argv) requires mcsl::valid_ctor<mcsl::remove_ptr<T>, Argv_t...>;
      template<astNode_t asT, astNode_t T = asT, typename... Argv_t> index<asT> make(Argv_t... argv) requires mcsl::valid_ctor<T, Argv_t...> { index<asT> index = _buf.size(); make<asT*,T*>(std::forward<Argv_t>(argv)...); return index; }

      template<astNode_ptr_t newT, astNode_t oldT, typename... Argv_t> newT remake(index<oldT> i, Argv_t... argv) requires (mcsl::is_t<mcsl::remove_ptr<newT>, mcsl::remove_ptr<oldT>> || mcsl::valid_ctor<mcsl::remove_ptr<newT>, mcsl::remove_ptr<oldT>>) && mcsl::valid_ctor<mcsl::remove_ptr<newT>, Argv_t...>;
      template<astNode_t newT, astNode_t oldT, typename... Argv_t> index<newT> remake(index<oldT> i, Argv_t... argv) requires (mcsl::is_t<newT, oldT> || mcsl::valid_ctor<newT, oldT>) && mcsl::valid_ctor<newT, Argv_t...> { remake<newT*>(i, std::forward<Argv_t>(argv)...); return +i; }
      
      index<Expr> makeExpr(const OpID, index<astNode>);
      index<Expr> makeExpr(const OpID, index<astNode>, index<astNode>);


      template<typename T> mcsl::dyn_arr<T>& allocBuf() { return _alloc.at(_alloc.alloc<T>()); }
      index<InterfaceSpec> allocInterfaceSpec() { return _ifaceSpecBuf.emplace_back() - _ifaceSpecBuf.begin(); }
      index<NamespaceSpec> allocNamespaceSpec() { return _nsSpecBuf.emplace_back() - _nsSpecBuf.begin(); }
      index<ObjTypeSpec> allocObjTypeSpec() { return _objSpecBuf.emplace_back() - _objSpecBuf.begin(); }
};

//!astTreeNodeBundle - for printf
template<typename T> struct clef::astTNB {
   const SyntaxTree& tree;
   const index<const T> i;
};

#include "MAP_MACRO.h"
#define __DEF_TNB_WRITEF(T) uint writef(File& file, const clef::astTNB<clef::T> obj, char mode, FmtArgs args);

namespace mcsl {
   inline File& write(File& file, const clef::SyntaxTree& obj) { obj.printf(file); return file; }

   uint writef(File& file, const clef::SyntaxTree& tree, char mode, FmtArgs args);
   
   MCSL_MAP(__DEF_TNB_WRITEF, CLEF_ALL_AST_NODE_T)
};

#undef __DEF_TNB_WRITEF
#include "MAP_MACRO_UNDEF.h"

#pragma region inlinesrc

inline void clef::SyntaxTree::release() {
   _buf.release();
   _ifaceSpecBuf.release();
   _nsSpecBuf.release();
   _objSpecBuf.release();
   _alloc.release();
}

template<clef::astNode_ptr_t asT, clef::astNode_ptr_t T = asT, typename... Argv_t> asT clef::SyntaxTree::make(Argv_t... argv) requires mcsl::valid_ctor<mcsl::remove_ptr<T>, Argv_t...> {
   astNode* tmp = _buf.emplace_back(std::move(mcsl::remove_ptr<T>{std::forward<Argv_t>(argv)...}));
   if constexpr (!mcsl::is_t<mcsl::remove_ptr<asT>, mcsl::remove_ptr<T>>) {
      tmp->anyCast(mcsl::remove_ptr<asT>::nodeType());
   }
   return (asT)tmp;
}

template<clef::astNode_ptr_t newT, clef::astNode_t oldT, typename... Argv_t> newT clef::SyntaxTree::remake(index<oldT> i, Argv_t... argv) requires (mcsl::is_t<mcsl::remove_ptr<newT>, mcsl::remove_ptr<oldT>> || mcsl::valid_ctor<mcsl::remove_ptr<newT>,mcsl::remove_ptr<oldT>>) && mcsl::valid_ctor<mcsl::remove_ptr<newT>, Argv_t...> {
   assume(i);
   astNode* tmp = _buf.emplace(i, std::move(mcsl::remove_ptr<newT>{std::forward<Argv_t>(argv)...}));
   if constexpr (!mcsl::same_t<mcsl::remove_ptr<oldT>, mcsl::remove_ptr<newT>>) {
      tmp->anyCast(mcsl::remove_ptr<newT>::nodeType());
   }
   return (newT)tmp;
}

#pragma endregion inlinesrc

#endif //SYNTAX_TREE_HPP