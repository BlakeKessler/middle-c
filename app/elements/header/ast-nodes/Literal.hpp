#ifndef LITERAL_HPP
#define LITERAL_HPP

#include "CLEF.hpp"

struct clef::Literal {
   public:
      using SyntaxNode_t = void;
   private:
      union {
         ulong u;
         slong s;
         flong f;
         void* p;
         bool b;
         char ch;
         char8 ch8;
         char16 ch16;
         char32 ch32;
         const mcsl::str_slice str;
      } _val;
      FundTypeID _id;
      Literal(decltype(_val) val, FundTypeID id):_val{val},_id{id} {}
   public:
      Literal();
      Literal(const Literal& other): Literal() { mcsl::memcpy((ubyte*)this, (ubyte*)&other, sizeof(Literal)); }
      Literal& operator=(const Literal& other) { return *new (this) Literal{other}; }

      static Literal makeUint(ulong u, FundTypeID id) {
         debug_assert(isUint(id));
         return Literal{{.u = u}, id};
      }
      static Literal makeSint(slong s, FundTypeID id) {
         debug_assert(isSint(id));
         return Literal{{.s = s}, id};
      }
      static Literal makeReal(flong f, FundTypeID id) {
         debug_assert(isFloat(id));
         return Literal{{.f = f}, id};
      }
      static Literal makePtr(void* p) {
         return Literal{{.p = p}, FundTypeID::NULLPTR_T};
      }
      static Literal makeBool(bool b) {
         return Literal{{.b = b}, FundTypeID::BOOL};
      }
      static Literal makeChar(char ch) {
         return Literal{{.ch = ch}, FundTypeID::CHAR};
      }
      static Literal makeStr(const mcsl::str_slice str) {
         return Literal{{.str = str}, FundTypeID::STR};
      }
};

#endif