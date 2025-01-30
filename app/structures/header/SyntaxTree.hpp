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

      FundType* getFundType(const KeywordID fundTypeKeyword);
      astNode* getValueKeyword(const KeywordID valueKeyword);

      void printf() const;
      void print() const;

      template<typename T> const T& operator[](index<const T> i) { safe_mode_assert(i); return *(self + i); }
      template<typename T> T& operator[](index<T> i) { safe_mode_assert(i); return *(self + i); }

      template<astNode_t T> const T* operator+(index<const T> i) { return _buf + i; }
      template<astNode_t T> T* operator+(index<T> i) { return _buf + i; }

      const InterfaceSpec* operator+(index<const InterfaceSpec> i) { return _ifaceSpecBuf + i; }
      const NamespaceSpec* operator+(index<const NamespaceSpec> i) { return _nsSpecBuf + i; }
      const ObjTypeSpec* operator+(index<const ObjTypeSpec> i) { return _objSpecBuf + i; }
      InterfaceSpec* operator+(index<InterfaceSpec> i) { return _ifaceSpecBuf + i; }
      NamespaceSpec* operator+(index<NamespaceSpec> i) { return _nsSpecBuf + i; }
      ObjTypeSpec* operator+(index<ObjTypeSpec> i) { return _objSpecBuf + i; }



      astNode* allocNode(const NodeType type) { return _buf.emplace_back(type); }
      template<typename T> mcsl::dyn_arr<T>& allocBuf() { return _alloc.at(_alloc.alloc<T>()); }
      InterfaceSpec* allocInterfaceSpec() { return _ifaceSpecBuf.emplace_back(); }
      NamespaceSpec* allocNamespaceSpec() { return _nsSpecBuf.emplace_back(); }
      ObjTypeSpec* allocObjTypeSpec() { return _objSpecBuf.emplace_back(); }
};

#endif //SYNTAX_TREE_HPP