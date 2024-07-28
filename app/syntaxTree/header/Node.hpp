#pragma once
#ifndef AST_NODE_HPP
#define AST_NODE_HPP

#include "CLEF.hpp"
#include "Token.hpp"
#include "dyn_arr.hpp"

class clef::Node {
   private:
      Token* _token;

      Node* _next;
      Node* _prev;

      Node* _children[MAX_AST_CHILDREN];
      Node* _parent;
      uint _indexInParent;

      NodeType _type;
      unsigned short _status;     //info interpreted differently by type
   public:
      //constructors
      #pragma region constructors
      Node();
      Node(Token* tok, const NodeType type = NodeType::NONE);
      Node(Node* previous, const NodeType type = NodeType::NONE, Token* tok = nullptr);
      Node(const Node& other) = default;
      #pragma endregion constructors

      //typed makers
      static Node* makeBlock(Node* open, Node* close, const DelimPairType type);
      static Node* makeBlock(Node* open, Node* close);
      static Node* makeStatement(Node* firstTok, Node* eosTok);

      //getters
      #pragma region get
      inline Node* next() const {return _next;}
      inline Node* prev() const {return _prev;}
      Node* front() const;
      Node* back() const;
      Node* child(const uint i) const;
      inline Node* parent() const {return _parent;}
      inline Token* token() const {return _token;}
      inline NodeType type() const {return _type;}
      inline uint status() const {return _status;}

      mcs::dyn_arr<Node*> extractByType(const NodeType type) const;
      #pragma endregion get

      //evaluators
      #pragma region eval
      inline bool isKeyword() const { return _token ? _token->isKeyword() : false; }
      inline DelimPairType blockDelimEval() const { return _token ? _token->blockDelimEval() : DelimPairType::NONE; }
      bool checkDelimMatch(Node* close) const;
      bool isOpening() const { return _token ? _token->blockDelimEval() != DelimPairType::NONE : false; }
      #pragma endregion eval

      //setters
      #pragma region set
      Node* setNext(Node* newNext);
      Node* setPrev(Node* newPrev);
      inline Node* unlinkNext();
      inline Node* unlinkPrev();
      Node* setChild(const uint i, Node* newChild);
      Token* setToken(Token* newToken);
      NodeType setType(const NodeType newType);
      uint setStatus(const uint newStatus);

      static Node* removeSegment(Node* open, Node* close);
      #pragma endregion set

      //push/pop
      Node* insertAfter(Node* newNode);
      Node* insertBefore(Node* newNode);
      Node* pop();

      //search
      Node* findAfter(const char* tokStr) const;

      //IO
      void printf(const uint indents) const;
      void throwError(const ErrCode code) const;
};

#endif //AST_NODE_HPP