#ifndef KEYWORD_DECODER_HPP
#define KEYWORD_DECODER_HPP

#include "CLEF.hpp"

#include "str_slice.hpp"
#include "raw_buf_str.hpp"

#include "frozen/string.h"
#include "frozen/unordered_map.h"

class clef::KeywordDecoder {
   private:
      static constexpr std::pair<frozen::string, clef::KeywordID> _mapBuf[] = {
         {"void", KeywordID::VOID},
         {"auto", KeywordID::AUTO},
         {"char", KeywordID::CHAR},
         {"char8", KeywordID::CHAR_UTF_8},
         {"char16", KeywordID::CHAR_UTF_16},
         {"char32", KeywordID::CHAR_UTF_32},
         {"bool", KeywordID::BOOL},
         {"ubyte", KeywordID::UBYTE},
         {"ushort", KeywordID::USHORT},
         {"uint", KeywordID::UINT},
         {"ulong", KeywordID::ULONG},
         {"uintptr", KeywordID::UINT_PTR},
         {"uword", KeywordID::UWORD},
         {"uint8", KeywordID::UINT_8},
         {"uint16", KeywordID::UINT_16},
         {"uint32", KeywordID::UINT_32},
         {"uint64", KeywordID::UINT_64},
         {"uint128", KeywordID::UINT_128},
         {"uint256", KeywordID::UINT_256},
         {"sign_t", KeywordID::SIGN_T},
         {"sbyte", KeywordID::SBYTE},
         {"sshort", KeywordID::SSHORT},
         {"sint", KeywordID::SINT},
         {"slong", KeywordID::SLONG},
         {"sintptr", KeywordID::SINT_PTR},
         {"sword", KeywordID::SWORD},
         {"sint8", KeywordID::SINT_8},
         {"sint16", KeywordID::SINT_16},
         {"sint32", KeywordID::SINT_32},
         {"sint64", KeywordID::SINT_64},
         {"sint128", KeywordID::SINT_128},
         {"sint256", KeywordID::SINT_256},
         {"float", KeywordID::FLOAT},
         {"float16", KeywordID::FLOAT_16},
         {"float32", KeywordID::FLOAT_32},
         {"float64", KeywordID::FLOAT_64},
         {"float128", KeywordID::FLOAT_128},
         {"float256", KeywordID::FLOAT_256},
         {"class", KeywordID::CLASS},
         {"struct", KeywordID::STRUCT},
         {"interface", KeywordID::INTERFACE},
         {"union", KeywordID::UNION},
         {"enum", KeywordID::ENUM},
         {"enumunion", KeywordID::ENUM_UNION},
         {"mask", KeywordID::MASK},
         {"namespace", KeywordID::NAMESPACE},
         {"func", KeywordID::FUNC},
         {"const", KeywordID::CONST},
         {"constexpr", KeywordID::CONSTEXPR},
         {"immediate", KeywordID::IMMEDIATE},
         {"view", KeywordID::VIEW},
         {"notown", KeywordID::NON_OWNING},
         {"mutable", KeywordID::MUTABLE},
         {"volatile", KeywordID::VOLATILE},
         {"atomic", KeywordID::ATOMIC},
         {"extern", KeywordID::EXTERN},
         {"inline", KeywordID::INLINE},
         {"noexcept", KeywordID::NOEXCEPT},
         {"virtual", KeywordID::VIRTUAL},
         {"override", KeywordID::OVERRIDE},
         {"public", KeywordID::PUBLIC},
         {"private", KeywordID::PRIVATE},
         {"protected", KeywordID::PROTECTED},
         {"cast", KeywordID::CAST},
         {"up_cast", KeywordID::UP_CAST},
         {"dyn_cast", KeywordID::DYN_CAST},
         {"bit_cast", KeywordID::BIT_CAST},
         {"const_cast", KeywordID::CONST_CAST},
         {"typeof", KeywordID::TYPEOF},
         {"sizeof", KeywordID::SIZEOF},
         {"alignas", KeywordID::ALIGNAS},
         {"alignof", KeywordID::ALIGNOF},
         {"goto", KeywordID::GOTO},
         {"return", KeywordID::RETURN},
         {"if", KeywordID::IF},
         {"else", KeywordID::ELSE},
         {"for", KeywordID::FOR},
         {"foreach", KeywordID::FOREACH},
         {"while", KeywordID::WHILE},
         {"do", KeywordID::DO},
         {"break", KeywordID::BREAK},
         {"continue", KeywordID::CONTINUE},
         {"switch", KeywordID::SWITCH},
         {"match", KeywordID::MATCH},
         {"case", KeywordID::CASE},
         {"default", KeywordID::DEFAULT},
         {"try", KeywordID::TRY},
         {"catch", KeywordID::CATCH},
         {"throw", KeywordID::THROW},
         {"this", KeywordID::THIS},
         {"self", KeywordID::SELF},
         {"true", KeywordID::TRUE},
         {"false", KeywordID::FALSE},
         {"null", KeywordID::NULLPTR},
         {"assert", KeywordID::ASSERT},
         {"static_assert", KeywordID::STATIC_ASSERT},
         {"debug_assert", KeywordID::DEBUG_ASSERT},
         {"assume", KeywordID::ASSUME},
         {"let", KeywordID::LET},
         {"using", KeywordID::ALIAS},
         {"asm", KeywordID::ASM}
      };

      static constexpr frozen::unordered_map _map = frozen::make_unordered_map(_mapBuf);
   public:
      static constexpr KeywordID keywordID(const mcsl::str_slice& str) {
         const auto tmp = _map.find(frozen::string{str.begin(), str.size()});
         if (tmp == _map.end()) {
            return {};
         }
         return tmp->second;
      }
};

#endif