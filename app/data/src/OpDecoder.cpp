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
         if (it->op() == back->op()) {
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
   using Op = Oplike;
   using Prop = OpProps;
   using Type = TokenType;
   using _ = OpData;
   ubyte prec = 16;
   return OpDecoder{
      _(FMT("#"),   Op::PREPROC,           Prop::PREFIX,      0,      Type::PREPROC_INIT), //preprocessor
      _(FMT("::"),  Op::SCOPE_RESOLUTION,  Prop::INFIX_LEFT,  0,      Type::OP          ), //scope resolution
      _(FMT("\\"),  Op::ESC,               Prop::PREFIX,      0,      Type::NONE        ), //escape character
      _(FMT(";"),   Op::EOS,               Prop::PREFIX,      0,      Type::EOS         ), //end of statement
      _(FMT("//"),  Op::LINE_CMNT,         Prop::PREFIX,      0,      Type::BLOCK_DELIM ), //line comment
      _(FMT("/*"),  Op::BLOCK_CMNT_OPEN,   Prop::OPEN_DELIM,  0,      Type::BLOCK_DELIM ), //block comment
      _(FMT("*/"),  Op::BLOCK_CMNT_CLOSE,  Prop::CLOSE_DELIM, 0,      Type::BLOCK_DELIM ), //block comment
      _(FMT("\'"),  Op::CHAR,              Prop::DELIM,       0,      Type::BLOCK_DELIM ), //char
      _(FMT("\""),  Op::STRING,            Prop::DELIM,       0,      Type::BLOCK_DELIM ), //string
      _(FMT("@"),   Op::ATTR,              Prop::PREFIX,      0,      Type::OP          ), //interpolated string


      _(FMT("("),   Op::CALL_OPEN,         Prop::OPEN_DELIM,  0,      Type::BLOCK_DELIM ), //function calls/functional casts
      _(FMT(")"),   Op::CALL_CLOSE,        Prop::CLOSE_DELIM, 0,      Type::BLOCK_DELIM ), //function calls/functional casts
      _(FMT("["),   Op::INDEX_OPEN,        Prop::OPEN_DELIM,  0,      Type::BLOCK_DELIM ), //subscript
      _(FMT("]"),   Op::INDEX_CLOSE,       Prop::CLOSE_DELIM, 0,      Type::BLOCK_DELIM ), //subscript
      _(FMT("{"),   Op::LIST_OPEN,         Prop::OPEN_DELIM,  0,      Type::BLOCK_DELIM ), //scope/functional casts
      _(FMT("}"),   Op::LIST_CLOSE,        Prop::CLOSE_DELIM, 0,      Type::BLOCK_DELIM ), //scope/functional casts
      _(FMT("<:"),  Op::SPECIALIZER_OPEN,  Prop::OPEN_DELIM,  0,      Type::BLOCK_DELIM ), //specifier
      _(FMT(":>"),  Op::SPECIALIZER_CLOSE, Prop::CLOSE_DELIM, 0,      Type::BLOCK_DELIM ), //specifier

      _(FMT("++"),  Op::INC,               Prop::PREFIX,      0,      Type::OP          ), //pre-increment
      _(FMT("--"),  Op::DEC,               Prop::PREFIX,      0,      Type::OP          ), //pre-decrement
      _(FMT("."),   Op::MEMBER_ACCESS,     Prop::INFIX_LEFT,  0,      Type::OP          ), //element access
      _(FMT("->"),  Op::PTR_MEMBER_ACCESS, Prop::INFIX_LEFT,  0,      Type::OP          ), //element access
      _(FMT(".."),  Op::RANGE,             Prop::INFIX_LEFT,  0,      Type::OP          ), //range
      _(FMT("..."), Op::SPREAD,            Prop::INFIX_LEFT,  0,      Type::OP          ), //array spread

      _(FMT("++"),  Op::INC,               Prop::POSTFIX,     0,      Type::OP          ), //post-increment
      _(FMT("--"),  Op::DEC,               Prop::POSTFIX,     0,      Type::OP          ), //post-decrement
      _(FMT("+"),   Op::UNARY_PLUS,        Prop::PREFIX,      0,      Type::OP          ), //unary plus
      _(FMT("-"),   Op::UNARY_MINUS,       Prop::PREFIX,      0,      Type::OP          ), //integer negation
      _(FMT("!"),   Op::LOGICAL_NOT,       Prop::PREFIX,      0,      Type::OP          ), //logical negation
      _(FMT("~"),   Op::BIT_NOT,           Prop::PREFIX,      0,      Type::OP          ), //bitwise negation
      _(FMT("&"),   Op::ADDRESS_OF,        Prop::PREFIX,      0,      Type::OP          ), //reference/address of
      _(FMT("*"),   Op::DEREF,             Prop::PREFIX,      0,      Type::OP          ), //raw pointer/dereference
      _(FMT("&"),   Op::REFERENCE,         Prop::TYPE_MOD,    0,      Type::OP          ), //reference/address of
      _(FMT("*"),   Op::RAW_PTR,           Prop::TYPE_MOD,    0,      Type::OP          ), //raw pointer/dereference
      _(FMT("[]"),  Op::SLICE,             Prop::TYPE_MOD,    0,      Type::OP          ), //slice

      _(FMT(".*"),  Op::METHOD_PTR,        Prop::INFIX_LEFT,  --prec, Type::OP          ), //pointer to member
      _(FMT("->*"), Op::ARROW_METHOD_PTR,  Prop::INFIX_LEFT,    prec, Type::OP          ), //pointer to member

      _(FMT("^^"),  Op::EXP,               Prop::INFIX_LEFT,  --prec, Type::OP          ), //exponentiation

      _(FMT("*"),   Op::MUL,               Prop::INFIX_LEFT,  --prec, Type::OP          ), //multiplication
      _(FMT("/"),   Op::DIV,               Prop::INFIX_LEFT,    prec, Type::OP          ), //division
      _(FMT("%"),   Op::MOD,               Prop::INFIX_LEFT,    prec, Type::OP          ), //modulo

      _(FMT("+"),   Op::ADD,               Prop::INFIX_LEFT,  --prec, Type::OP          ), //addition
      _(FMT("-"),   Op::SUB,               Prop::INFIX_LEFT,    prec, Type::OP          ), //subtraction

      _(FMT("<<"),  Op::SHL,               Prop::INFIX_LEFT,  --prec, Type::OP          ), //left bit-shift
      _(FMT(">>"),  Op::SHR,               Prop::INFIX_LEFT,    prec, Type::OP          ), //right bit-shift

      _(FMT("<=>"), Op::THREE_WAY_COMP,    Prop::INFIX_LEFT,  --prec, Type::OP          ), //three-way comparison

      _(FMT("<"),   Op::LESSER,            Prop::INFIX_LEFT,  --prec, Type::OP          ), //less than
      _(FMT(">"),   Op::GREATER,           Prop::INFIX_LEFT,    prec, Type::OP          ), //greater than
      _(FMT("<="),  Op::LESSER_OR_EQ,      Prop::INFIX_LEFT,    prec, Type::OP          ), //less than or equal to
      _(FMT(">="),  Op::GREATER_OR_EQ,     Prop::INFIX_LEFT,    prec, Type::OP          ), //greather than or equal to

      _(FMT("=="),  Op::IS_EQUAL,          Prop::INFIX_LEFT,  --prec, Type::OP          ), //equality
      _(FMT("!="),  Op::IS_UNEQUAL,        Prop::INFIX_LEFT,    prec, Type::OP          ), //inequality

      _(FMT("&"),   Op::BIT_AND,           Prop::INFIX_LEFT,  --prec, Type::OP          ), //bitwise AND
      _(FMT("^"),   Op::BIT_XOR,           Prop::INFIX_LEFT,  --prec, Type::OP          ), //bitwise XOR
      _(FMT("|"),   Op::BIT_OR,            Prop::INFIX_LEFT,  --prec, Type::OP          ), //bitwise OR
      _(FMT("&&"),  Op::LOGICAL_AND,       Prop::INFIX_LEFT,  --prec, Type::OP          ), //logical AND
      _(FMT("||"),  Op::LOGICAL_OR,        Prop::INFIX_LEFT,  --prec, Type::OP          ), //logical OR

      _(FMT("??"),  Op::COALESCE,          Prop::INFIX_RIGHT, --prec, Type::OP          ), //null coalescing
      _(FMT("?"),   Op::INLINE_IF,         Prop::INFIX_RIGHT,   prec, Type::OP          ), //inline if
      _(FMT(":"),   Op::INLINE_ELSE,       Prop::POSTFIX,       prec, Type::OP          ), //inline else
      _(FMT("="),   Op::ASSIGN,            Prop::INFIX_RIGHT,   prec, Type::OP          ), //direct assignment
      _(FMT("+="),  Op::ADD_ASSIGN,        Prop::INFIX_RIGHT,   prec, Type::OP          ), //compound assignment (add)
      _(FMT("-="),  Op::SUB_ASSIGN,        Prop::INFIX_RIGHT,   prec, Type::OP          ), //compound assignment (sub)
      _(FMT("*="),  Op::MUL_ASSIGN,        Prop::INFIX_RIGHT,   prec, Type::OP          ), //compound assignment (mul)
      _(FMT("/="),  Op::DIV_ASSIGN,        Prop::INFIX_RIGHT,   prec, Type::OP          ), //compound assignment (div)
      _(FMT("%="),  Op::MOD_ASSIGN,        Prop::INFIX_RIGHT,   prec, Type::OP          ), //compound assignment (mod)
      _(FMT("^^="), Op::EXP_ASSIGN,        Prop::INFIX_RIGHT,   prec, Type::OP          ), //compound assignment (exp)
      _(FMT("<<="), Op::SHL_ASSIGN,        Prop::INFIX_RIGHT,   prec, Type::OP          ), //compound assignment (shl)
      _(FMT(">>="), Op::SHR_ASSIGN,        Prop::INFIX_RIGHT,   prec, Type::OP          ), //compound assignment (shr)
      _(FMT("&="),  Op::AND_ASSIGN,        Prop::INFIX_RIGHT,   prec, Type::OP          ), //compound assignment (AND)
      _(FMT("^="),  Op::XOR_ASSIGN,        Prop::INFIX_RIGHT,   prec, Type::OP          ), //compound assignment (XOR)
      _(FMT("|="),  Op::OR_ASSIGN,         Prop::INFIX_RIGHT,   prec, Type::OP          ), //compound assignment (OR)
      _(FMT("??="), Op::COALESCE_ASSIGN,   Prop::INFIX_RIGHT,   prec, Type::OP          ), //compound assignment (null coalescing)

      _(FMT(","),   Op::COMMA,             Prop::INFIX_LEFT,  --prec, Type::OP          )  //comma
   };
}

#endif //OP_DECODER_CPP