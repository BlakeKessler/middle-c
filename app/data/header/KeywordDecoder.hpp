#ifndef KEYWORD_DECODER_HPP
#define KEYWORD_DECODER_HPP

#include "CLEF.hpp"

#include "raw_str_span.hpp"
#include "raw_buf_str.hpp"
#include <cstring>


//code adapted from gperf


class clef::KeywordDecoder {
   private:
      static constexpr ubyte TOTAL_KEYWORDS  = 96;
      static constexpr ubyte MIN_WORD_LENGTH = 2;
      static constexpr ubyte MAX_WORD_LENGTH = 13;
      static constexpr ubyte MIN_HASH_VALUE  = 11;
      static constexpr ubyte MAX_HASH_VALUE  = 181;

      struct keyword {
         private:
            mcsl::raw_buf_str<MAX_WORD_LENGTH, ubyte> _name;
            KeywordID _id;

         public:
            [[gnu::const]] constexpr keyword(mcsl::raw_buf_str<MAX_WORD_LENGTH, ubyte> name = "", KeywordID id = KeywordID::_NOT_A_KEYWORD):_name{name},_id{id} {}

            [[gnu::const]] constexpr auto name() const { return _name; }
            [[gnu::const]] constexpr auto id() const { return _id; }
            [[gnu::const]] constexpr auto size() const { return _name.size(); }
      };

      static constexpr uint hash(const mcsl::raw_str_span& str);

   public:
      static constexpr KeywordID keywordID(const mcsl::raw_str_span& str);
};


#pragma region src

[[gnu::const]] constexpr uint clef::KeywordDecoder::hash(const mcsl::raw_str_span& str) {
   static constexpr ubyte asso_values[] = {
      182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182,
      182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182,
      182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182,
      182, 182, 182, 182, 182, 75,  182, 95,  182, 0,   182, 35,  182, 182, 182,
      182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182,
      182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182,
      182, 182, 182, 182, 182, 182, 182, 25,  55,  35,  5,   0,   15,  77,   65,
      30,  182, 20,  10,  75,  55,  30,  35,  182, 65,  5,   10,  0,   80,   85,
      5,   60,  182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182,
      182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182,
      182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182,
      182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182,
      182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182,
      182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182,
      182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182,
      182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182,
      182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182, 182,
      182
   };
   return str.size() + asso_values[static_cast<ubyte>(str[1])] + asso_values[static_cast<ubyte>(str[0])] + asso_values[static_cast<ubyte>(str.back())];
}

[[gnu::const]] constexpr clef::KeywordID clef::KeywordDecoder::keywordID(const mcsl::raw_str_span& str) {
   using enum KeywordID;
   static constexpr keyword wordlist[] = {
      {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {},
      {"delete", DELETE},
      {}, {},
      {"else", ELSE},
      {}, {}, {},
      {"template", TEMPLATE},
      {}, {},
      {"ushort", USHORT},
      {"default", DEFAULT},
      {"explicit", EXPLICIT},
      {"self", SELF},
      {},
      {"sshort", SSHORT},
      {"debug_assert", DEBUG_ASSERT},
      {}, {}, {},
      {"struct", STRUCT},
      {"float16", FLOAT_16},
      {"float256", FLOAT_256},
      {}, {},
      {"uint16", UINT_16},
      {"uint256", UINT_256},
      {"static_assert", STATIC_ASSERT},
      {},
      {"float", FLOAT},
      {"sint16", SINT_16},
      {"sint256", SINT_256},
      {},
      {"uint", UINT},
      {"false", FALSE},
      {"assert", ASSERT},
      {"alignas", ALIGNAS},
      {},
      {"sint", SINT},
      {},
      {"sign_t", SIGN_T},
      {"up_cast", UP_CAST},
      {},
      {"func", FUNC},
      {"class", CLASS},
      {"sizeof", SIZEOF},
      {"alignof", ALIGNOF},
      {},
      {"auto", AUTO},
      {"ubyte", UBYTE},
      {},
      {"if", IF},
      {},
      {"case", CASE},
      {"sbyte", SBYTE},
      {},
      {"do", DO},
      {"float128", FLOAT_128},
      {"null", NULLPTR},
      {"uint8", UINT_8},
      {},
      {"uint128", UINT_128},
      {"continue", CONTINUE},
      {"cast", CAST},
      {"sint8", SINT_8},
      {"atomic", ATOMIC},
      {"sint128", SINT_128},
      {"typename", TYPENAME},
      {"true", TRUE},
      {"const", CONST},
      {"typeid", TYPEID},
      {"mutable", MUTABLE},
      {"dyn_cast", DYN_CAST},
      {"this", THIS},
      {"const_cast", CONST_CAST},
      {},
      {"using", USING},
      {},
      {"namespace", NAMESPACE},
      {},
      {"typeof", TYPEOF},
      {"ulong", ULONG},
      {},
      {"interface", INTERFACE},
      {"uword", UWORD},
      {},
      {"slong", SLONG},
      {},
      {"bool", BOOL},
      {"sword", SWORD},
      {}, {},
      {"bit_cast", BIT_CAST},
      {"uptr", UPTR},
      {},
      {"char16", CHAR_UTF_16},
      {"float32", FLOAT_32},
      {"asm", ASM},
      {"sptr", SPTR},
      {},
      {"uint32", UINT_32},
      {},
      {"for", FOR},
      {"immediate", IMMEDIATE},
      {"union", UNION},
      {"sint32", SINT_32},
      {"foreach", FOREACH},
      {"volatile", VOLATILE},
      {"void", VOID},
      {}, {}, {}, {},
      {"mask", MASK},
      {},
      {"return", RETURN},
      {"float64", FLOAT_64},
      {}, {},
      {"catch", CATCH},
      {"uint64", UINT_64},
      {},
      {"readonly", READONLY},
      {"enum", ENUM},
      {},
      {"sint64", SINT_64},
      {},
      {"try", TRY},
      {"constexpr", CONSTEXPR},
      {"char8", CHAR_UTF_8},
      {"goto", GOTO},
      {},
      {"new", NEW},
      {},
      {"break", BREAK},
      {}, {}, {}, {}, {},
      {"arrlen", ARRLEN},
      {}, {}, {},
      {"while", WHILE},
      {}, {}, {}, {}, {},
      {"switch", SWITCH},
      {}, {}, {},
      {"throw", THROW},
      {}, {}, {},
      {"char", CHAR},
      {"match", MATCH},
      {}, {}, {}, {}, {}, {}, {}, {}, {}, {},
      {"char32", CHAR_UTF_32}
   };

   if (str.size() <= MAX_WORD_LENGTH && str.size() >= MIN_WORD_LENGTH) {
      const uint key = hash(str);

      if (key <= MAX_HASH_VALUE) {
         const keyword word = wordlist[key];

         // if (str == word.name()) { return word.id(); }
         if (str.size() == word.size()) {
            if (str[0] == word.name()[0] && !std::memcmp(str.begin() + 1, word.name().begin() + 1, word.size() - 1)) {
               return word.id();
            }
         }
      }
   }
   return _NOT_A_KEYWORD;
}

#pragma endregion src

#endif