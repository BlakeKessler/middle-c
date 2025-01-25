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

      astNode* _root;
      astNode* _back;

      allocator _alloc;
   public:
      SyntaxTree():/*_names{},*/_buf{},_root{},_back{},_alloc{} {}
      SyntaxTree(const SyntaxTree& other):/*_names{other._names},*/_buf{other._buf},_ifaceSpecBuf{other._ifaceSpecBuf},_root{other._root},_back{other._back},_alloc{other._alloc} {}
      SyntaxTree(SyntaxTree&& other);

      astNode& getNode(const uint i) { return _buf[i]; }
      const astNode& getNode(const uint i) const { return _buf[i]; }

      astNode* root() { return _root; }
      const astNode* root() const { return _root; }

      FundType* getFundType(const KeywordID fundTypeKeyword);
      astNode* getValueKeyword(const KeywordID valueKeyword);

      void printf() const;
      void print() const;



      astNode* allocNode(const NodeType type) { _buf.emplace_back(type); return &_buf.back(); }
      template<typename T> mcsl::dyn_arr<T>& allocBuf() { return _alloc.at(_alloc.alloc<T>()); }
      InterfaceSpec* allocInterfaceSpec() { return _ifaceSpecBuf.emplace_back(); }
      NamespaceSpec* allocNamespaceSpec() { return _nsSpecBuf.emplace_back(); }
      ObjTypeSpec* allocObjTypeSpec() { return _objSpecBuf.emplace_back(); }
};

#endif //SYNTAX_TREE_HPP