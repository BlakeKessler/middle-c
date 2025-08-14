#ifndef ATTRS_HPP
#define ATTRS_HPP

#include "CLEF.hpp"

class clef::Attr {
   private:
      Symbol* _symbol;
      Args* _args;

      Attr* _next;
      Attr* _prev;
   public:
};

#endif