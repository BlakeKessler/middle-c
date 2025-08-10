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
      Token(M _): m{_} {}
   public:
      //constructors
      Token(): m{._name={},._keyword={}, ._type=TokenType::NONE} {}
      Token(const Token& other): m{} { mcsl::memcpy((ubyte*)this, (ubyte*)&other, sizeof(Token)); }
      Token& operator=(const Token& other) { return *new (this) Token(other); }

      static Token makeIden(const mcsl::str_slice name) {
         return M{
            ._name = name,
            ._keyword = {},
            ._type = name.size() && name[0] == '@' ? TokenType::ATTR : TokenType::IDEN
         };
      }
      static Token makeIden(const mcsl::str_slice name, bool isMacro) {
         return M{
            ._name = name,
            ._keyword = {},
            ._type = isMacro ? TokenType::MACRO_INVOKE : (name.size() && name[0] == '@' ? TokenType::ATTR : TokenType::IDEN)
         };
      }
      static Token makeKeyword(const KeywordID id) {
         return M{
            ._name = {},
            ._keyword = id,
            ._type = +id ? TokenType::KEYWORD : TokenType::IDEN
         };
      }
      static Token makeBool(const bool val) {
         return M{
            ._boolVal = val,
            ._keyword = val ? KeywordID::TRUE : KeywordID::FALSE,
            ._type = TokenType::BOOL_LIT
         };
      }
      static Token makeUint(const ulong val, const KeywordID t) {
         return M{
            ._uintVal = val,
            ._keyword = t == KeywordID::_NOT_A_KEYWORD ? KeywordID::UINT : t,
            ._type = TokenType::UINT_NUM
         };
      }
      static Token makeSint(const slong val, const KeywordID t) {
         return M{
            ._sintVal = val,
            ._keyword = t == KeywordID::_NOT_A_KEYWORD ? KeywordID::SINT : t,
            ._type = TokenType::SINT_NUM
         };
      }
      static Token makeReal(const flong val, const KeywordID t) {
         return M{
            ._realVal = val,
            ._keyword = t == KeywordID::_NOT_A_KEYWORD ? KeywordID::FLOAT : t,
            ._type = TokenType::REAL_NUM
         };
      }
      static Token makeChar(const char val) {
         return M{
            ._charVal = val,
            ._keyword = {},
            ._type = TokenType::BOOL_LIT
         };
      }
      static Token makeStr(const mcsl::str_slice str) {
         return M{
            ._strVal = str,
            ._keyword = KeywordID::_NOT_A_KEYWORD,
            ._type = TokenType::STR_LIT
         };
      }
      static Token makeOp(const OpData op) {
         return M{
            ._op = op,
            ._keyword = {},
            ._type = TokenType::OP
         };
      }
      static Token makeBlock(const BlockType type, const BlockDelimRole role, const OpData op) {
         return M{
            ._blockDelim = {.invoke=op, .type=type, .role=role},
            ._keyword = {},
            ._type = TokenType::BLOCK_DELIM
         };
      }
      static Token makeEOS() {
         return M{
            ._name = {},
            ._keyword = {},
            ._type = TokenType::EOS
         };
      }
      static Token makePreprocInit() {
         return M{
            ._name = {},
            ._keyword = {},
            ._type = TokenType::PREPROC_INIT
         };
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
};

namespace mcsl {
   File& write(File&, const clef::Token&);
   uint writef(File&, const clef::Token&, char mode, FmtArgs args);
};

#endif //TOKEN_HPP