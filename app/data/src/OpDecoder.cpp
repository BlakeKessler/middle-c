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
            assert(it->combineWith(*back), "invalid operator combination (conflicting strings or precedences)");
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
   using enum clef::OpProps;
   ubyte prec = 16;
   return OpDecoder{
      OpData("#",     OpID::PREPROCESSOR, PREFIX, 0, TokenType::PREPROC_INIT),
      OpData("::",    OpID::SCOPE_RESOLUTION, INFIX_LEFT, 0, TokenType::OP),  //scope resolution
      OpData("\\",    OpID::ESCAPE, PREFIX, 0, TokenType::ESC),      //escape character
      OpData(";",     OpID::EOS, PREFIX, 0, TokenType::EOS),      //end of statement
      OpData("//",    OpID::LINE_CMNT, PREFIX, 0, TokenType::BLOCK_DELIM),      //line comment
      OpData("/*",    OpID::BLOCK_CMNT_OPEN, OPEN_DELIM, 0, TokenType::BLOCK_DELIM), //block comment
      OpData("*/",    OpID::BLOCK_CMNT_CLOSE, CLOSE_DELIM, 0, TokenType::BLOCK_DELIM), //block comment
      OpData("\'",    OpID::CHAR, DELIM, 0, TokenType::BLOCK_DELIM), //char
      OpData("\"",    OpID::STRING, DELIM, 0, TokenType::BLOCK_DELIM), //string
      OpData("`",     OpID::STRING, DELIM, 0, TokenType::BLOCK_DELIM), //interpolated string


      OpData("(",     OpID::CALL_OPEN, OPEN_DELIM, 0, TokenType::BLOCK_DELIM), //function calls/functional casts
      OpData(")",     OpID::CALL_CLOSE, CLOSE_DELIM, 0, TokenType::BLOCK_DELIM), //function calls/functional casts
      OpData("[",     OpID::SUBSCRIPT_OPEN, OPEN_DELIM, 0, TokenType::BLOCK_DELIM), //subscript
      OpData("]",     OpID::SUBSCRIPT_CLOSE, CLOSE_DELIM, 0, TokenType::BLOCK_DELIM), //subscript
      OpData("{",     OpID::LIST_OPEN, OPEN_DELIM, 0, TokenType::BLOCK_DELIM), //scope/functional casts
      OpData("}",     OpID::LIST_CLOSE, CLOSE_DELIM, 0, TokenType::BLOCK_DELIM), //scope/functional casts
      OpData("<:",    OpID::SPECIALIZER_OPEN, OPEN_DELIM, 0, TokenType::BLOCK_DELIM), //specifier
      OpData(":>",    OpID::SPECIALIZER_CLOSE, CLOSE_DELIM, 0, TokenType::BLOCK_DELIM), //specifier





      OpData("++",    OpID::INC, PREFIX, 0, TokenType::OP),            //pre-increment
      OpData("--",    OpID::DEC, PREFIX, 0, TokenType::OP),            //pre-decrement
      OpData(".",     OpID::MEMBER_ACCESS, INFIX_LEFT, 0, TokenType::OP),        //element access
      OpData("->",    OpID::MEMBER_OF_POINTER_ACCESS, INFIX_LEFT, 0, TokenType::OP),        //element access
      OpData("..",    OpID::RANGE, INFIX_LEFT, 0, TokenType::OP),        //range
      OpData("...",   OpID::SPREAD, INFIX_LEFT, 0, TokenType::OP),        //array spread

      OpData("++",    OpID::INC, POSTFIX, 0, TokenType::OP),           //post-increment
      OpData("--",    OpID::DEC, POSTFIX, prec, TokenType::OP),           //post-decrement
      OpData("+",     OpID::UNARY_PLUS, POSTFIX, 0, TokenType::OP),           //unary plus
      OpData("-",     OpID::UNARY_MINUS, POSTFIX, 0, TokenType::OP),           //integer negation
      OpData("!",     OpID::LOGICAL_NOT, POSTFIX, 0, TokenType::OP),           //logical negation
      OpData("~",     OpID::BIT_NOT, POSTFIX, 0, TokenType::OP),           //bitwise negation
      OpData("&",     OpID::ADDRESS_OF, PREFIX, 0, TokenType::OP), //reference/address of
      OpData("*",     OpID::DEREF, PREFIX, 0, TokenType::OP), //raw pointer/dereference
      OpData("&",     OpID::REFERENCE, TYPE_MOD, 0, TokenType::OP), //reference/address of
      OpData("*",     OpID::RAW_PTR, TYPE_MOD, 0, TokenType::OP), //raw pointer/dereference
      OpData("@",     OpID::UNIQUE_PTR, TYPE_MOD, 0, TokenType::OP),          //unique pointer
      OpData("$",     OpID::SHARED_PTR, TYPE_MOD, 0, TokenType::OP),          //shared pointer
      OpData("`",     OpID::WEAK_PTR, TYPE_MOD, 0, TokenType::OP),          //weak pointer
      OpData("%",     OpID::ITERATOR, TYPE_MOD, 0, TokenType::OP),          //iterator
      
      OpData(".*",    OpID::POINTER_TO_MEMBER, INFIX_LEFT, --prec, TokenType::OP),        //pointer to member
      OpData("->*",   OpID::POINTER_TO_MEMBER_OF_POINTER, INFIX_LEFT, prec, TokenType::OP),        //pointer to member

      OpData("^^",    OpID::EXP, INFIX_LEFT, --prec, TokenType::OP),        //exponentiation

      OpData("*",     OpID::MUL, INFIX_LEFT, --prec, TokenType::OP),        //multiplication
      OpData("/",     OpID::DIV, INFIX_LEFT, prec, TokenType::OP),        //division
      OpData("%",     OpID::MOD, INFIX_LEFT, prec, TokenType::OP),        //modulo

      OpData("+",     OpID::ADD, INFIX_LEFT, --prec, TokenType::OP),        //addition
      OpData("-",     OpID::SUB, INFIX_LEFT, prec, TokenType::OP),        //subtraction

      OpData("<<",    OpID::SHIFT_LEFT, INFIX_LEFT, --prec, TokenType::OP),        //left bit-shift
      OpData(">>",    OpID::SHIFT_RIGHT, INFIX_LEFT, prec, TokenType::OP),        //right bit-shift

      OpData("<=>",   OpID::THREE_WAY_COMP, INFIX_LEFT, --prec, TokenType::OP),        //three-way comparison

      OpData("<",     OpID::LESSER, INFIX_LEFT, --prec, TokenType::OP),        //less than
      OpData(">",     OpID::GREATER, INFIX_LEFT, prec, TokenType::OP),        //greater than
      OpData("<=",    OpID::LESSER_OR_EQ, INFIX_LEFT, prec, TokenType::OP),        //less than or equal to
      OpData(">=",    OpID::GREATER_OR_EQ, INFIX_LEFT, prec, TokenType::OP),        //greather than or equal to

      OpData("==",    OpID::IS_EQUAL, INFIX_LEFT, --prec, TokenType::OP),        //equality
      OpData("!=",    OpID::IS_UNEQUAL, INFIX_LEFT, prec, TokenType::OP),        //inequality
      // OpData("===",   OpID::SQUAM, INFIX_LEFT, prec, TokenType::OP),        //strict equality
      // OpData("!==",   OpID::SQUAM, INFIX_LEFT, prec, TokenType::OP),        //strict inequality

      OpData("&",     OpID::BIT_AND, INFIX_LEFT, --prec, TokenType::OP),        //bitwise AND
      OpData("^",     OpID::BIT_XOR, INFIX_LEFT, --prec, TokenType::OP),        //bitwise XOR
      OpData("|",     OpID::BIT_OR, INFIX_LEFT, --prec, TokenType::OP),        //bitwise OR
      OpData("&&",    OpID::LOGICAL_AND, INFIX_LEFT, --prec, TokenType::OP),        //logical AND
      OpData("||",    OpID::LOGICAL_OR, INFIX_LEFT, --prec, TokenType::OP),        //logical OR

      OpData("??",    OpID::COALESCE, INFIX_RIGHT, --prec, TokenType::OP),       //null coalescing
      OpData("?",     OpID::INLINE_IF, INFIX_RIGHT, prec, TokenType::OP),       //inline if
      OpData(":",     OpID::INLINE_ELSE, INFIX_RIGHT, prec, TokenType::OP),       //inline else
      OpData("=",     OpID::ASSIGN, INFIX_RIGHT, prec, TokenType::OP),       //direct assignment
      // OpData(":=",    OpID::CONST_ASSIGN, INFIX_RIGHT, prec, TokenType::OP),       //const assignment
      OpData("+=",    OpID::ADD_ASSIGN, INFIX_RIGHT, prec, TokenType::OP),       //compound assignment (add)
      OpData("-=",    OpID::SUB_ASSIGN, INFIX_RIGHT, prec, TokenType::OP),       //compound assignment (sub)
      OpData("*=",    OpID::MUL_ASSIGN, INFIX_RIGHT, prec, TokenType::OP),       //compound assignment (mul)
      OpData("/=",    OpID::DIV_ASSIGN, INFIX_RIGHT, prec, TokenType::OP),       //compound assignment (div)
      OpData("%=",    OpID::MOD_ASSIGN, INFIX_RIGHT, prec, TokenType::OP),       //compound assignment (mod)
      OpData("^^=",   OpID::EXP_ASSIGN, INFIX_RIGHT, prec, TokenType::OP),       //compound assignment (exp)
      OpData("<<=",   OpID::SHL_ASSIGN, INFIX_RIGHT, prec, TokenType::OP),       //compound assignment (shl)
      OpData(">>=",   OpID::SHR_ASSIGN, INFIX_RIGHT, prec, TokenType::OP),       //compound assignment (shr)
      OpData("&=",    OpID::AND_ASSIGN, INFIX_RIGHT, prec, TokenType::OP),       //compound assignment (AND)
      OpData("^=",    OpID::XOR_ASSIGN, INFIX_RIGHT, prec, TokenType::OP),       //compound assignment (XOR)
      OpData("|=",    OpID::OR_ASSIGN, INFIX_RIGHT, prec, TokenType::OP),       //compound assignment (OR)
      OpData("??=",   OpID::COALESCE_ASSIGN, INFIX_RIGHT, prec, TokenType::OP),       //compound assignment (null coalescing)

      OpData(",",     OpID::COMMA, INFIX_LEFT, --prec, TokenType::OP)         //comma
   };
}

#endif //OP_DECODER_CPP