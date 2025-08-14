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
         char ch;
         char8 ch8;
         char16 ch16;
         char32 ch32;
         const mcsl::str_slice str; 
      } _val;
      FundTypeID _id;
   public:
      Literal();
      Literal(const Literal& other): Literal() { mcsl::memcpy((ubyte*)this, (ubyte*)&other, sizeof(Literal)); }
      Literal& operator=(const Literal& other) { return *new (this) Literal{other}; }
};

#endif