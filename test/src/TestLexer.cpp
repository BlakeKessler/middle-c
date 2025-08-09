#ifndef TOKENIZER_TEST
#define TOKENIZER_TEST

#include "Lexer.hpp"

#include "io.hpp"

#define FMT mcsl::FMT

int main(int argc, char** argv) {
   //check that file was provided
   if (argc < 2) {
      mcsl::err_printf(FMT("No file path provided\n"));
      return EXIT_FAILURE;
   }

   //read file
   clef::Source src = clef::Source::readFile(FMT(argv[1]));

   //print file contents
   const mcsl::str_slice fileDelim = FMT("----------------------\n");
   mcsl::printf(FMT("Printing \"%s\" (%u lines, %u chars)\n%s"), FMT(argv[1]), src.lineCount(), src.buf().size(), fileDelim);
   for (uint i = 0; i < src.lineCount(); ++i) {
      mcsl::printf(FMT("line %u: %s"), i + 1, src.line(i));
   }
   mcsl::printf(FMT("\n%sDone printing file.\n"), fileDelim);


   //tokenize
   mcsl::printf(FMT("\nLexing file.\n"));
   clef::Lexer lexer(std::move(src));

   //print tokens
   mcsl::printf(FMT("\nPrinting tokens\n%s"), fileDelim);
   while (!lexer.done()) {
      mcsl::printf(FMT("%s\n"), lexer.nextToken());
   }
   mcsl::printf(FMT("%sDone printing tokens.\n"), fileDelim);
}

#endif //TOKENIZER_TEST