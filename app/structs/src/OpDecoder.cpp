#ifndef OP_DECODER_CPP
#define OP_DECODER_CPP

#include "OpDecoder.hpp"

#include "raw_str_span.hpp"
#include <algorithm>

template <uint _size> template <mcsl::is_t<clef::OpData>... Argv_t>
constexpr clef::OpDecoder<_size>::OpDecoder(const Argv_t... initList)
requires ( sizeof...(Argv_t) == _size )
:_firstCharBuckets{},_opBuf{initList...},_groupBuf{},_groupCount{0} {
   //sort operators - descending order to:
   // * ensure that longer matches come first
   // * ensure that operators with higher precedence come first
   std::sort(_opBuf.begin(), _opBuf.end(), [](const OpData& lhs, const OpData& rhs){
      sint temp = lhs.toString() <=> rhs.toString();
      return temp ? temp > 0 : lhs > rhs;
   });

   //construct groups
   uint start = 0;
   for (uint i = 1; i < _size; ++i) {
      if (_opBuf[i].toString() != _opBuf[start].toString()) {
         _groupBuf[_groupCount] = {start, i-start};
         ++_groupCount;
         start = i;
      }
   }
   //create last group
   _groupBuf[_groupCount] = {start, _size-start};
   ++_groupCount;

   //construct buckets (by first char)
   start = 0;
   for (uint i = 1; i < _groupCount; ++i) {
      //check for end of group
      if (_opBuf[_groupBuf[i].first].toString()[0] != _opBuf[_groupBuf[start].first].toString()[0]) {
         _firstCharBuckets[_opBuf[_groupBuf[start].first].toString()[0] % _firstCharBuckets.size()] = {start, i-start};
         start = i;
      }
   }
   //handle last group
   _firstCharBuckets[_opBuf[_groupBuf[start].first].toString()[0] % _firstCharBuckets.size()] = {start, _groupCount-start};

   //set ids
   for (byte i = _size; i;) {
      --i;
      _opBuf[i].resetID(i);
   }
}

template <uint _size> template<mcsl::str_t str_t> constexpr clef::OpGroup clef::OpDecoder<_size>::operator[](const str_t& str) const {
   //check string size
   if (!str.size()) {
      return OpGroup{};
   }

   //find operator group
   const auto bucketBounds = self[str[0]];
   assert(str[0] == _groupBuf[bucketBounds.first].toString().toString()[0]); //check that the first character is correct
   for (uint i = bucketBounds.first; i < bucketBounds.second; ++i) {
      if (!_groupBuf[i].toString().substrcmp(str)) {
         return OpGroup{_opBuf.begin() + _groupBuf[i].first, _groupBuf[i].second};
      }
   }
   //no operator group found - return null group
   return OpGroup{};
}

