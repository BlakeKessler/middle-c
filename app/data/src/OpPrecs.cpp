#ifndef OP_PRECS_CPP
#define OP_PRECS_CPP

#include "OpPrecs.hpp"

#pragma region inlinesrc
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-value"
template <mcsl::is_t<clef::OpPrecs::OpPrecData>... Argv_t> constexpr clef::OpPrecs::OpPrecs(const Argv_t... initList):
   _precs{} {
      ((_precs[+initList.id][makeFlags(initList.props)] = mcsl::pair{initList.prec, (bool)(initList.props & OpProps::IS_LEFT_ASSOC)}), ...);
}
#pragma GCC diagnostic pop

constexpr ubyte clef::OpPrecs::makeFlags(OpProps props) {
   using enum OpProps;
   switch (props) {
      case    TYPE_MOD: fthru;
      case      PREFIX: return 0b10;
      case     POSTFIX: return 0b01;
      case  INFIX_LEFT: fthru;
      case INFIX_RIGHT: return 0b11;
      case        null: return 0b00;
      default         : UNREACHABLE;
   }
}

constexpr clef::OpPrecs clef::GetAllOpPrecsData() {
   using Op = OpID;
   using Prop = OpProps;
   using _ = OpPrecs::OpPrecData;
   ubyte prec = ~0;
   return OpPrecs{
      _(Op::SCOPE_RESOLUTION,  Prop::INFIX_LEFT,  --prec), //scope resolution

      _(Op::INC,               Prop::POSTFIX,       prec), //post-increment
      _(Op::DEC,               Prop::POSTFIX,       prec), //post-decrement
      _(Op::CALL,              Prop::POSTFIX,       prec), //function call
      _(Op::INIT_LIST,         Prop::POSTFIX,       prec), //initializer list
      _(Op::INDEX,             Prop::POSTFIX,       prec), //subscript
      _(Op::MEMBER_ACCESS,     Prop::INFIX_LEFT,    prec), //element access
      _(Op::PTR_MEMBER_ACCESS, Prop::INFIX_LEFT,    prec), //element access



      _(Op::INC,               Prop::PREFIX,      --prec), //pre-increment
      _(Op::DEC,               Prop::PREFIX,        prec), //pre-decrement
      _(Op::SPREAD,            Prop::TYPE_MOD,      prec), //array spread
      _(Op::UNARY_PLUS,        Prop::PREFIX,        prec), //unary plus
      _(Op::UNARY_MINUS,       Prop::PREFIX,        prec), //integer negation
      _(Op::LOGICAL_NOT,       Prop::PREFIX,        prec), //logical negation
      _(Op::BIT_NOT,           Prop::PREFIX,        prec), //bitwise negation
      _(Op::ADDRESS_OF,        Prop::PREFIX,        prec), //reference/address of
      _(Op::DEREF,             Prop::PREFIX,        prec), //raw pointer/dereference
      _(Op::REFERENCE,         Prop::TYPE_MOD,      prec), //reference/address of
      _(Op::RAW_PTR,           Prop::TYPE_MOD,      prec), //raw pointer/dereference

      _(Op::RANGE,             Prop::INFIX_LEFT,  --prec), //range

      _(Op::METHOD_PTR,        Prop::INFIX_LEFT,  --prec), //pointer to member
      _(Op::ARROW_METHOD_PTR,  Prop::INFIX_LEFT,    prec), //pointer to member

      _(Op::EXP,               Prop::INFIX_LEFT,  --prec), //exponentiation

      _(Op::MUL,               Prop::INFIX_LEFT,  --prec), //multiplication
      _(Op::DIV,               Prop::INFIX_LEFT,    prec), //division
      _(Op::MOD,               Prop::INFIX_LEFT,    prec), //modulo

      _(Op::ADD,               Prop::INFIX_LEFT,  --prec), //addition
      _(Op::SUB,               Prop::INFIX_LEFT,    prec), //subtraction

      _(Op::SHL,               Prop::INFIX_LEFT,  --prec), //left bit-shift
      _(Op::SHR,               Prop::INFIX_LEFT,    prec), //right bit-shift

      _(Op::THREE_WAY_COMP,    Prop::INFIX_LEFT,  --prec), //three-way comparison

      _(Op::LESSER,            Prop::INFIX_LEFT,  --prec), //less than
      _(Op::GREATER,           Prop::INFIX_LEFT,    prec), //greater than
      _(Op::LESSER_OR_EQ,      Prop::INFIX_LEFT,    prec), //less than or equal to
      _(Op::GREATER_OR_EQ,     Prop::INFIX_LEFT,    prec), //greather than or equal to

      _(Op::IS_EQUAL,          Prop::INFIX_LEFT,  --prec), //equality
      _(Op::IS_UNEQUAL,        Prop::INFIX_LEFT,    prec), //inequality

      _(Op::BIT_AND,           Prop::INFIX_LEFT,  --prec), //bitwise AND
      _(Op::BIT_XOR,           Prop::INFIX_LEFT,  --prec), //bitwise XOR
      _(Op::BIT_OR,            Prop::INFIX_LEFT,  --prec), //bitwise OR
      _(Op::LOGICAL_AND,       Prop::INFIX_LEFT,  --prec), //logical AND
      _(Op::LOGICAL_OR,        Prop::INFIX_LEFT,  --prec), //logical OR

      _(Op::COALESCE,          Prop::INFIX_RIGHT, --prec), //null coalescing
      _(Op::INLINE_IF,         Prop::INFIX_RIGHT,   prec), //inline if
      _(Op::INLINE_ELSE,       Prop::POSTFIX,       prec), //inline else
      _(Op::ASSIGN,            Prop::INFIX_RIGHT,   prec), //direct assignment
      _(Op::ADD_ASSIGN,        Prop::INFIX_RIGHT,   prec), //compound assignment (add)
      _(Op::SUB_ASSIGN,        Prop::INFIX_RIGHT,   prec), //compound assignment (sub)
      _(Op::MUL_ASSIGN,        Prop::INFIX_RIGHT,   prec), //compound assignment (mul)
      _(Op::DIV_ASSIGN,        Prop::INFIX_RIGHT,   prec), //compound assignment (div)
      _(Op::MOD_ASSIGN,        Prop::INFIX_RIGHT,   prec), //compound assignment (mod)
      _(Op::EXP_ASSIGN,        Prop::INFIX_RIGHT,   prec), //compound assignment (exp)
      _(Op::SHL_ASSIGN,        Prop::INFIX_RIGHT,   prec), //compound assignment (shl)
      _(Op::SHR_ASSIGN,        Prop::INFIX_RIGHT,   prec), //compound assignment (shr)
      _(Op::AND_ASSIGN,        Prop::INFIX_RIGHT,   prec), //compound assignment (AND)
      _(Op::XOR_ASSIGN,        Prop::INFIX_RIGHT,   prec), //compound assignment (XOR)
      _(Op::OR_ASSIGN,         Prop::INFIX_RIGHT,   prec), //compound assignment (OR)
      _(Op::COALESCE_ASSIGN,   Prop::INFIX_RIGHT,   prec), //compound assignment (null coalescing)

      _(Op::COMMA,             Prop::INFIX_LEFT,  --prec)  //comma
   };
   #undef lit
}

#endif //OP_PRECS_CPP