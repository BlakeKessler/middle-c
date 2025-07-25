#ifndef PARSER_TEST
#define PARSER_TEST

#include "CodeGenerator.hpp"
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
      clef::SyntaxTree tree{};
      clef::Parser::parse(mcsl::str_slice::make_from_cstr(argv[i]), tree);
      mcsl::printf(mcsl::FMT("\033[1m%s:\033[22m\n%s%s\n%s\n"), FMT(argv[i]), SMALL_HEADER, tree, SMALL_HEADER);
      clef::CodeGenerator::gen(tree, mcsl::stdout);
      mcsl::printf(FMT("\n%s"), BIG_HEADER);
   }

   return EXIT_SUCCESS;
}

#endif //PARSER_TEST