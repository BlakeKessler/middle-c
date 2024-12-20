#include "SyntaxTree.hpp"
#include "Lexer.hpp"


#include <cstdio>

#define BIG_HEADER "\033[;1m=======================\n\033[0m"
#define SMALL_HEADER "\033[;1m-----------------------\n\033[0m"


int main(const int argc, char** argv) {
   if (argc < 2) {
      std::printf("No file path provided. Exiting.\n");
      return EXIT_FAILURE;
   }

   //read and tokenize file
   printf("%s", BIG_HEADER);
   clef::SourceTokens tokens = clef::Lexer::LexFile(argv[1]);
   std::printf("\033[1mTokens:\033[22m\n%s", SMALL_HEADER);
   tokens.printf();
   std::printf("\n%s\n", BIG_HEADER);


   clef::SyntaxTree tree{std::move(tokens.tokens()),tokens.lines()};
   std::printf("\n%s\033[1mAbstract Syntax Tree:\033[22m\n%s", BIG_HEADER, SMALL_HEADER);
   tree.printf();
   std::printf("\n%s\n", BIG_HEADER);
   

   return EXIT_SUCCESS;
}