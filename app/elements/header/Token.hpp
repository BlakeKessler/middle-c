#pragma once
#ifndef TOKEN_HPP
#define TOKEN_HPP

#include "CLEF.hpp"
#include "TokenData.hpp"
#include "OpData.hpp"

#include "str_slice.hpp"
#include "pair.hpp"

struct [[clang::trivial_abi]] clef::Token {
   private:
      struct M {
         union {
            const mcsl::str_slice _name; //identifier
            
            ulong _uintVal;
            slong _sintVal;
            flong _realVal;
            bool _boolVal;

            const mcsl::str_slice _strVal;
            char _charVal;
            char8 _char8Val;
            char16 _char16Val;
            char32 _char32Val;

            OpData _op;
            struct {
               OpData invoke;
               BlockType type;
               BlockDelimRole role;
            } _blockDelim;
         };
         KeywordID _keyword;
         TokenType _type;
      } m;
      const mcsl::str_slice _tokstr;
      Token(M _, const mcsl::str_slice tokstr): m{_},_tokstr{tokstr} {}
   public:
      //constructors
      Token(const mcsl::str_slice tokstr = {}): m{._name={},._keyword={}, ._type=TokenType::NONE},_tokstr{tokstr} {}
      Token(const Token& other): m{},_tokstr{other._tokstr} { mcsl::memcpy((ubyte*)this, (ubyte*)&other, sizeof(Token)); }
      Token& operator=(const Token& other) { return *new (this) Token(other); }

      static Token makeIden(const mcsl::str_slice name, const mcsl::str_slice tokstr) {
         return {M{
            ._name = name,
            ._keyword = {},
            ._type = name.size() && name[0] == '@' ? TokenType::ATTR : TokenType::IDEN
         }, tokstr};
      }
      static Token makeIden(const mcsl::str_slice name, bool isMacro, const mcsl::str_slice tokstr) {
         return {M{
            ._name = name,
            ._keyword = {},
            ._type = isMacro ? TokenType::MACRO_INVOKE : (name.size() && name[0] == '@' ? TokenType::ATTR : TokenType::IDEN)
         }, tokstr};
      }
      static Token makeKeyword(const KeywordID id, const mcsl::str_slice tokstr) {
         return {M{
            ._name = {},
            ._keyword = id,
            ._type = +id ? TokenType::KEYWORD : TokenType::IDEN
         }, tokstr};
      }
      static Token makeBool(const bool val, const mcsl::str_slice tokstr) {
         return {M{
            ._boolVal = val,
            ._keyword = val ? KeywordID::TRUE : KeywordID::FALSE,
            ._type = TokenType::BOOL_LIT
         }, tokstr};
      }
      static Token makeUint(const ulong val, const KeywordID t, const mcsl::str_slice tokstr) {
         return {M{
            ._uintVal = val,
            ._keyword = t == KeywordID::_NOT_A_KEYWORD ? KeywordID::UINT : t,
            ._type = TokenType::UINT_NUM
         }, tokstr};
      }
      static Token makeSint(const slong val, const KeywordID t, const mcsl::str_slice tokstr) {
         return {M{
            ._sintVal = val,
            ._keyword = t == KeywordID::_NOT_A_KEYWORD ? KeywordID::SINT : t,
            ._type = TokenType::SINT_NUM
         }, tokstr};
      }
      static Token makeReal(const flong val, const KeywordID t, const mcsl::str_slice tokstr) {
         return {M{
            ._realVal = val,
            ._keyword = t == KeywordID::_NOT_A_KEYWORD ? KeywordID::FLOAT : t,
            ._type = TokenType::REAL_NUM
         }, tokstr};
      }
      static Token makeChar(const char val, const mcsl::str_slice tokstr) {
         return {M{
            ._charVal = val,
            ._keyword = {},
            ._type = TokenType::BOOL_LIT
         }, tokstr};
      }
      static Token makeStr(const mcsl::str_slice str, const mcsl::str_slice tokstr) {
         return {M{
            ._strVal = str,
            ._keyword = KeywordID::_NOT_A_KEYWORD,
            ._type = TokenType::STR_LIT
         }, tokstr};
      }
      static Token makeOp(const OpData op, const mcsl::str_slice tokstr) {
         return {M{
            ._op = op,
            ._keyword = {},
            ._type = TokenType::OP
         }, tokstr};
      }
      static Token makeBlock(const BlockType type, const BlockDelimRole role, const OpData op, const mcsl::str_slice tokstr) {
         return {M{
            ._blockDelim = {.invoke=op, .type=type, .role=role},
            ._keyword = {},
            ._type = TokenType::BLOCK_DELIM
         }, tokstr};
      }
      static Token makeEOS(const mcsl::str_slice tokstr) {
         return {M{
            ._name = {},
            ._keyword = {},
            ._type = TokenType::EOS
         }, tokstr};
      }
      static Token makePreprocInit(const mcsl::str_slice tokstr) {
         return {M{
            ._name = {},
            ._keyword = {},
            ._type = TokenType::PREPROC_INIT
         }, tokstr};
      }

      //getters
      TokenType type() const { return m._type; }
      
      const mcsl::str_slice name() const { debug_assert(m._type == TokenType::IDEN || m._type == TokenType::ATTR); return m._name; }
      ulong uintVal() const { debug_assert(m._type == TokenType::UINT_NUM); return m._uintVal; }
      slong sintVal() const { debug_assert(m._type == TokenType::SINT_NUM); return m._sintVal; }
      flong realVal() const { debug_assert(m._type == TokenType::REAL_NUM); return m._realVal; }
      bool  boolVal() const { debug_assert(m._type == TokenType::BOOL_LIT); return m._boolVal; }
      char  charVal() const { debug_assert(m._type == TokenType::CHAR_LIT); return m._charVal; }
      const mcsl::str_slice strVal() const { debug_assert(m._type == TokenType::STR_LIT); return m._strVal; }

      KeywordID keywordID() const { debug_assert(m._type == TokenType::KEYWORD || isNumber(m._type)); return m._keyword; }
      OpData op() const { debug_assert(m._type == TokenType::OP); return m._op; }
      OpID opID() const { debug_assert(m._type == TokenType::OP); return m._op.opID(); }
      OpProps opProps() const { debug_assert(m._type == TokenType::OP); return m._op.props(); }
      auto block() const { debug_assert(m._type == TokenType::BLOCK_DELIM); return m._blockDelim; }

      const mcsl::str_slice tokStr() const { return _tokstr; }
};

namespace mcsl {
   File& write(File&, const clef::Token&);
   uint writef(File&, const clef::Token&, char mode, FmtArgs args);
};

#endif //TOKEN_HPP