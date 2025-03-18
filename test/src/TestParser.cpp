#ifndef PARSER_TEST
#define PARSER_TEST

#include "Parser.hpp"
#include "Lexer.hpp"
#include "dyn_arr.hpp"
#include "io.hpp"

static const auto BIG_HEADER = mcsl::FMT("\033[;1m=======================\n\033[0m");
static const auto SMALL_HEADER = mcsl::FMT("\033[;1m-----------------------\n\033[0m");

int main(const int argc, char** argv) {
   if (argc < 2) {
      mcsl::printf(mcsl::FMT("No file path provided. Exiting.\n"));
      return EXIT_FAILURE;
   }

   //read and tokenize file
   mcsl::printf(mcsl::FMT("%s"), BIG_HEADER);
   clef::SourceTokens tokens = clef::Lexer::LexFile(argv[1]);
   mcsl::printf(mcsl::FMT("\033[1mTokens:\033[22m\n%s"), SMALL_HEADER);
   tokens.printf();
   mcsl::printf(mcsl::FMT("\n%s"),BIG_HEADER);

   //abstract syntax tree
   clef::SyntaxTree tree{};
   clef::Parser::parse(tokens, tree);
   mcsl::printf(mcsl::FMT("\033[1mAbstract Syntax Tree:\033[22m\n%s"), SMALL_HEADER);
   tree.printf();
   mcsl::printf(mcsl::FMT("\n%s\n"), BIG_HEADER);

   //offer to print debug info
   mcsl::printf(mcsl::FMT("quit (q); print parser debug info (p): "));
   char ch = mcsl::get();
   mcsl::printf(mcsl::FMT("\n%s\n"), BIG_HEADER);
   switch (ch) {
      case 'q':
         break;
      case 'p':
         tree.print();
         mcsl::printf(mcsl::FMT("\n%s\n"), BIG_HEADER);
         break;
      default:
         break;
   }

   return EXIT_SUCCESS;
}

#endif //PARSER_TEST