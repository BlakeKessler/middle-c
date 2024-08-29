#pragma once
#ifndef AST_NODE_HPP
#define AST_NODE_HPP

#include "CLEF.hpp"
#include "TypeSafetyEnums.hpp"
#include "static_arr.hpp"

struct clef::Node {
   //next and prev
   NodeID_t nextID;
   NodeID_t prevID;

   //properties
   TokenID_t tokenID;
   byte indexInParent;        //address in parent's children array
   NodeType type;
   unsigned short status;     //info interpreted differently by type


   //parent and children
   NodeID_t parentID;
   mcsl::static_arr<NodeID_t,MAX_AST_CHILDREN> childIDs;


   //constructors
   constexpr Node() = default;
   Node(const TokenID_t tok, const NodeType t = NodeType::NONE);
   Node(const NodeID_t previous, const NodeType type = NodeType::NONE, const TokenID_t tok = TOK_NIL);
   Node(const NodeID_t next, const NodeID_t previous, const NodeType type = NodeType::NONE, const TokenID_t tok = TOK_NIL);
   Node(NodeID_t,NodeID_t,TokenID_t,byte,NodeType,unsigned short,NodeID_t,mcsl::static_arr<NodeID_t,MAX_AST_CHILDREN>);
   // Node(const Node& other) = default;

   inline bool is(const NodeType t) const { return type == t; }
};

#endif //AST_NODE_HPP