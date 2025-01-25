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
      mcsl::dyn_arr<InterfaceSpec> _intSpecBuf;

      astNode* _root;
      astNode* _back;

      allocator _alloc;
   public:
      SyntaxTree():/*_names{},*/_buf{},_root{},_back{},_alloc{} {}
      SyntaxTree(const SyntaxTree& other):/*_names{other._names},*/_buf{other._buf},_intSpecBuf{other._intSpecBuf},_root{other._root},_back{other._back},_alloc{other._alloc} {}
      SyntaxTree(SyntaxTree&& other);

      astNode* root() { return _root; }
      const astNode* root() const { return _root; }

      constexpr FundType* getFundType(const KeywordID fundTypeKeyword);
      constexpr Expr* getValueKeyword(const KeywordID valueKeyword);

      void printf() const;
      void print() const;



      astNode* allocNode(const NodeType type) { _buf.emplace_back(type); return &_buf.back(); }
      template<typename T> mcsl::dyn_arr<T>& allocBuf() { return _alloc.at(_alloc.alloc<T>()); }
      InterfaceSpec* allocInterfaceSpec() { _intSpecBuf.emplace_back(); return &_intSpecBuf.back(); }
};

#endif //SYNTAX_TREE_HPP