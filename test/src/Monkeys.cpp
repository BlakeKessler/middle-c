// Infinite Monkeys Compiler Fuzzer

// arguments:
// * n: number of programs to generate
// * l: length of each program (in bytes)

#define N 0
#define L 1

#include "CLEF.hpp"
#include "str_slice.hpp"

sint main(sint argc, char** argv) {
   uint args[2];
   args[N] = 0;
   args[L] = 0x100;

   sint curr = -1;
   for (sint i = 1; i < argc; ++i) {
      const mcsl::str_slice arg = FMT(argv[i]);
      if (curr < 0) {
         if (arg.size() != 2 || arg[0] != '-') {
            return EXIT_FAILURE;
         }
         switch (arg[1]) {
            case 'n': curr = N; break;
            case 'l': curr = L; break;
            default: return EXIT_FAILURE;
         }
      } else {
         debug_assert(curr >= 0 && curr < (sizeof(args) / sizeof(args[0])));
         args[curr] = mcsl::str_to_uint(arg.begin(), arg.size());
      }
   }

   if (curr >= 0) {
      return EXIT_FAILURE;
   }

   
   while (args[N]--) {
      TODO;
      //generate source code of `args[L]` printable chars

      //run the compiler on it and capture the return code

      //indicate to the user whether the compiler encountered any internal errors
   }
   return EXIT_SUCCESS;
}