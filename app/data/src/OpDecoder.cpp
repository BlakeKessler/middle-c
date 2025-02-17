#ifndef OP_DECODER_CPP
#define OP_DECODER_CPP

#include "OpDecoder.hpp"

#include "raw_str_span.hpp"
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

      while (++it < bufEnd) { //!NOTE: TEST THIS
         if (it->opID() == back->opID()) {
            assert(back->combineWith(*it), "invalid operator combination (conflicting strings or precedences)");
         } else {
            *++back = *it;
         }
      }
      _opCount = back - _opBuf.begin();
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

template <uint _size> template<mcsl::str_t str_t> [[gnu::const]] constexpr clef::OpData clef::OpDecoder<_size>::operator[](const str_t& str) const {
   //check string size
   if (!str.size()) {
      return OpData{};
   }

   //find operator group
   const auto bucketBounds = self[str[0]];
   if (str[0] == _opBuf[bucketBounds.first].toString()[0]) { //check that the first character is correct
      for (uint i = bucketBounds.first; i < bucketBounds.second; ++i) {
         if (!_opBuf[i].toString().substrcmp(str)) {
            return _opBuf[i];
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
   ubyte prec = 16;
   return OpDecoder{
      _("#",   Op::PREPROCESSOR,      Prop::PREFIX,      0,      Type::PREPROC_INIT), //preprocessor
      _("::",  Op::SCOPE_RESOLUTION,  Prop::INFIX_LEFT,  0,      Type::OP          ), //scope resolution
      _("\\",  Op::ESCAPE,            Prop::PREFIX,      0,      Type::ESC         ), //escape character
      _(";",   Op::EOS,               Prop::PREFIX,      0,      Type::EOS         ), //end of statement
      _("//",  Op::LINE_CMNT,         Prop::PREFIX,      0,      Type::BLOCK_DELIM ), //line comment
      _("/*",  Op::BLOCK_CMNT_OPEN,   Prop::OPEN_DELIM,  0,      Type::BLOCK_DELIM ), //block comment
      _("*/",  Op::BLOCK_CMNT_CLOSE,  Prop::CLOSE_DELIM, 0,      Type::BLOCK_DELIM ), //block comment
      _("\'",  Op::CHAR,              Prop::DELIM,       0,      Type::BLOCK_DELIM ), //char
      _("\"",  Op::STRING,            Prop::DELIM,       0,      Type::BLOCK_DELIM ), //string
      _("`",   Op::INTERP_STRING,     Prop::DELIM,       0,      Type::BLOCK_DELIM ), //interpolated string


      _("(",   Op::CALL_OPEN,         Prop::OPEN_DELIM,  0,      Type::BLOCK_DELIM ), //function calls/functional casts
      _(")",   Op::CALL_CLOSE,        Prop::CLOSE_DELIM, 0,      Type::BLOCK_DELIM ), //function calls/functional casts
      _("[",   Op::SUBSCRIPT_OPEN,    Prop::OPEN_DELIM,  0,      Type::BLOCK_DELIM ), //subscript
      _("]",   Op::SUBSCRIPT_CLOSE,   Prop::CLOSE_DELIM, 0,      Type::BLOCK_DELIM ), //subscript
      _("{",   Op::LIST_OPEN,         Prop::OPEN_DELIM,  0,      Type::BLOCK_DELIM ), //scope/functional casts
      _("}",   Op::LIST_CLOSE,        Prop::CLOSE_DELIM, 0,      Type::BLOCK_DELIM ), //scope/functional casts
      _("<:",  Op::SPECIALIZER_OPEN,  Prop::OPEN_DELIM,  0,      Type::BLOCK_DELIM ), //specifier
      _(":>",  Op::SPECIALIZER_CLOSE, Prop::CLOSE_DELIM, 0,      Type::BLOCK_DELIM ), //specifier





      _("++",  Op::INC,               Prop::PREFIX,      0,      Type::OP          ), //pre-increment
      _("--",  Op::DEC,               Prop::PREFIX,      0,      Type::OP          ), //pre-decrement
      _(".",   Op::MEMBER_ACCESS,     Prop::INFIX_LEFT,  0,      Type::OP          ), //element access
      _("->",  Op::PTR_MEMBER_ACCESS, Prop::INFIX_LEFT,  0,      Type::OP          ), //element access
      _("..",  Op::RANGE,             Prop::INFIX_LEFT,  0,      Type::OP          ), //range
      _("...", Op::SPREAD,            Prop::INFIX_LEFT,  0,      Type::OP          ), //array spread

      _("++",  Op::INC,               Prop::POSTFIX,     0,      Type::OP          ), //post-increment
      _("--",  Op::DEC,               Prop::POSTFIX,     0,      Type::OP          ), //post-decrement
      _("+",   Op::UNARY_PLUS,        Prop::PREFIX,      0,      Type::OP          ), //unary plus
      _("-",   Op::UNARY_MINUS,       Prop::PREFIX,      0,      Type::OP          ), //integer negation
      _("!",   Op::LOGICAL_NOT,       Prop::POSTFIX,     0,      Type::OP          ), //logical negation
      _("~",   Op::BIT_NOT,           Prop::POSTFIX,     0,      Type::OP          ), //bitwise negation
      _("&",   Op::ADDRESS_OF,        Prop::PREFIX,      0,      Type::OP          ), //reference/address of
      _("*",   Op::DEREF,             Prop::PREFIX,      0,      Type::OP          ), //raw pointer/dereference
      _("&",   Op::REFERENCE,         Prop::TYPE_MOD,    0,      Type::OP          ), //reference/address of
      _("*",   Op::RAW_PTR,           Prop::TYPE_MOD,    0,      Type::OP          ), //raw pointer/dereference
      _("@",   Op::UNIQUE_PTR,        Prop::TYPE_MOD,    0,      Type::OP          ), //unique pointer
      _("$",   Op::SHARED_PTR,        Prop::TYPE_MOD,    0,      Type::OP          ), //shared pointer
      _("`",   Op::WEAK_PTR,          Prop::TYPE_MOD,    0,      Type::OP          ), //weak pointer
      _("%",   Op::ITERATOR,          Prop::TYPE_MOD,    0,      Type::OP          ), //iterator

      _(".*",  Op::METHOD_PTR,        Prop::INFIX_LEFT,  --prec, Type::OP          ), //pointer to member
      _("->*", Op::ARROW_METHOD_PTR,  Prop::INFIX_LEFT,    prec, Type::OP          ), //pointer to member

      _("^^",  Op::EXP,               Prop::INFIX_LEFT,  --prec, Type::OP          ), //exponentiation

      _("*",   Op::MUL,               Prop::INFIX_LEFT,  --prec, Type::OP          ), //multiplication
      _("/",   Op::DIV,               Prop::INFIX_LEFT,    prec, Type::OP          ), //division
      _("%",   Op::MOD,               Prop::INFIX_LEFT,    prec, Type::OP          ), //modulo

      _("+",   Op::ADD,               Prop::INFIX_LEFT,  --prec, Type::OP          ), //addition
      _("-",   Op::SUB,               Prop::INFIX_LEFT,    prec, Type::OP          ), //subtraction

      _("<<",  Op::SHIFT_LEFT,        Prop::INFIX_LEFT,  --prec, Type::OP          ), //left bit-shift
      _(">>",  Op::SHIFT_RIGHT,       Prop::INFIX_LEFT,    prec, Type::OP          ), //right bit-shift

      _("<=>", Op::THREE_WAY_COMP,    Prop::INFIX_LEFT,  --prec, Type::OP          ), //three-way comparison

      _("<",   Op::LESSER,            Prop::INFIX_LEFT,  --prec, Type::OP          ), //less than
      _(">",   Op::GREATER,           Prop::INFIX_LEFT,    prec, Type::OP          ), //greater than
      _("<=",  Op::LESSER_OR_EQ,      Prop::INFIX_LEFT,    prec, Type::OP          ), //less than or equal to
      _(">=",  Op::GREATER_OR_EQ,     Prop::INFIX_LEFT,    prec, Type::OP          ), //greather than or equal to

      _("==",  Op::IS_EQUAL,          Prop::INFIX_LEFT,  --prec, Type::OP          ), //equality
      _("!=",  Op::IS_UNEQUAL,        Prop::INFIX_LEFT,    prec, Type::OP          ), //inequality

      _("&",   Op::BIT_AND,           Prop::INFIX_LEFT,  --prec, Type::OP          ), //bitwise AND
      _("^",   Op::BIT_XOR,           Prop::INFIX_LEFT,  --prec, Type::OP          ), //bitwise XOR
      _("|",   Op::BIT_OR,            Prop::INFIX_LEFT,  --prec, Type::OP          ), //bitwise OR
      _("&&",  Op::LOGICAL_AND,       Prop::INFIX_LEFT,  --prec, Type::OP          ), //logical AND
      _("||",  Op::LOGICAL_OR,        Prop::INFIX_LEFT,  --prec, Type::OP          ), //logical OR

      _("??",  Op::COALESCE,          Prop::INFIX_RIGHT, --prec, Type::OP          ), //null coalescing
      _("?",   Op::INLINE_IF,         Prop::INFIX_RIGHT,   prec, Type::OP          ), //inline if
      _(":",   Op::INLINE_ELSE,       Prop::INFIX_RIGHT,   prec, Type::OP          ), //inline else
      _("=",   Op::ASSIGN,            Prop::INFIX_RIGHT,   prec, Type::OP          ), //direct assignment
      _("+=",  Op::ADD_ASSIGN,        Prop::INFIX_RIGHT,   prec, Type::OP          ), //compound assignment (add)
      _("-=",  Op::SUB_ASSIGN,        Prop::INFIX_RIGHT,   prec, Type::OP          ), //compound assignment (sub)
      _("*=",  Op::MUL_ASSIGN,        Prop::INFIX_RIGHT,   prec, Type::OP          ), //compound assignment (mul)
      _("/=",  Op::DIV_ASSIGN,        Prop::INFIX_RIGHT,   prec, Type::OP          ), //compound assignment (div)
      _("%=",  Op::MOD_ASSIGN,        Prop::INFIX_RIGHT,   prec, Type::OP          ), //compound assignment (mod)
      _("^^=", Op::EXP_ASSIGN,        Prop::INFIX_RIGHT,   prec, Type::OP          ), //compound assignment (exp)
      _("<<=", Op::SHL_ASSIGN,        Prop::INFIX_RIGHT,   prec, Type::OP          ), //compound assignment (shl)
      _(">>=", Op::SHR_ASSIGN,        Prop::INFIX_RIGHT,   prec, Type::OP          ), //compound assignment (shr)
      _("&=",  Op::AND_ASSIGN,        Prop::INFIX_RIGHT,   prec, Type::OP          ), //compound assignment (AND)
      _("^=",  Op::XOR_ASSIGN,        Prop::INFIX_RIGHT,   prec, Type::OP          ), //compound assignment (XOR)
      _("|=",  Op::OR_ASSIGN,         Prop::INFIX_RIGHT,   prec, Type::OP          ), //compound assignment (OR)
      _("??=", Op::COALESCE_ASSIGN,   Prop::INFIX_RIGHT,   prec, Type::OP          ), //compound assignment (null coalescing)

      _(",",   Op::COMMA,             Prop::INFIX_LEFT,  --prec, Type::OP          )  //comma
   };
}

#endif //OP_DECODER_CPP