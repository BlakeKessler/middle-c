#pragma once
#ifndef MCSL_HASH_HPP
#define MCSL_HASH_HPP

#include "MCSL.hpp"
#include <cstring>

namespace mcsl {
   static constexpr const luint FNV_offset = 0xcbf29ce484222325;
   static constexpr const luint FNV_prime = 0x100000001b3;
}

//!FNV hashing for arbitrary object
//!not recommended for most containers (including strings)
template<typename T> struct hash_func {
   using is_hash = void;

   luint operator()(const T& key) const {
      const byte* bytes = reinterpret_cast<byte*>(&key);

      luint hash = FNV_offset;
      for (uint i = 0; i < sizeof(key); ++i) {
         hash ^= bytes[i];
         hash *= FNV_prime;
      }

      return hash;
   }
};

//!transparent FNV hashing for arbitrary object
//!not recommended for most containers (including strings)
struct transparent_hash_func {
   using is_hash = void;

   template<typename T> luint operator()(const T& key) const {
      const byte* bytes = reinterpret_cast<byte*>(&key);

      luint hash = FNV_offset;
      for (uint i = 0; i < sizeof(key); ++i) {
         hash ^= bytes[i];
         hash *= FNV_prime;
      }

      return hash;
   }
};


//!FNV hashing for arbitrary contiguous container
template<typename T> struct contig_hash_func {
   using is_hash = void;
   
   luint operator()(const T& key) const {
      const byte* bytes = reinterpret_cast<byte*>(key.begin());
      const uint byteCount = key.size() * sizeof(key[0]);

      luint hash = FNV_offset;
      for (uint i = 0; i < byteCount; ++i) {
         hash ^= bytes[i];
         hash *= FNV_prime;
      }

      return hash;
   }
};

//!transparent FNV hashing for arbitrary contiguous container
struct transparent_contig_hash_func {
   using is_hash = void;

   template<typename T> luint operator()(const T& key) const {
      const byte* bytes = reinterpret_cast<byte*>(key.begin());
      const uint byteCount = key.size() * sizeof(key[0]);

      luint hash = FNV_offset;
      for (uint i = 0; i < byteCount; ++i) {
         hash ^= bytes[i];
         hash *= FNV_prime;
      }

      return hash;
   }
};


//!FNV hashing for arbitrary container
template<mcsl::container_t T> struct mcsl::container_hash_func {
   using is_hash = void;

   luint operator()(const T& container) const {
      byte* bytes;

      luint hash = FNV_offset;
      for (const auto& key : container) {
         bytes = reinterpret_cast<byte*>(&key);
         for (uint i = 0; i < sizeof(key); ++i) {
            hash ^= bytes[i];
            hash *= FNV_prime;
         }
      }

      return hash;
   }
};

#endif //MCSL_HASH_HPP