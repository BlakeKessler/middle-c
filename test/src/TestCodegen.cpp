#ifndef PARSER_TEST
#define PARSER_TEST

#include "CodeGenerator.hpp"
#include "Parser.hpp"
#include "Lexer.hpp"
#include "dyn_arr.hpp"
#include "io.hpp"

int main(const int argc, char** argv) {
   if (argc == 1) {
      mcsl::printf(FMT("No paths to files to parse provided."));
      return EXIT_FAILURE;
   }
   //parse
   clef::SyntaxTree tree{};
   clef::Parser::parse(mcsl::str_slice::make_from_cstr(argv[1]), tree);
   //output generated code to stdout
   clef::CodeGenerator::gen(tree, mcsl::stdout);

   return EXIT_SUCCESS;
}

#endif //PARSER_TEST