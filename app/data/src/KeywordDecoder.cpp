#ifndef KEYWORD_DECODER_CPP
#define KEYWORD_DECODER_CPP

#include "KeywordDecoder.hpp"

template<uint _size> template <mcsl::is_t<clef::KeywordSpec>... Argv_t>
clef::KeywordDecoder<_size>::KeywordDecoder(const Argv_t... initList)
requires ( sizeof...(Argv_t) == _size ):_buf{initList...},_map{} {
   for (uint i = 0; i < _buf.size(); ++i) {
      _map[_buf[i].toString()] = i;
   }
}

template<uint _size> clef::KeywordID clef::KeywordDecoder<_size>::operator[](const mcsl::raw_str_span& tok) const {
   const auto tmp = _map.find(tok);
   if (tmp == _map.end()) {
      return KeywordID::_NOT_A_KEYWORD;
   }
   return _buf[tmp->second];
}



auto clef::GET_ALL_KEYWORDS() {
   using enum KeywordID;
   return KeywordDecoder{
      KeywordSpec{mcsl::raw_str{"void"}, VOID},
      KeywordSpec{mcsl::raw_str{"auto"}, AUTO},


      KeywordSpec{mcsl::raw_str{"char"}, CHAR},
      KeywordSpec{mcsl::raw_str{"char_utf8"}, CHAR_UTF_8},
      KeywordSpec{mcsl::raw_str{"char_utf16"}, CHAR_UTF_16},
      KeywordSpec{mcsl::raw_str{"char_utf32"}, CHAR_UTF_32},
      

      KeywordSpec{mcsl::raw_str{"bool"}, BOOL},
      KeywordSpec{mcsl::raw_str{"ubyte"}, UBYTE},
      KeywordSpec{mcsl::raw_str{"ushort"}, USHORT},
      KeywordSpec{mcsl::raw_str{"uint"}, UINT},
      KeywordSpec{mcsl::raw_str{"ulong"}, ULONG},

      KeywordSpec{mcsl::raw_str{"sign_t"}, SIGN_T},
      KeywordSpec{mcsl::raw_str{"sbyte"}, SBYTE},
      KeywordSpec{mcsl::raw_str{"sshort"}, SSHORT},
      KeywordSpec{mcsl::raw_str{"sint"}, SINT},
      KeywordSpec{mcsl::raw_str{"slong"}, SLONG},


      KeywordSpec{mcsl::raw_str{"uint8"}, UINT_8},
      KeywordSpec{mcsl::raw_str{"uint16"}, UINT_16},
      KeywordSpec{mcsl::raw_str{"uint32"}, UINT_32},
      KeywordSpec{mcsl::raw_str{"uint64"}, UINT_64},
      KeywordSpec{mcsl::raw_str{"uint128"}, UINT_128},
      KeywordSpec{mcsl::raw_str{"uint256"}, UINT_256},

      KeywordSpec{mcsl::raw_str{"sint8"}, SINT_8},
      KeywordSpec{mcsl::raw_str{"sint16"}, SINT_16},
      KeywordSpec{mcsl::raw_str{"sint32"}, SINT_32},
      KeywordSpec{mcsl::raw_str{"sint64"}, SINT_64},
      KeywordSpec{mcsl::raw_str{"sint128"}, SINT_128},
      KeywordSpec{mcsl::raw_str{"sint256"}, SINT_256},

      KeywordSpec{mcsl::raw_str{"float"}, FLOAT},

      KeywordSpec{mcsl::raw_str{"float16"}, FLOAT_16},
      KeywordSpec{mcsl::raw_str{"float32"}, FLOAT_32},
      KeywordSpec{mcsl::raw_str{"float64"}, FLOAT_64},
      KeywordSpec{mcsl::raw_str{"float128"}, FLOAT_128},
      KeywordSpec{mcsl::raw_str{"float256"}, FLOAT_256},







      KeywordSpec{mcsl::raw_str{"class"}, CLASS},
      KeywordSpec{mcsl::raw_str{"struct"}, STRUCT},
      KeywordSpec{mcsl::raw_str{"interface"}, INTERFACE},
      KeywordSpec{mcsl::raw_str{"union"}, UNION},
      KeywordSpec{mcsl::raw_str{"enum"}, ENUM},
      KeywordSpec{mcsl::raw_str{"mask"}, MASK},
      KeywordSpec{mcsl::raw_str{"namespace"}, NAMESPACE},

      KeywordSpec{mcsl::raw_str{"const"}, CONST},
      KeywordSpec{mcsl::raw_str{"constexpr"}, CONSTEXPR},
      KeywordSpec{mcsl::raw_str{"immediate"}, IMMEDIATE},
      KeywordSpec{mcsl::raw_str{"readonly"}, READONLY},
      KeywordSpec{mcsl::raw_str{"mutable"}, MUTABLE},
      KeywordSpec{mcsl::raw_str{"volatile"}, VOLATILE},
      KeywordSpec{mcsl::raw_str{"atomic"}, ATOMIC},
      KeywordSpec{mcsl::raw_str{"explicit"}, EXPLICIT},

      KeywordSpec{mcsl::raw_str{"cast"}, CAST},
      KeywordSpec{mcsl::raw_str{"up_cast"}, UP_CAST},
      KeywordSpec{mcsl::raw_str{"dyn_cast"}, DYN_CAST},
      KeywordSpec{mcsl::raw_str{"bit_cast"}, BIT_CAST},
      KeywordSpec{mcsl::raw_str{"const_cast"}, CONST_CAST},

      KeywordSpec{mcsl::raw_str{"typeof"}, TYPEOF},
      KeywordSpec{mcsl::raw_str{"typeid"}, TYPEID},
      KeywordSpec{mcsl::raw_str{"typename"}, TYPENAME},
      KeywordSpec{mcsl::raw_str{"sizeof"}, SIZEOF},
      KeywordSpec{mcsl::raw_str{"arrlen"}, ARRLEN},
      KeywordSpec{mcsl::raw_str{"alignas"}, ALIGNAS},
      KeywordSpec{mcsl::raw_str{"alignof"}, ALIGNOF},
      KeywordSpec{mcsl::raw_str{"template"}, TEMPLATE},
      

      KeywordSpec{mcsl::raw_str{"goto"}, GOTO},
      KeywordSpec{mcsl::raw_str{"if"}, IF},
      KeywordSpec{mcsl::raw_str{"else"}, ELSE},
      KeywordSpec{mcsl::raw_str{"for"}, FOR},
      KeywordSpec{mcsl::raw_str{"foreach"}, FOREACH},
      KeywordSpec{mcsl::raw_str{"while"}, WHILE},
      KeywordSpec{mcsl::raw_str{"do"}, DO},
      KeywordSpec{mcsl::raw_str{"break"}, BREAK},
      KeywordSpec{mcsl::raw_str{"continue"}, CONTINUE},
      KeywordSpec{mcsl::raw_str{"switch"}, SWITCH},
      KeywordSpec{mcsl::raw_str{"match"}, MATCH},
      KeywordSpec{mcsl::raw_str{"case"}, CASE},
      KeywordSpec{mcsl::raw_str{"default"}, DEFAULT},
      KeywordSpec{mcsl::raw_str{"try"}, TRY},
      KeywordSpec{mcsl::raw_str{"catch"}, CATCH},
      KeywordSpec{mcsl::raw_str{"throw"}, THROW},




      KeywordSpec{mcsl::raw_str{"this"}, THIS},
      KeywordSpec{mcsl::raw_str{"self"}, SELF},

      KeywordSpec{mcsl::raw_str{"new"}, NEW},
      KeywordSpec{mcsl::raw_str{"delete"}, DELETE},

      KeywordSpec{mcsl::raw_str{"assert"}, ASSERT},
      KeywordSpec{mcsl::raw_str{"static_assert"}, STATIC_ASSERT},
      KeywordSpec{mcsl::raw_str{"debug_assert"}, DEBUG_ASSERT},

      KeywordSpec{mcsl::raw_str{"true"}, TRUE},
      KeywordSpec{mcsl::raw_str{"false"}, FALSE},

      KeywordSpec{mcsl::raw_str{"nullptr"}, NULLPTR},

      KeywordSpec{mcsl::raw_str{"return"}, RETURN},

      KeywordSpec{mcsl::raw_str{"using"}, USING},

      // KeywordSpec{mcsl::raw_str{"asm"}, ASM},
   };
}


#endif //KEYWORD_DECODER_CPP