constexpr auto clef::GetAllOplikesData() {
   using enum clef::OpType;
   byte prec = ~0;
   return OpDecoder{
      OpData("\\",    --prec, PREFIX),      //escape character
      OpData(";",       prec, PREFIX),      //end of statement

      OpData("\"",    --prec, BLOCK_DELIM), //string
      OpData("\'",      prec, BLOCK_DELIM), //char

      OpData("//",      prec, PREFIX),      //line comment
      OpData("/*",      prec, BLOCK_DELIM), //block comment
      OpData("*/",      prec, BLOCK_DELIM), //block comment

      OpData("(",     --prec, BLOCK_DELIM), //function calls/functional casts
      OpData(")",       prec, BLOCK_DELIM), //function calls/functional casts
      OpData("[",       prec, BLOCK_DELIM), //subscript
      OpData("]",       prec, BLOCK_DELIM), //subscript
      OpData("{",       prec, BLOCK_DELIM), //scope/functional casts
      OpData("}",       prec, BLOCK_DELIM), //scope/functional casts
      OpData("<",       prec, BLOCK_DELIM), //specifier
      OpData(">",       prec, BLOCK_DELIM), //specifier





      OpData("#",     --prec, PREFIX),            //invoke preprocessor

      OpData("::",    --prec, INFIX_LEFT),        //scope resolution

      OpData("++",    --prec, PREFIX),            //pre-increment
      OpData("--",      prec, PREFIX),            //pre-decrement
      OpData(".",       prec, INFIX_LEFT),        //element access
      OpData("->",      prec, INFIX_LEFT),        //element access
      OpData("..",      prec, INFIX_LEFT),        //range
      OpData("...",     prec, INFIX_LEFT),        //array spread

      OpData("++",    --prec, POSTFIX),           //post-increment
      OpData("--",      prec, POSTFIX),           //post-decrement
      OpData("+",       prec, POSTFIX),           //unary plus
      OpData("-",       prec, POSTFIX),           //integer negation
      OpData("!",       prec, POSTFIX),           //logical negation
      OpData("~",       prec, POSTFIX),           //bitwise negation
      OpData("&",       prec, TYPE_MOD | PREFIX), //reference/address of
      OpData("*",       prec, TYPE_MOD | PREFIX), //raw pointer/dereference
      OpData("@",       prec, TYPE_MOD),          //unique pointer
      OpData("$",       prec, TYPE_MOD),          //shared pointer
      OpData("`",       prec, TYPE_MOD),          //weak pointer
      OpData("%",       prec, TYPE_MOD),          //iterator
      OpData("<",       prec, BLOCK_DELIM),       //specifier
      OpData(">",       prec, BLOCK_DELIM),       //specifier
      
      OpData(".*",    --prec, INFIX_LEFT),        //pointer to member
      OpData("->*",     prec, INFIX_LEFT),        //pointer to member

      OpData("^^",    --prec, INFIX_LEFT),        //exponentiation

      OpData("*",     --prec, INFIX_LEFT),        //multiplication
      OpData("/",       prec, INFIX_LEFT),        //division
      OpData("%",       prec, INFIX_LEFT),        //modulo

      OpData("+",     --prec, INFIX_LEFT),        //addition
      OpData("-",       prec, INFIX_LEFT),        //subtraction

      OpData("<<",    --prec, INFIX_LEFT),        //left bit-shift
      OpData(">>",      prec, INFIX_LEFT),        //right bit-shift

      OpData("<=>",   --prec, INFIX_LEFT),        //three-way comparison

      OpData(">",     --prec, INFIX_LEFT),        //less than
      OpData("<",       prec, INFIX_LEFT),        //greater than
      OpData("<=",      prec, INFIX_LEFT),        //less than or equal to
      OpData(">=",      prec, INFIX_LEFT),        //greather than or equal to

      OpData("==",    --prec, INFIX_LEFT),        //equality
      OpData("!=",      prec, INFIX_LEFT),        //inequality
      OpData("===",     prec, INFIX_LEFT),        //strict equality
      OpData("!==",     prec, INFIX_LEFT),        //strict inequality

      OpData("&",     --prec, INFIX_LEFT),        //bitwise AND
      OpData("^",     --prec, INFIX_LEFT),        //bitwise XOR
      OpData("|",     --prec, INFIX_LEFT),        //bitwise OR
      OpData("&&",    --prec, INFIX_LEFT),        //logical AND
      OpData("||",    --prec, INFIX_LEFT),        //logical OR

      OpData("\?\?",  --prec, INFIX_RIGHT),       //null coalescing
      OpData("?",       prec, INFIX_RIGHT),       //inline if
      OpData(":",       prec, INFIX_RIGHT),       //inline else
      OpData("=",       prec, INFIX_RIGHT),       //direct assignment
      OpData(":=",      prec, INFIX_RIGHT),       //const assignment
      OpData("+=",      prec, INFIX_RIGHT),       //compound assignment (add)
      OpData("-=",      prec, INFIX_RIGHT),       //compound assignment (sub)
      OpData("*=",      prec, INFIX_RIGHT),       //compound assignment (mul)
      OpData("/=",      prec, INFIX_RIGHT),       //compound assignment (div)
      OpData("%=",      prec, INFIX_RIGHT),       //compound assignment (mod)
      OpData("^^=",     prec, INFIX_RIGHT),       //compound assignment (exp)
      OpData("<<=",     prec, INFIX_RIGHT),       //compound assignment (shl)
      OpData(">>=",     prec, INFIX_RIGHT),       //compound assignment (shr)
      OpData("&=",      prec, INFIX_RIGHT),       //compound assignment (AND)
      OpData("^=",      prec, INFIX_RIGHT),       //compound assignment (XOR)
      OpData("|=",      prec, INFIX_RIGHT),       //compound assignment (OR)
      OpData("\?\?=",   prec, INFIX_RIGHT),       //compound assignment (null coalescing)

      OpData(",",     --prec, INFIX_LEFT)         //comma

      //not included in array: triangle (free/unspecified) operators
   };
}




