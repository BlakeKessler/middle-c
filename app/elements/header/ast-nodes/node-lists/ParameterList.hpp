#pragma once
#ifndef PARAM_LIST_HPP
#define PARAM_LIST_HPP

#include "CLEF.hpp"
#include "astNode.hpp"

#include "dyn_arr.hpp"

struct clef::ParameterList : public mcsl::contig_base<Variable*> {
   private:
      mcsl::dyn_arr<Variable*>* _params;
   public:
      static constexpr NodeType nodeType() { return NodeType::PARAM_LIST; }

      ParameterList(mcsl::dyn_arr<Variable*>& params):_params{&params} {}


      #pragma region dyn_arr
      auto size() const { return _params->size(); }
      auto capacity() const { return _params->capacity(); }
      
      auto* data() { return _params->data(); }
      auto* begin() { return _params->begin(); }
      auto* const* ptr_to_buf() { return _params->ptr_to_buf(); }

      const auto* data() const { return _params->data(); }
      const auto* begin() const { return _params->begin(); }
      const auto* const* ptr_to_buf() const { return _params->ptr_to_buf(); }


      bool resize(const uint newSize) { return _params->resize(newSize); }
      bool resize_exact(const uint newSize) { return _params->resize_exact(newSize); }
      auto* release() { return _params->release(); }
      bool push_back(Variable* obj) { return _params->push_back(obj); }
      auto pop_back() { return _params->pop_back(); }
      auto* emplace(const uint i, auto&&... args) { return _params->emplace(i, std::forward<decltype(auto)>(args...)); }
      auto* emplace_back(auto&&... args) { return _params->emplace_back(std::forward<decltype(auto)>(args...)); }
      #pragma endregion dyn_arr
};

#endif //PARAM_LIST_HPP