#ifndef PARSER_TEST
#define PARSER_TEST

#include "Parser.hpp"
#include "Lexer.hpp"
#include "dyn_arr.hpp"
#include "io.hpp"

static const auto BIG_HEADER = mcsl::FMT("\033[;1m==============================================\n\033[0m");
static const auto SMALL_HEADER = mcsl::FMT("\033[;1m----------------------------------------------\n\033[0m");

int main(const int argc, char** argv) {
   if (argc == 1) {
      mcsl::printf(FMT("No paths to files to parse provided."));
   }
   mcsl::printf(BIG_HEADER);
   for (int i = 1; i < argc; ++i) {
      //abstract syntax tree
      clef::SyntaxTree tree{clef::DataModel::LP64};
      clef::Parser::parseFile(mcsl::str_slice::make_from_cstr(argv[i]), tree);
      mcsl::printf(mcsl::FMT("\033[1m%s:\033[22m\n%s%s\n%s"), FMT(argv[i]), SMALL_HEADER, tree, BIG_HEADER);
   }

   return EXIT_SUCCESS;
}

#endif //PARSER_TEST