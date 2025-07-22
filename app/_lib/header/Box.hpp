#ifndef BOX_HPP
#define BOX_HPP

#include "CLEF.hpp"

#include "alloc.hpp"

template<typename T> class clef::Box {
   private:
      struct __impl {
         T* ptr;
      };
      __impl m;

      Box(__impl p): m{p} {}
   public:
      template<typename... Argv_t> requires mcsl::valid_ctor<T, Argv_t...> Box(Argv_t... argv):
         m{.ptr = new (mcsl::malloc<T>(1)) T(std::forward<Argv_t>(argv)...)} {}
      static Box intoBox(T* p) { return __impl{p}; } //takes ownership of *p

      ~Box() { free(); }
      void free() {
         std::destroy_at(m.ptr);
         mcsl::free(m.ptr);
      }
      T* release() {
         T* tmp = m.ptr;
         m.ptr = nullptr;
         return tmp;
      }

      T& operator*() { return *m.ptr; }
      T* operator->() { return m.ptr; }
      const T& operator*() const { return *m.ptr; }
      const T* operator->() const { return m.ptr; }
};

#endif