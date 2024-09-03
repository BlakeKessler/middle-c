#pragma once
#ifndef MCSL_CONTIG_BASE_HPP
#define MCSL_CONTIG_BASE_HPP

#include "MCSL.hpp"
#include "alloc.hpp"

#include <typeinfo>

namespace mcsl{
   template<typename arr_t, typename T> concept contig_t = requires { std::is_base_of_v<arr_t, contig_base<T>>; };
}

#define MCSL_SELF_T template<typename Self_t> requires contig_t<Self_t,T>

//!IMPLEMENTATION GUIDE:
//!    create member variables and template params (buffer,size)
//!    write constructors
//!    implement data()
//!    implement ptr_to_buf()
//!    implement size()
//!    implement name()
template<typename T> struct mcsl::contig_base {
   static constexpr const char _name[] = "contig_base";
   static constexpr const auto& name() { return _name; }

   //properties
   MCSL_SELF_T constexpr uint size(this Self_t&& obj) { return obj.size(); }

   //element access
   MCSL_SELF_T constexpr T* const* ptr_to_buf(this Self_t&& obj)   { return obj.ptr_to_buf(); }
   MCSL_SELF_T constexpr T* data(this Self_t&& obj)   { return obj.data(); }
   MCSL_SELF_T constexpr T* begin(this Self_t&& obj)   { return obj.begin(); }

   MCSL_SELF_T inline constexpr auto end(this Self_t&& obj) -> decltype(auto)   { return obj.begin() + obj.size(); }
   MCSL_SELF_T inline constexpr auto operator[](this Self_t&& obj, const uint i) -> decltype(auto)   { return obj.begin()[i]; }
   MCSL_SELF_T inline constexpr auto at(this Self_t&& obj, const uint i) -> decltype(auto)   { if (i >= obj.size()) { mcsl_throw(ErrCode::SEGFAULT, "%s of size %u accessed at index %u", obj.name(), obj.size(), i); } return obj[i]; }
   MCSL_SELF_T inline constexpr auto front(this Self_t&& obj) -> decltype(auto)   { return obj.begin()[0]; }
   MCSL_SELF_T inline constexpr auto back(this Self_t&& obj) -> decltype(auto)   { return obj.begin()[obj.size()-1]; }

   //slicing
   inline auto slice(this const auto&& obj, const uint stop) { return obj.slice(0, stop); }
   auto slice(this const auto&& obj, const uint start, const uint stop);
   auto slice(this const auto&& obj, const uint start, const uint stop, const uint step);

   MCSL_SELF_T inline operator bool() const { return static_cast<bool>(size()); }
};

#undef MCSL_SELF_T

#endif //MCSL_CONTIG_BASE_HPP