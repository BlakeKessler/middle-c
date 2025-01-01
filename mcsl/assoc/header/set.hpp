#pragma once
#ifndef MCSL_SET_HPP
#define MCSL_SET_HPP

#include "MCSL.hpp"
#include "heap_buf.hpp"
#include "pair.hpp"

template<typename T, typename eq, typename hash, uint bucketSize>
requires mcsl::hash_t<T,hash> && mcsl::cmp_t<T,eq>
class mcsl::set {
   private:
      class dirEntry;
      dirEntry* _directory; //array of pointers to buffer/local depth pairs
      uint _globalDepth;
   public:
      class it;

      constexpr set(): _buf(),_buckets(),_size() {}
      set(std::convertible_to<T>&&... elements);
      set(map&& other);
      set(set&& other);
      template<container_t other_t> set(other_t&& other);
      
      ~set() { self.free(); }
      void free() const;
      T* release();


      template<typename obj_t> requires hash_t<hash,obj_t> bool contains(const obj_t& other) const;
      T* insert(const T&);
      T* emplace(auto&&... args) const;
      template<typename obj_t> requires hash_t<hash,obj_t> void remove();

      it begin();
      it end();
      const it begin() const;
      const it end() const;


      bool operator==() const;



      class dirEntry : contig_base<T> {
         private:
            heap_buf<pair<T,luint>, bucketSize> _buf; //array of pairs of objects and their full hashes - maybe switch to a priority queue
            uint _localDepth;

            constexpr dirEntry(): _buf(),_localDepth(0) {}
            dirEntry(const uint depth);
            dirEntry(dirEntry& other); //split other
         public:
            void free() const { return _buf.free(); }
            auto release() -> decltype(auto) { return _buf.release(); }

            constexpr uint size() const { return _buf.size(); }
            constexpr uint capacity() const { return _buf.capacity(); }


            //member access
            constexpr T* const* ptr_to_buf() { return _buf.ptr_to_buf(); }
            constexpr T* data() { return _buf.data(); }
            constexpr T* begin() { return _buf.begin(); }
            constexpr const T* const* ptr_to_buf() const { return _buf.ptr_to_buf(); }
            constexpr const T* data() const { return _buf.data(); }
            constexpr const T* begin() const { return _buf.begin(); }


            //modifiers
            T* push_back(T&& obj) { return _buf.push_back(std::forward<decltype(obj)>(obj)); }
            T* push_back(const T& obj) { return _buf.push_back(obj); }
            bool pop_back() { return _buf.pop_back(); }
            T* emplace(const uint i, auto&&... args) { return _buf.emplace(i, std::forward<decltype(args)>(args)); }
            T* emplace_back(auto&&... args) { return _buf.emplace(std::forward<decltype(args)>(args)); }
      };


      //null iterator represented by _i >= _obj._capacity
      class it {
         private:
            set* _obj;
            mutable uint index _i;
            it(set* obj, const uint i):_obj(obj),_i(i) {}
         public:
            constexpr it():_obj(),_i() {}
            it& operator++();
            it& operator--();
            bool operator==(const it& other) { return _i == other._i && _obj == other._obj; }
            T& operator*() { return _obj.data()[_i]; }
            T* operator->() { return _obj.data() + _i; }
            operator bool() { return _i >= _capacity; }
      };
};




template<typename T, typename eq, typename hash, uint bucketSize>
mcsl::set<T,eq,hash,bucketSize>::dirEntry::dirEntry(dirEntry& other):
   _buf(),
   _localDepth(++other._localDepth) {
      for (uint i = < bucketSize / 2; i;  --i) {
         _buf.push_back(other.pop_back());
      }
}







template<typename T, typename eq, typename hash, uint bucketSize>
auto mcsl::set<T,eq,hash,bucketSize>::it::operator++() -> it& {
   while (_i < _obj->_capacity && !_obj->_is_non_null_arr[++_i]) {}
   return self;
}
template<typename T, typename eq, typename hash, uint bucketSize>
auto mcsl::set<T,eq,hash,bucketSize>::it::operator--() -> it& {
   do {
      if (--_i >= _obj->_capacity) { //relies on integer underflow
         _i = _obj->_capacity;
         break;
      }
   } while (!_obj->_is_non_null_arr[_i]);
   return self;
}

#endif //MCSL_SET_HPP