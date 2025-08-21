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
      Symbol* symbol() { return _symbol; }
      Args* args() { return _args; }

      Attr* next() { return _next; }
      Attr* prev() { return _prev; }
};

#endif