constexpr auto clef::GetOpData() {
   using enum clef::OpType;
   byte prec = ~0;
   return OpDecoder{
      OpData("#",     --prec, PREFIX),            //invoke preprocessor

      OpData("::",    --prec, INFIX_LEFT),        //scope resolution

      OpData("++",    --prec, PREFIX),            //pre-increment
      OpData("--",      prec, PREFIX),            //pre-decrement
      OpData(".",       prec, INFIX_LEFT),        //element access
      OpData("->",      prec, INFIX_LEFT),        //element access
      OpData("..",      prec, INFIX_LEFT),        //range
      OpData("...",     prec, INFIX_LEFT),        //array spread

      OpData("++",    --prec, POSTFIX),           //post-increment
      OpData("--",      prec, POSTFIX),           //post-decrement
      OpData("+",       prec, POSTFIX),           //unary plus
      OpData("-",       prec, POSTFIX),           //integer negation
      OpData("!",       prec, POSTFIX),           //logical negation
      OpData("~",       prec, POSTFIX),           //bitwise negation
      OpData("&",       prec, TYPE_MOD | PREFIX), //reference/address of
      OpData("*",       prec, TYPE_MOD | PREFIX), //raw pointer/dereference
      OpData("@",       prec, TYPE_MOD),          //unique pointer
      OpData("$",       prec, TYPE_MOD),          //shared pointer
      OpData("`",       prec, TYPE_MOD),          //weak pointer
      OpData("%",       prec, TYPE_MOD),          //iterator
      OpData("<",       prec, BLOCK_DELIM),       //specifier
      OpData(">",       prec, BLOCK_DELIM),       //specifier
      
      OpData(".*",    --prec, INFIX_LEFT),        //pointer to member
      OpData("->*",     prec, INFIX_LEFT),        //pointer to member

      OpData("^^",    --prec, INFIX_LEFT),        //exponentiation

      OpData("*",     --prec, INFIX_LEFT),        //multiplication
      OpData("/",       prec, INFIX_LEFT),        //division
      OpData("%",       prec, INFIX_LEFT),        //modulo

      OpData("+",     --prec, INFIX_LEFT),        //addition
      OpData("-",       prec, INFIX_LEFT),        //subtraction

      OpData("<<",    --prec, INFIX_LEFT),        //left bit-shift
      OpData(">>",      prec, INFIX_LEFT),        //right bit-shift

      OpData("<=>",   --prec, INFIX_LEFT),        //three-way comparison

      OpData(">",     --prec, INFIX_LEFT),        //less than
      OpData("<",       prec, INFIX_LEFT),        //greater than
      OpData("<=",      prec, INFIX_LEFT),        //less than or equal to
      OpData(">=",      prec, INFIX_LEFT),        //greather than or equal to

      OpData("==",    --prec, INFIX_LEFT),        //equality
      OpData("!=",      prec, INFIX_LEFT),        //inequality
      OpData("===",     prec, INFIX_LEFT),        //strict equality
      OpData("!==",     prec, INFIX_LEFT),        //strict inequality

      OpData("&",     --prec, INFIX_LEFT),        //bitwise AND
      OpData("^",     --prec, INFIX_LEFT),        //bitwise XOR
      OpData("|",     --prec, INFIX_LEFT),        //bitwise OR
      OpData("&&",    --prec, INFIX_LEFT),        //logical AND
      OpData("||",    --prec, INFIX_LEFT),        //logical OR

      OpData("\?\?",  --prec, INFIX_RIGHT),       //null coalescing
      OpData("?",       prec, INFIX_RIGHT),       //inline if
      OpData(":",       prec, INFIX_RIGHT),       //inline else
      OpData("=",       prec, INFIX_RIGHT),       //direct assignment
      OpData(":=",      prec, INFIX_RIGHT),       //const assignment
      OpData("+=",      prec, INFIX_RIGHT),       //compound assignment (add)
      OpData("-=",      prec, INFIX_RIGHT),       //compound assignment (sub)
      OpData("*=",      prec, INFIX_RIGHT),       //compound assignment (mul)
      OpData("/=",      prec, INFIX_RIGHT),       //compound assignment (div)
      OpData("%=",      prec, INFIX_RIGHT),       //compound assignment (mod)
      OpData("^^=",     prec, INFIX_RIGHT),       //compound assignment (exp)
      OpData("<<=",     prec, INFIX_RIGHT),       //compound assignment (shl)
      OpData(">>=",     prec, INFIX_RIGHT),       //compound assignment (shr)
      OpData("&=",      prec, INFIX_RIGHT),       //compound assignment (AND)
      OpData("^=",      prec, INFIX_RIGHT),       //compound assignment (XOR)
      OpData("|=",      prec, INFIX_RIGHT),       //compound assignment (OR)
      OpData("\?\?=",   prec, INFIX_RIGHT),       //compound assignment (null coalescing)

      OpData(",",     --prec, INFIX_LEFT)         //comma

      //not included in array: triangle (free/unspecified) operators
   };
}
constexpr auto clef::GetPtxtData() {
   using enum clef::OpType;
   return OpDecoder{
      OpData("\"",    1,  BLOCK_DELIM), //string
      OpData("\'",    1,  BLOCK_DELIM), //char

      OpData("//",    1,  PREFIX),      //line comment
      OpData("/*",    1,  BLOCK_DELIM), //block comment
      OpData("*/",    1,  BLOCK_DELIM), //block comment
   };
}
constexpr auto clef::GetBlockData() {
   using enum clef::OpType;
   return OpDecoder{
      OpData("(",     3,  BLOCK_DELIM), //function calls/functional casts
      OpData("[",     3,  BLOCK_DELIM), //subscript
      OpData("{",     3,  BLOCK_DELIM), //scope/functional casts
      OpData("<",     3,  BLOCK_DELIM), //specifier

      OpData(")",     3,  BLOCK_DELIM), //function calls/functional casts
      OpData("]",     3,  BLOCK_DELIM), //subscript
      OpData("}",     3,  BLOCK_DELIM), //scope/functional casts
      OpData(">",     3,  BLOCK_DELIM), //specifier
   };
}

#endif //OP_DECODER_CPP