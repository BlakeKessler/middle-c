#pragma once
#ifndef MCSL_CONTIG_BASE_HPP
#define MCSL_CONTIG_BASE_HPP

#include "MCSL.hpp"
#include "alloc.hpp"

#include <typeinfo>

namespace mcsl{
   template<typename arr_t, typename T> concept contig_t = requires { std::is_base_of_v<arr_t, contig_base<T>>; };
}


//!IMPLEMENTATION GUIDE:
//!    create member variables and template params (buffer,size)
//!    write constructors
//!    implement data()
//!    implement ptr_to_buf()
//!    implement size()
//!    implement name()
template<typename T> struct mcsl::contig_base {
   static constexpr const char temp[] = "temp";
   //properties
   template<typename Self_t> requires contig_t<Self_t,T> constexpr uint size(this Self_t&& obj) { return obj.size(); }

   //element access
   template<typename Self_t> requires contig_t<Self_t,T> constexpr T* const* ptr_to_buf(this Self_t&& obj)   { return obj.ptr_to_buf(); }
   template<typename Self_t> requires contig_t<Self_t,T> constexpr T* data(this Self_t&& obj)   { return obj.data(); }
   template<typename Self_t> requires contig_t<Self_t,T> constexpr const char* name(this Self_t&&) { return temp; }
   // template<contig_t Self_t,typename char_t = char> constexpr const str_base<char_t>& name(this Self_t&&) { return Self_t::_name; }
   
   template<typename Self_t> requires contig_t<Self_t,T> inline constexpr auto begin(this Self_t&& obj) -> decltype(auto)   { return obj.data(); }
   template<typename Self_t> requires contig_t<Self_t,T> inline constexpr auto end(this Self_t&& obj) -> decltype(auto)   { return obj.data() + obj.size(); }
   template<typename Self_t> requires contig_t<Self_t,T> inline constexpr auto operator[](this Self_t&& obj, const uint i) -> decltype(auto)   { return obj.data()[i]; }
   template<typename Self_t> requires contig_t<Self_t,T> inline constexpr auto at(this Self_t&& obj, const uint i) -> decltype(auto)   { if (i >= obj.size()) { mcsl_throw(ErrCode::SEGFAULT, "%s of size %u accessed at index %u", obj.name(), obj.size(), i); } return obj[i]; }
   template<typename Self_t> requires contig_t<Self_t,T> inline constexpr auto front(this Self_t&& obj) -> decltype(auto)   { return obj.data()[0]; }
   template<typename Self_t> requires contig_t<Self_t,T> inline constexpr auto back(this Self_t&& obj) -> decltype(auto)   { return obj.data()[obj.size()-1]; }

   //slicing
   inline auto slice(this const auto&& obj, const uint stop) { return obj.slice(0, stop); }
   auto slice(this const auto&& obj, const uint start, const uint stop);
   auto slice(this const auto&& obj, const uint start, const uint stop, const uint step);

   template<typename Self_t> requires contig_t<Self_t,T> inline operator bool() const { return static_cast<bool>(size()); }
};

#endif //MCSL_CONTIG_BASE_HPP