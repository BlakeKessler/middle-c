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
      union {
         const mcsl::str_slice _name; //identifier
         ulong _uintVal;
         slong _sintVal;
         flong _realVal;
         const mcsl::str_slice _strVal;
         char _charVal;
      };
      union {
         KeywordID _keyword;
         OpData _op;
         mcsl::pair<BlockType,BlockDelimRole> _blockDelim;
         PtxtType _ptxtType;
      };
      TokenType _type;
   public:
      //constructors
      Token():_type{TokenType::NONE} {}
      Token(const mcsl::str_slice name):_name{name},_type{TokenType::IDEN} {}
      Token(const mcsl::str_slice name, bool isMacro):_name{name},_type{isMacro ? TokenType::MACRO_INVOKE : TokenType::IDEN} {}
      Token(const KeywordID id):_keyword{id},_type{+id ? TokenType::KEYWORD : TokenType::IDEN} {}
      Token(const ulong val, const KeywordID t):_uintVal{val},_keyword{t == KeywordID::_NOT_A_KEYWORD ? KeywordID::UINT  : t},_type{TokenType::UINT_NUM} {}
      Token(const slong val, const KeywordID t):_sintVal{val},_keyword{t == KeywordID::_NOT_A_KEYWORD ? KeywordID::SINT  : t},_type{TokenType::SINT_NUM} {}
      Token(const flong val, const KeywordID t):_realVal{val},_keyword{t == KeywordID::_NOT_A_KEYWORD ? KeywordID::FLOAT : t},_type{TokenType::REAL_NUM} {}
      Token(const OpData op):_op{op},_type{TokenType::OP} {}
      Token(const BlockType type, const BlockDelimRole role):_blockDelim{mcsl::pair{type,role}},_type{TokenType::BLOCK_DELIM} {}
      Token(const mcsl::str_slice val, const PtxtType type):_strVal{val},_ptxtType{type},_type{TokenType::PTXT_SEG} {}
      Token(const char c):_charVal{c},_ptxtType{PtxtType::CHAR},_type{TokenType::PTXT_SEG} {}
      Token(const TokenType type):_type{type} { debug_assert(_type == TokenType::PREPROC_INIT || _type == TokenType::PREPROC_EOS || _type == TokenType::EOS || _type == TokenType::ESC); } //intended for PREPROC_INIT, PREPROC_EOS, EOS, ESC only

      Token(const Token& other) { mcsl::memcpy((ubyte*)this, (ubyte*)&other, sizeof(Token)); }
      Token& operator=(const Token& other) { mcsl::memcpy((ubyte*)this, (ubyte*)&other, sizeof(Token)); return self; }

      //getters
      TokenType type() const { return _type; }
      
      const mcsl::str_slice& name() const { debug_assert(_type == TokenType::IDEN); return _name; }
      ulong uintVal() const { debug_assert(_type == TokenType::UINT_NUM); return _uintVal; }
      slong sintVal() const { debug_assert(_type == TokenType::SINT_NUM); return _sintVal; }
      flong realVal() const { debug_assert(_type == TokenType::REAL_NUM); return _realVal; }
      const mcsl::str_slice& strVal() const { debug_assert(_type == TokenType::PTXT_SEG && _ptxtType == PtxtType::STR); return _strVal; }
      const mcsl::str_slice& unprocessedStrVal() const { debug_assert(_type == TokenType::PTXT_SEG && _ptxtType == PtxtType::UNPROCESSED_STR); return _strVal; }
      char charVal() const { debug_assert(_type == TokenType::PTXT_SEG && _ptxtType == PtxtType::CHAR); return _charVal; }

      KeywordID keywordID() const { debug_assert(_type == TokenType::KEYWORD || isNumber(_type)); return _keyword; }
      OpData op() const { debug_assert(_type == TokenType::OP); return _op; }
      OpID opID() const { debug_assert(_type == TokenType::OP); return _op; }
      OpProps opProps() const { debug_assert(_type == TokenType::OP); return _op.props(); }
      BlockType blockType() const { debug_assert(_type == TokenType::BLOCK_DELIM); return _blockDelim.first; }
      BlockDelimRole blockDelimRole() const { debug_assert(_type == TokenType::BLOCK_DELIM); return _blockDelim.second; }
      PtxtType ptxtType() const { debug_assert(_type == TokenType::PTXT_SEG); return _ptxtType; }
};

namespace mcsl {
   File& write(File&, const clef::Token&);
   uint writef(File&, const clef::Token&, char mode, FmtArgs args);
};

#endif //TOKEN_HPP