#pragma once
#ifndef PARAM_LIST_HPP
#define PARAM_LIST_HPP

#include "CLEF.hpp"

#include "dyn_arr.hpp"

struct clef::ParameterList : public mcsl::contig_base<index<Variable>> { //!TODO: TypeDecl instead of Variable?
   private:
      mcsl::dyn_arr<index<Variable>>* _params;
   public:
      static constexpr NodeType nodeType() { return NodeType::PARAM_LIST; }

      ParameterList(mcsl::dyn_arr<index<Variable>>* params):_params{params} {}


      #pragma region dyn_arr
      auto size() const { return _params->size(); }
      auto capacity() const { return _params->capacity(); }
      
      auto* data() { return _params->data(); }
      auto* begin() { return _params->begin(); }
      auto* const* ptr_to_buf() { return _params->ptr_to_buf(); }

      const auto* data() const { return _params->data(); }
      const auto* begin() const { return _params->begin(); }
      const auto* const* ptr_to_buf() const { return _params->ptr_to_buf(); }


      bool reserve(const uint newSize) { return _params->reserve(newSize); }
      bool reserve_exact(const uint newSize) { return _params->reserve_exact(newSize); }
      auto* release() { return _params->release(); }
      bool push_back(index<Variable> obj) { return _params->push_back(obj); }
      auto pop_back() { return _params->pop_back(); }
      auto* emplace(const uint i, auto&&... args) requires mcsl::valid_ctor<Variable, decltype(args)...> { return _params->emplace(i, std::forward<decltype(args)>(args)...); }
      auto* emplace_back(auto&&... args) requires mcsl::valid_ctor<Variable, decltype(args)...> { return _params->emplace_back(std::forward<decltype(args)>(args)...); }
      #pragma endregion dyn_arr
};

namespace mcsl {
   File& write(File&, const clef::ParamList&);
};

#endif //PARAM_LIST_HPP