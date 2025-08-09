#ifndef PARSER_TEST
#define PARSER_TEST

#include "CodeGenerator.hpp"
#include "Parser.hpp"
#include "Lexer.hpp"
#include "dyn_arr.hpp"
#include "io.hpp"

// static const auto BIG_HEADER = mcsl::FMT("\033[;1m==============================================\n\033[0m");
// static const auto SMALL_HEADER = mcsl::FMT("\033[;1m----------------------------------------------\n\033[0m");

int main(const int argc, char** argv) {
   if (argc == 1) {
      mcsl::printf(FMT("No paths to files to parse provided."));
      return EXIT_FAILURE;
   }
   //parse
   clef::SyntaxTree tree{};
   clef::Parser::parse(mcsl::str_slice::make_from_cstr(argv[1]), tree);
   // mcsl::err_printf(mcsl::FMT("\033[1m%s:\033[22m\n%s%s\n%s\n\n"), FMT(argv[1]), SMALL_HEADER, tree, BIG_HEADER);
   // mcsl::stderr.flush();

   //run codegen and print it to stdout
   clef::CodeGenerator::gen(tree, mcsl::stdout);

   return EXIT_SUCCESS;
}

#endif //PARSER_TEST