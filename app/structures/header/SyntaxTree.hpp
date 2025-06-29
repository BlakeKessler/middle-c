#ifndef SYNTAX_TREE_HPP
#define SYNTAX_TREE_HPP

#include "CLEF.hpp"
#include "astNode.hpp"
#include "SymbolNode.hpp"
#include "TypeSpec.hpp"
#include "allocator.hpp"
#include "DataModel.hpp"

#include "dyn_arr.hpp"
#include "arr_list.hpp"
#include "map.hpp"

class clef::SyntaxTree {
   private:
      mcsl::dyn_arr<astNode> _buf; //owning storage for member nodes

      SymbolNode _globalScope;
      mcsl::map<KeywordID, SymbolNode*> _keywordTypes;
      mcsl::arr_list<SymbolNode> _symbolBuf;
      mcsl::arr_list<TypeSpec> _typeTable;

      allocator _alloc;

      DataModel _dataModel;

      void initTables();
   public:
      SyntaxTree():_buf{},_globalScope{},_keywordTypes{},_symbolBuf{},_typeTable{},_alloc{},_dataModel(DataModel::LP64) {
         _buf.emplace_back(NodeType::ERROR);
         initTables();
      }
      SyntaxTree(SyntaxTree&& other):
         _buf{std::move(other._buf)},
         _globalScope{std::move(other._globalScope)},
         _keywordTypes{std::move(other._keywordTypes)},
         _symbolBuf{std::move(other._symbolBuf)},
         _typeTable{std::move(other._typeTable)},
         _alloc{std::move(other._alloc)},
         _dataModel(other._dataModel) {
            if (this != &other) {
               other.release();
            }
      }
      
      inline void release();

      inline void reserve(const uint nodeCount) { _buf.reserve(nodeCount); }

#if PARALLEL_COMPILE_FILES
      void merge(SyntaxTree&& other);
#endif

      index<astNode> getValueKeyword(const KeywordID valueKeyword);

      void printf(mcsl::File&) const;
      void print(mcsl::File&) const;

      SymbolNode* findSymbol(const mcsl::str_slice name, SymbolNode* scope = {}); //return null if it isn't in the symbol table yet
      SymbolNode* registerSymbol(const mcsl::str_slice name, SymbolNode* scope = {}); //add to the table if it isn't in the symbol table yet
      SymbolNode* registerAlias(SymbolNode* alias, SymbolNode* target);
      TypeSpec* registerType(SymbolNode* name, TypeSpec::MetaType metatype, SymbolType symbolType);
      // uint freeTypesAfter(TypeSpec*); //inclusive
      // uint popNodesAfter(index<astNode>); //inclusive
      TypeSpec* makeIndirType(index<Identifier> targetNode, TypeSpec* pointee, QualMask quals, IndirTable::Entry firstEntry);

      SymbolNode* globalScope() { return &_globalScope; }
      SymbolNode* getFundType(KeywordID id) { return _keywordTypes[id]; }

      uint nodeCount() const { return _buf.size(); }
      astNode& getNode(const uint i) { assume(i); return _buf[i]; }
      const astNode& getNode(const uint i) const { assume(i); return _buf[i]; }

      template<typename T> const T& operator[](index<const T> i) const { assume(i); return *(self + i); }
      template<typename T> const T& operator[](index<T> i) const { return self[i.toConst()]; }
      template<typename T> T& operator[](index<T> i) { assume(i); return *(self + i); }

      template<astNode_t T> const T* operator+(index<const T> i) const { return (const T*)(_buf + i); }
      template<astNode_t T> T* operator+(index<T> i) { return (T*)(_buf + i); }
      const astNode* operator+(index<const astNode> i) const { return _buf + i; }
      astNode* operator+(index<astNode> i) { return _buf + i; }


      template<astNode_ptr_t asT, astNode_ptr_t T, typename... Argv_t> asT make(Argv_t... argv) requires mcsl::valid_ctor<mcsl::remove_ptr<T>, Argv_t...>;
      template<astNode_t asT, astNode_t T = asT, typename... Argv_t> index<asT> make(Argv_t... argv) requires mcsl::valid_ctor<T, Argv_t...> { index<asT> index = _buf.size(); make<asT*,T*>(std::forward<Argv_t>(argv)...); return index; }

      template<astNode_ptr_t newT, astNode_t oldT, typename... Argv_t> newT remake(index<oldT> i, Argv_t... argv) requires (mcsl::is_t<mcsl::remove_ptr<newT>, mcsl::remove_ptr<oldT>> || mcsl::valid_ctor<mcsl::remove_ptr<newT>, mcsl::remove_ptr<oldT>>) && mcsl::valid_ctor<mcsl::remove_ptr<newT>, Argv_t...>;
      template<astNode_t newT, astNode_t oldT, typename... Argv_t> index<newT> remake(index<oldT> i, Argv_t... argv) requires (mcsl::is_t<newT, oldT> || mcsl::valid_ctor<newT, oldT>) && mcsl::valid_ctor<newT, Argv_t...> { remake<newT*>(i, std::forward<Argv_t>(argv)...); return +i; }
      
