#ifndef OP_DECODER_CPP
#define OP_DECODER_CPP

#include "OpDecoder.hpp"

#include "str_slice.hpp"
#include "assert.hpp"
#include <algorithm>

template <uint _size> template <mcsl::is_t<clef::OpData>... Argv_t>
constexpr clef::OpDecoder<_size>::OpDecoder(const Argv_t... initList)
requires ( sizeof...(Argv_t) == _size )
:_firstCharBuckets{},_opBuf{initList...},_opCount{0} {
   //sort operators - descending order to:
   // * ensure that longer matches come first
   // * ensure that operators with higher precedence come first
   std::sort(_opBuf.begin(), _opBuf.end(), [](const OpData& lhs, const OpData& rhs){
      sint temp = lhs.toString() <=> rhs.toString();
      return temp ? temp > 0 : lhs > rhs;
   });

   ///remove duplicates and adjust OpProps accordingly
   {
      OpData* it = _opBuf.begin();
      OpData* back = _opBuf.begin();
      OpData* bufEnd = _opBuf.end();

      while (++it < bufEnd) {
         if (it->opID() == back->opID()) {
            assert(back->combineWith(*it), "invalid operator combination (conflicting strings or precedences)");
         } else {
            *++back = *it;
         }
      }
      _opCount = back - _opBuf.begin() + 1;
      debug_assert(_opCount <= _opBuf.size());
   }
   

   //construct buckets (by first char)
   uint start = 0;
   for (uint i = 1; i < _opCount; ++i) {
      //check for end of group
      if (_opBuf[i].toString()[0] != _opBuf[start].toString()[0]) {
         _firstCharBuckets[_opBuf[start].toString()[0] % _firstCharBuckets.size()] = {start, i};
         start = i;
      }
   }
   //handle last group
   _firstCharBuckets[_opBuf[start].toString()[0] % _firstCharBuckets.size()] = {start, _opCount};
}

template <uint _size> [[gnu::const]] constexpr clef::OpData clef::OpDecoder<_size>::operator[](const mcsl::str_slice str) const {
   //check string size
   if (!str.size()) {
      return OpData{};
   }

   //find operator group
   const auto bucketBounds = self[str[0]];
   if (str[0] == _opBuf[bucketBounds.first].toString()[0]) { //check that the first character is correct
      for (uint i = bucketBounds.first; i < bucketBounds.second; ++i) {
         OpData op = _opBuf[i];
         mcsl::str_slice opStr = op.toString();
         if (str.size() >= opStr.size() && opStr == str.slice(opStr.size())) {
            return op;
         }
      }
   }
   //no operator group found - return null group
   return OpData{};
}

[[gnu::const]] constexpr auto clef::GetAllOplikesData() {
   using Op = OpID;
   using Prop = OpProps;
   using Type = TokenType;
   using _ = OpData;
   #define lit(str) mcsl::str_slice::make_from_cstr(str)
   // using lit = mcsl::raw_buf_str<MAX_OP_LEN, ubyte>;
   ubyte prec = 16;
   return OpDecoder{
      _(lit("#"),   Op::PREPROCESSOR,      Prop::PREFIX,      0,      Type::PREPROC_INIT), //preprocessor
      _(lit("::"),  Op::SCOPE_RESOLUTION,  Prop::INFIX_LEFT,  0,      Type::OP          ), //scope resolution
      _(lit("\\"),  Op::ESCAPE,            Prop::PREFIX,      0,      Type::ESC         ), //escape character
      _(lit(";"),   Op::EOS,               Prop::PREFIX,      0,      Type::EOS         ), //end of statement
      _(lit("//"),  Op::LINE_CMNT,         Prop::PREFIX,      0,      Type::BLOCK_DELIM ), //line comment
      _(lit("/*"),  Op::BLOCK_CMNT_OPEN,   Prop::OPEN_DELIM,  0,      Type::BLOCK_DELIM ), //block comment
      _(lit("*/"),  Op::BLOCK_CMNT_CLOSE,  Prop::CLOSE_DELIM, 0,      Type::BLOCK_DELIM ), //block comment
      _(lit("\'"),  Op::CHAR,              Prop::DELIM,       0,      Type::BLOCK_DELIM ), //char
      _(lit("\""),  Op::STRING,            Prop::DELIM,       0,      Type::BLOCK_DELIM ), //string
      _(lit("`"),   Op::INTERP_STRING,     Prop::DELIM,       0,      Type::BLOCK_DELIM ), //interpolated string
      _(lit("@"),   Op::ATTRIBUTE,         Prop::PREFIX,      0,      Type::OP          ), //interpolated string


      _(lit("("),   Op::CALL_OPEN,         Prop::OPEN_DELIM,  0,      Type::BLOCK_DELIM ), //function calls/functional casts
      _(lit(")"),   Op::CALL_CLOSE,        Prop::CLOSE_DELIM, 0,      Type::BLOCK_DELIM ), //function calls/functional casts
      _(lit("["),   Op::SUBSCRIPT_OPEN,    Prop::OPEN_DELIM,  0,      Type::BLOCK_DELIM ), //subscript
      _(lit("]"),   Op::SUBSCRIPT_CLOSE,   Prop::CLOSE_DELIM, 0,      Type::BLOCK_DELIM ), //subscript
      _(lit("{"),   Op::LIST_OPEN,         Prop::OPEN_DELIM,  0,      Type::BLOCK_DELIM ), //scope/functional casts
      _(lit("}"),   Op::LIST_CLOSE,        Prop::CLOSE_DELIM, 0,      Type::BLOCK_DELIM ), //scope/functional casts
      _(lit("<:"),  Op::SPECIALIZER_OPEN,  Prop::OPEN_DELIM,  0,      Type::BLOCK_DELIM ), //specifier
      _(lit(":>"),  Op::SPECIALIZER_CLOSE, Prop::CLOSE_DELIM, 0,      Type::BLOCK_DELIM ), //specifier





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
      _(lit("[]"),  Op::SLICE,             Prop::TYPE_MOD,    0,      Type::OP          ), //slice

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

#endif //OP_DECODER_CPP