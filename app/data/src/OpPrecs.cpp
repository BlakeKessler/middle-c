#ifndef OP_PRECS_CPP
#define OP_PRECS_CPP

#include "OpPrecs.hpp"

#pragma region inlinesrc
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-value"
template <mcsl::is_t<clef::OpData>... Argv_t> constexpr clef::OpPrecs::OpPrecs(const Argv_t... initList):
   _precs{} {
      ((_precs[+initList.opID()][makeFlags(initList.props())] = mcsl::pair{initList.precedence(), (bool)(initList.props() & OpProps::IS_LEFT_ASSOC)}), ...);
}
#pragma GCC diagnostic pop

constexpr ubyte clef::OpPrecs::makeFlags(OpProps props) {
   using enum OpProps;
   switch (props) {
      case    TYPE_MOD: [[fallthrough]];
      case      PREFIX: return 0b10;
      case     POSTFIX: return 0b01;
      case  INFIX_LEFT: [[fallthrough]];
      case INFIX_RIGHT: return 0b11;
      default         : UNREACHABLE;
   }
}

constexpr clef::OpPrecs clef::GetAllOpPrecsData() {
   using Op = OpID;
   using Prop = OpProps;
   using Type = TokenType;
   using _ = OpData;
   #define lit(str) mcsl::str_slice{}
   ubyte prec = 16;
   return OpPrecs{
      _(lit("++"),  Op::INC,               Prop::PREFIX,      0,      Type::OP          ), //pre-increment
      _(lit("--"),  Op::DEC,               Prop::PREFIX,      0,      Type::OP          ), //pre-decrement
      _(lit("."),   Op::MEMBER_ACCESS,     Prop::INFIX_LEFT,  0,      Type::OP          ), //element access
      _(lit("->"),  Op::PTR_MEMBER_ACCESS, Prop::INFIX_LEFT,  0,      Type::OP          ), //element access
      _(lit(".."),  Op::RANGE,             Prop::INFIX_LEFT,  0,      Type::OP          ), //range
      _(lit("..."), Op::SPREAD,            Prop::INFIX_LEFT,  0,      Type::OP          ), //array spread

      _(lit("++"),  Op::INC,               Prop::POSTFIX,     0,      Type::OP          ), //post-increment
      _(lit("--"),  Op::DEC,               Prop::POSTFIX,     0,      Type::OP          ), //post-decrement
      _(lit("+"),   Op::UNARY_PLUS,        Prop::PREFIX,      0,      Type::OP          ), //unary plus
      _(lit("-"),   Op::UNARY_MINUS,       Prop::PREFIX,      0,      Type::OP          ), //integer negation
      _(lit("!"),   Op::LOGICAL_NOT,       Prop::PREFIX,      0,      Type::OP          ), //logical negation
      _(lit("~"),   Op::BIT_NOT,           Prop::PREFIX,      0,      Type::OP          ), //bitwise negation
      _(lit("&"),   Op::ADDRESS_OF,        Prop::PREFIX,      0,      Type::OP          ), //reference/address of
      _(lit("*"),   Op::DEREF,             Prop::PREFIX,      0,      Type::OP          ), //raw pointer/dereference
      _(lit("&"),   Op::REFERENCE,         Prop::TYPE_MOD,    0,      Type::OP          ), //reference/address of
      _(lit("*"),   Op::RAW_PTR,           Prop::TYPE_MOD,    0,      Type::OP          ), //raw pointer/dereference

      _(lit(".*"),  Op::METHOD_PTR,        Prop::INFIX_LEFT,  --prec, Type::OP          ), //pointer to member
      _(lit("->*"), Op::ARROW_METHOD_PTR,  Prop::INFIX_LEFT,    prec, Type::OP          ), //pointer to member

      _(lit("^^"),  Op::EXP,               Prop::INFIX_LEFT,  --prec, Type::OP          ), //exponentiation

      _(lit("*"),   Op::MUL,               Prop::INFIX_LEFT,  --prec, Type::OP          ), //multiplication
      _(lit("/"),   Op::DIV,               Prop::INFIX_LEFT,    prec, Type::OP          ), //division
      _(lit("%"),   Op::MOD,               Prop::INFIX_LEFT,    prec, Type::OP          ), //modulo

      _(lit("+"),   Op::ADD,               Prop::INFIX_LEFT,  --prec, Type::OP          ), //addition
      _(lit("-"),   Op::SUB,               Prop::INFIX_LEFT,    prec, Type::OP          ), //subtraction

      _(lit("<<"),  Op::SHIFT_LEFT,        Prop::INFIX_LEFT,  --prec, Type::OP          ), //left bit-shift
      _(lit(">>"),  Op::SHIFT_RIGHT,       Prop::INFIX_LEFT,    prec, Type::OP          ), //right bit-shift

      _(lit("<=>"), Op::THREE_WAY_COMP,    Prop::INFIX_LEFT,  --prec, Type::OP          ), //three-way comparison

      _(lit("<"),   Op::LESSER,            Prop::INFIX_LEFT,  --prec, Type::OP          ), //less than
      _(lit(">"),   Op::GREATER,           Prop::INFIX_LEFT,    prec, Type::OP          ), //greater than
      _(lit("<="),  Op::LESSER_OR_EQ,      Prop::INFIX_LEFT,    prec, Type::OP          ), //less than or equal to
      _(lit(">="),  Op::GREATER_OR_EQ,     Prop::INFIX_LEFT,    prec, Type::OP          ), //greather than or equal to

      _(lit("=="),  Op::IS_EQUAL,          Prop::INFIX_LEFT,  --prec, Type::OP          ), //equality
      _(lit("!="),  Op::IS_UNEQUAL,        Prop::INFIX_LEFT,    prec, Type::OP          ), //inequality

      _(lit("&"),   Op::BIT_AND,           Prop::INFIX_LEFT,  --prec, Type::OP          ), //bitwise AND
      _(lit("^"),   Op::BIT_XOR,           Prop::INFIX_LEFT,  --prec, Type::OP          ), //bitwise XOR
      _(lit("|"),   Op::BIT_OR,            Prop::INFIX_LEFT,  --prec, Type::OP          ), //bitwise OR
      _(lit("&&"),  Op::LOGICAL_AND,       Prop::INFIX_LEFT,  --prec, Type::OP          ), //logical AND
      _(lit("||"),  Op::LOGICAL_OR,        Prop::INFIX_LEFT,  --prec, Type::OP          ), //logical OR

      _(lit("??"),  Op::COALESCE,          Prop::INFIX_RIGHT, --prec, Type::OP          ), //null coalescing
      _(lit("?"),   Op::INLINE_IF,         Prop::INFIX_RIGHT,   prec, Type::OP          ), //inline if
      _(lit(":"),   Op::INLINE_ELSE,       Prop::POSTFIX,       prec, Type::OP          ), //inline else
      _(lit("="),   Op::ASSIGN,            Prop::INFIX_RIGHT,   prec, Type::OP          ), //direct assignment
      _(lit("+="),  Op::ADD_ASSIGN,        Prop::INFIX_RIGHT,   prec, Type::OP          ), //compound assignment (add)
      _(lit("-="),  Op::SUB_ASSIGN,        Prop::INFIX_RIGHT,   prec, Type::OP          ), //compound assignment (sub)
      _(lit("*="),  Op::MUL_ASSIGN,        Prop::INFIX_RIGHT,   prec, Type::OP          ), //compound assignment (mul)
      _(lit("/="),  Op::DIV_ASSIGN,        Prop::INFIX_RIGHT,   prec, Type::OP          ), //compound assignment (div)
      _(lit("%="),  Op::MOD_ASSIGN,        Prop::INFIX_RIGHT,   prec, Type::OP          ), //compound assignment (mod)
      _(lit("^^="), Op::EXP_ASSIGN,        Prop::INFIX_RIGHT,   prec, Type::OP          ), //compound assignment (exp)
      _(lit("<<="), Op::SHL_ASSIGN,        Prop::INFIX_RIGHT,   prec, Type::OP          ), //compound assignment (shl)
      _(lit(">>="), Op::SHR_ASSIGN,        Prop::INFIX_RIGHT,   prec, Type::OP          ), //compound assignment (shr)
      _(lit("&="),  Op::AND_ASSIGN,        Prop::INFIX_RIGHT,   prec, Type::OP          ), //compound assignment (AND)
      _(lit("^="),  Op::XOR_ASSIGN,        Prop::INFIX_RIGHT,   prec, Type::OP          ), //compound assignment (XOR)
      _(lit("|="),  Op::OR_ASSIGN,         Prop::INFIX_RIGHT,   prec, Type::OP          ), //compound assignment (OR)
      _(lit("??="), Op::COALESCE_ASSIGN,   Prop::INFIX_RIGHT,   prec, Type::OP          ), //compound assignment (null coalescing)

      _(lit(","),   Op::COMMA,             Prop::INFIX_LEFT,  --prec, Type::OP          )  //comma
   };
   #undef lit
}

#endif //OP_PRECS_CPP