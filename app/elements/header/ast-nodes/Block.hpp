#pragma once
#ifndef BLOCK_HPP
#define BLOCK_HPP

#include "CLEF.hpp"
#include "Token.hpp"

#include "ast-nodes/Expression.hpp"

#include "dyn_arr.hpp"

struct clef::Block : public mcsl::contig_base<Expression*> {
   private:
      mcsl::dyn_arr<Expression*> _contents;
      Operator* _elemDelim;
      BlockType _type;
   public:
      Block():_contents{},_elemDelim{},_type{} {}
      Block(const BlockType type):_contents{},_elemDelim{},_type{type} {}
      Block(const BlockType type, Operator* delim):_contents{},_elemDelim{delim},_type{type} {}
      
      Block(OpData open);
      Block(OpData open, OpData close);
      Block(const Token& open);
      Block(const Token& open, const Token& close);

      BlockType type() const { return _type; }
      Operator* delim() { return _elemDelim; }
      const Operator* delim() const { return _elemDelim; }
      


      //!contig_base implementation
      #pragma region CONTIG_BASE
      void free() const { _contents.free(); }

      uint size() const { return _contents.size(); }
      uint capacity() const { return _contents.capacity(); }
      
      Expression** const* ptr_to_buf() { return _contents.ptr_to_buf(); }
      Expression** data() { return _contents.data(); }
      Expression** begin() { return _contents.begin(); }
      const Expression* const* const* ptr_to_buf() const { return _contents.ptr_to_buf(); }
      const Expression* const* data() const { return _contents.data(); }
      const Expression* const* begin() const { return _contents.begin(); }

      bool resize(const uint newSize) { return _contents.resize(newSize); }
      bool resize_exact(const uint newSize) { return _contents.resize_exact(newSize); }
      Expression** release() { return _contents.release(); }
      bool push_back(Expression* obj) { return _contents.push_back(obj); }
      Expression* pop_back() { return _contents.pop_back(); }
      Expression** emplace(const uint i, auto&&... args) { return _contents.emplace(i, std::forward(args...)); }
      Expression** emplace_back(auto&&... args) { return _contents.emplace_back(std::forward(args...)); }
      
      #pragma endregion CONTIG_BASE
};

#endif //BLOCK_HPP