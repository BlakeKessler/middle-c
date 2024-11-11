#pragma once
#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "CLEF.hpp"

#include "ast-nodes/Identifier.hpp"
#include "ast-nodes/Type.hpp"

#include "dyn_arr.hpp"

struct clef::Object : public mcsl::contig_base<astNode*> {
   private:
      Identifier* _name;
      Type* _type;

      mcsl::dyn_arr<astNode*> _members;
   public:
      Object():_name{},_type{},_members{} {}
      Object(Type* type):_name{},_type{type},_members{} {}
      Object(Identifier*,Type*);

      Identifier*& name() { return _name; }
      Type*& type() { return _type; }
      const Identifier* name() const { return _name; }
      const Type* type() const { return _type; }

      

      //!contig_base implementation
      #pragma region CONTIG_BASE
      void free() const { _members.free(); }

      uint size() const { return _members.size(); }
      uint capacity() const { return _members.capacity(); }
      
      astNode** const* ptr_to_buf() { return _members.ptr_to_buf(); }
      astNode** data() { return _members.data(); }
      astNode** begin() { return _members.begin(); }
      const astNode* const* const* ptr_to_buf() const { return _members.ptr_to_buf(); }
      const astNode* const* data() const { return _members.data(); }
      const astNode* const* begin() const { return _members.begin(); }

      bool resize(const uint newSize) { return _members.resize(newSize); }
      bool resize_exact(const uint newSize) { return _members.resize_exact(newSize); }
      astNode** release() { return _members.release(); }
      bool push_back(astNode* obj) { return _members.push_back(obj); }
      astNode* pop_back() { return _members.pop_back(); }
      astNode** emplace(const uint i, auto&&... args) { return _members.emplace(i, std::forward(args...)); }
      astNode** emplace_back(auto&&... args) { return _members.emplace_back(std::forward(args...)); }
      
      #pragma endregion CONTIG_BASE
};

#endif //OBJECT_HPP