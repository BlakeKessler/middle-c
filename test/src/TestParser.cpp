#ifndef PARSER_TEST
#define PARSER_TEST

#include "Parser.hpp"
#include "Lexer.hpp"
#include "dyn_arr.hpp"
#include <cstdio>

#define BIG_HEADER "\033[;1m=======================\n\033[0m"
#define SMALL_HEADER "\033[;1m-----------------------\n\033[0m"


int main(const int argc, char** argv) {
   if (argc < 2) {
      std::printf("No file path provided. Exiting.\n");
      return EXIT_FAILURE;
   }

   //read and tokenize file
   std::printf("%s", BIG_HEADER);
   clef::SourceTokens tokens = clef::Lexer::LexFile(argv[1]);
   std::printf("\033[1mTokens:\033[22m\n%s", SMALL_HEADER);
   tokens.printf();
   std::printf("\n%s",BIG_HEADER);

   //abstract syntax tree
   clef::SyntaxTree tree = clef::Parser::parse(tokens);
   std::printf("\033[1mAbstract Syntax Tree:\033[22m\n%s", SMALL_HEADER);
   tree.printf();
   std::printf("\n%s\n", BIG_HEADER);

   //offer to print debug info
   std::printf("quit (q); print parser debug info (p): ");
   char ch = std::getchar();
   std::printf("\n%s\n", BIG_HEADER);
   switch (ch) {
      case 'q':
         break;
      case 'p':
         tree.print();
         std::printf("\n%s\n", BIG_HEADER);
         break;
      default:
         break;
   }

   return EXIT_SUCCESS;
}

#endif //PARSER_TEST