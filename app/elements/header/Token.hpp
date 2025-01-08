#pragma once
#ifndef TOKEN_HPP
#define TOKEN_HPP

#include "CLEF.hpp"
#include "TokenData.hpp"
#include "raw_str_span.hpp"
#include "pair.hpp"

struct clef::Token {
   private:
      union {
         const mcsl::raw_str_span _name; //identifier
         ulong _intVal;
         double _realVal;
         const mcsl::raw_str_span _strVal;
         char _charVal;
      };
      union {
         KeywordID _keyword;
         OpID _op;
         mcsl::pair<BlockType,BlockDelimRole> _blockDelim;
         PtxtType _ptxtType;
      };
      TokenType _type;

      static constexpr const mcsl::raw_str _nameof = "Token";
   public:
      static constexpr const auto& nameof() { return _nameof; }

      //constructors
      Token():_type{TokenType::NONE} {}
      Token(const mcsl::raw_str_span name):_name{name},_type{TokenType::IDEN} {}
      Token(KeywordID id):_keyword{id},_type{TokenType::KEYWORD} {}
      Token(ulong val):_intVal{val},_type{TokenType::INT_NUM} {}
      Token(double val):_realVal{val},_type{TokenType::REAL_NUM} {}
      Token(OpID id):_op{id},_type{TokenType::OP} {}
      Token(BlockType type, BlockDelimRole role):_blockDelim{mcsl::pair{type,role}},_type{TokenType::BLOCK_DELIM} {}
      Token(const mcsl::raw_str_span val, PtxtType type):_strVal{val},_ptxtType{type},_type{TokenType::PTXT_SEG} {}
      Token(char c):_charVal{c},_ptxtType{PtxtType::CHAR},_type{TokenType::PTXT_SEG} {}
      Token(TokenType type):_type{type} { debug_assert(_type == TokenType::PREPROC_INIT || _type == TokenType::PREPROC_EOS || _type == TokenType::EOS || _type == TokenType::ESC); } //intended for PREPROC_INIT, PREPROC_EOS, EOS, ESC only


      //getters
      TokenType type() const { return _type; }
      
      const mcsl::raw_str_span& name() const { debug_assert(_type == TokenType::IDEN); return _name; }
      ulong intVal() const { debug_assert(_type == TokenType::INT_NUM); return _intVal; }
      double realVal() const { debug_assert(_type == TokenType::REAL_NUM); return _realVal; }
      const mcsl::raw_str_span& strVal() const { debug_assert(_type == TokenType::PTXT_SEG && _ptxtType == PtxtType::STR); return _strVal; }
      char charVal() const { debug_assert(_type == TokenType::PTXT_SEG && _ptxtType == PtxtType::CHAR); return _charVal; }

      KeywordID keywordID() const { debug_assert(_type == TokenType::KEYWORD); return _keyword; }
      OpID opID() const { debug_assert(_type == TokenType::OP); return _op; }
      BlockType blockType() const { debug_assert(_type == TokenType::BLOCK_DELIM); return _blockDelim.first; }
      BlockDelimRole blockDelimRole() const { debug_assert(_type == TokenType::BLOCK_DELIM); return _blockDelim.second; }
      PtxtType ptxtType() const { debug_assert(_type == TokenType::PTXT_SEG); return _ptxtType; }
};

#endif //TOKEN_HPP