#include "carry.hpp"

#include <cstdlib>
#include <cstdio>

uint64 rand64() {
   return ((uint64)rand() << 32) | rand();
}

int main() {
   uint64 x1 = rand64();
   uint64 x2 = rand64();
   uint64 y;
   uint64 carry = mcsl::carry::MUL(x1, x2, &y);

   std::printf("%lx * %lx = %lx %lx\n", x1, x2, carry, y);
}