      index<Expr> makeExpr(const OpID, index<astNode>);
      index<Expr> makeExpr(const OpID, index<astNode>, index<astNode>);

      index<Expr> remakeTernary(index<astNode> cond, index<Expr> vals);

      template<typename T> mcsl::dyn_arr<T>& allocBuf() { return _alloc.at(_alloc.alloc<T>()); }
      template<typename T> void freeBuf(mcsl::dyn_arr<T>& buf) { _alloc.freeBuf(buf); }



   private:
      struct __MangleData {
         mcsl::dyn_arr<SymbolNode*> substitutions;
         uint charsPrinted;

         sint subIndex(SymbolNode*) { TODO; }
      };
      static void __mangleSpecializerImpl(mcsl::File& file, SyntaxTree& tree, Identifier& name, __MangleData& data) { TODO; }
      static void __mangleImpl(mcsl::File& file, SyntaxTree& tree, Identifier& name, __MangleData& data);
   public:
      static uint manglePrint(mcsl::File& file, SyntaxTree& tree, index<Identifier> name) { TODO; }
};

//!quick little struct to indent when printing newlines in a `printf` call
struct clef::indenter {
   static constexpr uint indentSize = 3;
   
   uint indents;

   indenter(uint i): indents{i} {}

   operator uint() const { return indents; }
   operator uint&() { return indents; }
};

//!astTreeNodeBundle - for printf
template<typename T> struct clef::astTNB {
   const SyntaxTree& tree;
   index<const T> i;
   indenter indents = 0;

   astTNB(const SyntaxTree& t, uint n, indenter ind = 0):tree{t}, i{n}, indents{ind} {}
   astTNB(const SyntaxTree& t, index<const T> n, indenter ind = 0):tree{t}, i{n}, indents{ind} {}
   astTNB(const SyntaxTree& t, index<T> n, indenter ind = 0):tree{t}, i{n}, indents{ind} {}

   const T& get() const { return tree[i]; }
   const T& operator*() const { return tree[i]; }
   const T* operator->() const { return tree + i; }

   operator bool() const { return i; }
};
//!astTreeTypespecBundle - for printf
struct clef::astTTsB {
   const SyntaxTree& tree;
   const TypeSpec* ptr;
   indenter indents = 0;

   astTTsB(const SyntaxTree& t, const TypeSpec* p, indenter ind = 0):tree{t}, ptr{p}, indents{ind} {}

   const TypeSpec& get() const { return *ptr; }
   const TypeSpec& operator*() const { return *ptr; }
   const TypeSpec* operator->() const { return ptr; }

   operator bool() const { return ptr; }
};
//!astTreeSymbolBundle - for printf
struct clef::astTSB {
   const SyntaxTree& tree;
   const SymbolNode* ptr;
   indenter indents = 0;

   astTSB(const SyntaxTree& t, const SymbolNode* p, indenter ind = 0):tree{t}, ptr{p}, indents{ind} {}

   const SymbolNode& get() const { return *ptr; }
   const SymbolNode& operator*() const { return *ptr; }
   const SymbolNode* operator->() const { return ptr; }

   operator bool() const { return ptr; }
};

#include "MAP_MACRO.h"
#define __DEF_TNB_WRITEF(T) uint writef(File& file, const clef::astTNB<clef::T> obj, char mode, FmtArgs args);

namespace mcsl {
   uint writef(File& file, const clef::indenter i, char mode, FmtArgs args);

   uint writef(File& file, const clef::SyntaxTree& tree, char mode, FmtArgs args);
   uint writef(File& file, const clef::astTNB<clef::astNode> obj, char mode, FmtArgs args);
   MCSL_MAP(__DEF_TNB_WRITEF, CLEF_ALL_AST_NODE_T)
   uint writef(File& file, const clef::astTTsB obj, char mode, FmtArgs args);
   uint writef(File& file, const clef::astTSB obj, char mode, FmtArgs args);
   uint writef(File& file, const clef::IndirTable& obj, char mode, FmtArgs args);
   uint writef(File& file, clef::QualMask, char mode, FmtArgs args);

   inline File& write(File& file, const clef::SyntaxTree& obj) { writef(file, obj, 's', {}); return file; }
};

#undef __DEF_TNB_WRITEF
#include "MAP_MACRO_UNDEF.h"

#pragma region inlinesrc

inline void clef::SyntaxTree::release() {
   _buf.release();
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