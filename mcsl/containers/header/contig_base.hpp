#pragma once
#ifndef MCSL_CONTIG_BASE_HPP
#define MCSL_CONTIG_BASE_HPP

#include "MCSL.hpp"
#include "alloc.hpp"
#include "raw_str.hpp"

#define THIS static_cast<arr_t*>(this)
#define const_THIS static_cast<const arr_t*>(this)
#define SELF (*static_cast<arr_t*>(this))
#define const_SELF (*static_cast<const arr_t*>(this))

//!IMPLEMENTATION GUIDE:
//!    create member variables and template params (buffer,size)
//!    write constructors
//!    implement data()
//!    implement ptr_to_buf()
//!    implement size()
//!    implement name()
template<typename T, typename arr_t> struct mcsl::contig_base {
   //properties
   constexpr uint size() const { return const_SELF.size(); }

   //element access
          constexpr T* const* ptr_to_buf()   { return SELF.ptr_to_buf(); }
          constexpr T* data()   { return SELF.data(); }
   inline constexpr T* begin()   { return data(); }
   inline constexpr T* end()   { return data() + size(); }
   inline constexpr T& operator[](const uint i)   { return data()[i]; }
   inline constexpr T& at(const uint i)   { if (i >= size()) { mcsl_throw(ErrCode::SEGFAULT, "%s of size %u accessed at index %u", SELF.name(), size(), i); } return self[i]; }
   inline constexpr T& front()   { return data()[0]; }
   inline constexpr T& back()   { return data()[size()-1]; }

          constexpr const T* const* ptr_to_buf() const   { return const_SELF.ptr_to_buf(); }
          constexpr const T* data() const   { return const_SELF.data(); }
   inline constexpr const T* begin() const   { return data(); }
   inline constexpr const T* end() const   { return data() + size(); }
   inline constexpr const T& operator[](const uint i) const   { return data()[i]; }
   inline constexpr const T& at(const uint i) const   { if (i >= size()) { mcsl_throw(ErrCode::SEGFAULT, "%s of size %u accessed at index %u", const_SELF.name(), size(), i); } return self[i]; }
   inline constexpr const T& front() const   { return data()[0]; }
   inline constexpr const T& back() const   { return data()[size()-1]; }

   //slicing
   inline arr_t slice(const uint stop) const { return slice(0, stop); }
   arr_t slice(const uint start, const uint stop) const;
   arr_t slice(const uint start, const uint stop, const uint step) const;

   inline operator bool() { return static_cast<bool>(size()); }
};

#undef THIS
#undef const_THIS
#undef SELF
#undef const_SELF

#endif //MCSL_CONTIG_BASE_HPP