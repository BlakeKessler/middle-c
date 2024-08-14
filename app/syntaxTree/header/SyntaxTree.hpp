#pragma once
#ifndef SYNTAX_TREE_HPP
#define SYNTAX_TREE_HPP

#include "CLEF.hpp"
#include "Node.hpp"
#include "Token.hpp"
#include "array.hpp"
#include "dyn_arr.hpp"
#include "dyn_arr_span.hpp"
#include "pair.hpp"

class clef::SyntaxTree {
   private:
      NodeID_t _root;
      mcs::dyn_arr<Node> _nodes;
      mcs::dyn_arr<Token> _tokens;
      mcs::array<mcs::pair<uint>> _lineBounds;  //{index of start token, size}
   public:
      SyntaxTree():_root(NODE_NIL),_nodes(),_tokens(),_lineBounds() {}
      SyntaxTree(mcs::dyn_arr<Token>&& tokens, mcs::array<mcs::dyn_arr_span<Token>>& lines);
      ~SyntaxTree() = default;

      bool updateRoot();

      template<typename... ParamPack>
      NodeID_t emplaceNode(ParamPack... args) { return static_cast<NodeID_t>(_nodes.emplace_back(std::forward<ParamPack>(args)...) - _nodes.begin()); }
      NodeID_t pushNode(Node& node) { return (NodeID_t)_nodes.push_back(node); }

      uint size() const { return _nodes.size(); }

      astIt root();
      const astIt root() const;

      astIt begin();
      astIt end();
      const astIt begin() const;
      const astIt end() const;

      astIt it(const NodeID_t i);
      // astIt it(const uint i) { return it(static_cast<NodeID_t>(i)); }
      const astIt it(const NodeID_t i) const;
      // const astIt it(const uint i) const { return it(static_cast<NodeID_t>(i)); }
      Token& token(const TokenID_t i) { return _tokens[+i]; }
      const Token& token(const TokenID_t i) const { return _tokens[+i]; }

      void printf() const;
      #pragma region DEBUG
      void debug_printf() const;
      #pragma endregion DEBUG

      operator bool() const { return +_root; }


   friend class astIt;
};

#endif //SYNTAX_TREE_HPP