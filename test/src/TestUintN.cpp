#include "uint_n.hpp"

#include <cstdlib>
#include <cstdio>

int main() {
   mcsl::uint_n<128> num{0};
   for (uint i = 0; i < 0xF; ++i) {
      uint x;
      num.print();

      x = std::rand();
      std::printf(" * %x = ", x);
      num *= x;
      num.print();
      std::printf("\n");

      x = std::rand();
      std::printf(" + %x = ", x);
      num += x;
      num.print();
      std::printf("\n\n");
   }
   num.print();
   std::printf("\n");
}