#pragma once
#ifndef MCSL_SET_HPP
#define MCSL_SET_HPP

#include "MCSL.hpp"
#include "concepts.hpp"

#include <bit>

template<typename T, typename eq, typename hash>
requires mcsl::hash_t<T,hash> && mcsl::cmp_t<T,eq>
class mcsl::set {
   private:
      T* _buf;
      bool* _optionals;
      uint _buckets;
      uint _bucketSize;
      uint _size;
   public:
      constexpr set(): _buf(nullptr),_buckets(0),_size(0) {}
      set(const uint buckets): _buf(mcsl::calloc<T>(buckets)),_buckets(buckets),_size(0) {}
      set(std::convertible_to<T>... elements);
      set(map&& other);
      set(set&& other);
      template<typename other_t> requires (std::is_base_of_v<container_base,other_t>)
         set(other_t&& other);
      
      ~set() { self.free(); }
      void free() const;
      T* release();


      template<typename obj_t> requires hash_t<hash,obj_t> bool contains(const obj_t& other) const;
      T* insert(const T&);
      T* emplace(auto... args) const;
      template<typename obj_t> requires hash_t<hash,obj_t> void remove();

      T* begin();
      T* end();
      const T* begin() const;
      const T* end() const;

      T** ptr_to_buf();
      T* data();
      const T** ptr_to_buf() const;
      const T* data() const;


      bool operator==() const;
};

// : _buckets(std::bit_ceil(sizeof...(elements)) << 1),_size(0)

#endif //MCSL_SET_HPP