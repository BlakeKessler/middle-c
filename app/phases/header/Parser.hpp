#pragma once
#ifndef PARSER_HPP
#define PARSER_HPP

#include "CLEF.hpp"
#include "SourceTokens.hpp"
#include "SyntaxTree.hpp"

#include "OperatorData.hpp"
#include "KeywordData.hpp"

#include "alloc.hpp"

class clef::Parser {
   private:
      static constexpr const mcsl::raw_str _nameof = "Parser";

      //helper functions
      static char parseCharLitASCII(const char* begin, const char* end);
      static mcsl::string parseStrLitASCII(const char* begin, const char* end);
      static bool parseNumLit(SyntaxTree& tree, const Token*& tokIt, const Token* endtok);

      static BlockType consumeBlock(SyntaxTree& tree, const Token*& tokIt, const Token* endtok);
      static bool consumeSpecificBlock(const BlockType type, SyntaxTree& tree, const Token*& tokIt, const Token* endtok);
      static bool consumeEnclosedStmts(SyntaxTree& tree, const Token*& tokIt, const Token* endtok); //brace-enclosed statement sequence

      static bool consumeIdentifier(SyntaxTree& tree, const Token*& tokIt, const Token* endtok); //!NOTE: to handle identifiers (NOT KEYWORDS!) with scope resolution
      static bool consumeType(SyntaxTree& tree, const Token*& tokIt, const Token* endtok);

      static bool consumeExpression(SyntaxTree& tree, const Token*& tokIt, const Token* endtok); //greedy
      static bool consumeStatement(SyntaxTree& tree, const Token*& tokIt, const Token* endtok);

      //error logging
      template<bool isImmediatelyFatal> static bool logError(const clef::ErrCode code, const char* formatStr, auto&&... args);
   public:
      static constexpr const auto& nameof() { return _nameof; }

      //parse tokenized source code
      static SyntaxTree parse(const SourceTokens& src);
};


#pragma region src
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
template<bool isImmediatelyFatal> bool clef::Parser::logError(const clef::ErrCode code, const char* formatStr, auto&&... args) {
   if constexpr (isImmediatelyFatal) {
      clef::throwError(code, formatStr, std::forward<decltype(args)>(args)...);
   } else {
      std::printf("\033[35;1;4mCLEF PARSE ERROR:\033[0m");
      std::printf(formatStr, args...);
      std::printf("\n");
   }
   return true;
}
#pragma GCC diagnostic pop
#pragma endregion src

#endif //PARSER_HPP