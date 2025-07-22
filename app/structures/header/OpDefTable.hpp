#ifndef OP_OVERLOAD_TABLE_HPP
#define OP_OVERLOAD_TABLE_HPP

#include "CLEF.hpp"

#include "static_arr.hpp"

class clef::OpDefTable {
   public:
      enum Ops : uint8 {
         CALL_INVOKE, //parens
         SUBSCRIPT_INVOKE, //square brackets

         INC, //increment
         DEC, //decrement

         RANGE,
         
         ADD, //addition
         SUB, //subtraction
         MUL, //multiplication
         DIV, //division
         MOD, //modulo
         EXP, //exponentation
         
         LOGICAL_NOT,
         LOGICAL_AND,
         LOGICAL_OR,

         BIT_NOT,
         BIT_AND,
         BIT_OR,
         BIT_XOR,
         SHIFT_LEFT,
         SHIFT_RIGHT,
         

         THREE_WAY_COMP, //AKA spaceship
         LESSER, //less than
         GREATER, //greater than
         LESSER_OR_EQ, //less than or equal to
         GREATER_OR_EQ, //greater than or equal to

         IS_EQUAL, //equality comparison
         IS_UNEQUAL, //inequality comparison

         COALESCE, //null coalescing

         ADD_ASSIGN, //compound assignment (addition)
         SUB_ASSIGN, //compound assignment (substraction)
         MUL_ASSIGN, //compound assignment (multiplication)
         DIV_ASSIGN, //compound assignment (division)
         MOD_ASSIGN, //compound assignment (modulo)
         EXP_ASSIGN, //compound assignment (exponentiation)
         SHL_ASSIGN, //compound assignment (left-shift)
         SHR_ASSIGN, //compound assignment (right-shift)
         AND_ASSIGN, //compound assignment (bitwise and)
         XOR_ASSIGN, //compound assignment (bitwise exclusive or)
         OR_ASSIGN, //compound assignment (bitwise or)
         COALESCE_ASSIGN, //compound assignment (null-coalescing)


         __SIZE //the number of overloadable operators
      };

      struct const_entry {
         const SymbolNode* function;
         index<void> overload;

         inline explicit operator bool() const { return function; }
      };
      struct entry {
         SymbolNode* function;
         index<void> overload;

         inline explicit operator bool() const { return function; }
         inline operator const_entry() const { return {function, overload}; }
      };

   private:
      mcsl::static_arr<mcsl::dyn_arr<entry>[3], __SIZE> _buf;
      static inline uint subtableIndex(TypeSpec* lhs, TypeSpec* rhs) {
         debug_assert(lhs || rhs);
         return ((!lhs) << 1) | (!rhs);
      }
      
      /*unsafe*/ void insertUnchecked(SymbolNode*, index<void>, OpID op, TypeSpec* lhs, TypeSpec* rhs);
   public:
      OpDefTable():_buf{} {}

      static Ops toIndex(OpID op) { return (Ops)(+op - 1); }

      entry get(OpID op, TypeSpec* lhs, TypeSpec* rhs);
      const_entry get(OpID op, TypeSpec* lhs, TypeSpec* rhs) const;

      bool insert(SymbolNode*, index<void>, OpID op, TypeSpec* lhs, TypeSpec* rhs);
};

#endif