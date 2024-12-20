#include "str_to_num.hpp"

#include <cstring>
#include <cstdio>
#include <iostream>
#include <format>

char buf[0xFFFF];

bool testNum(ulong num) {
   ulong parsedNum = mcsl::str_to_uint(buf, std::sprintf(buf, "%lu", num));
   if (num != parsedNum) {
      std::printf("\t%16lu != %-16lu (%s)\n", num, parsedNum, std::format("{:064b}", std::bit_cast<ulong>(num) ^ std::bit_cast<ulong>(parsedNum)).data());
      return false;
   }
   return true;
}
bool testNum(slong num) {
   slong parsedNum = mcsl::str_to_sint(buf, std::sprintf(buf, "%ld", num));
   if (num != parsedNum) {
      std::printf("\t%16ld != %-16ld (%s)\n", num, parsedNum, std::format("{:064b}", std::bit_cast<ulong>(num) ^ std::bit_cast<ulong>(parsedNum)).data());
      return false;
   }
   return true;
}
bool testNum(double num) {
   double parsedNum = mcsl::str_to_real(buf, std::sprintf(buf, "%lf", num));
   num = std::atof(buf);
   if (num != parsedNum) {
      std::printf("\t%24la != %-24la (%s)\n", num, parsedNum, std::format("{:064b}", std::bit_cast<ulong>(num) ^ std::bit_cast<ulong>(parsedNum)).data());
      return false;
   } else {
      std::printf("\t%24la == %-24la\n", num, parsedNum);
   }
   return true;
}


#include <cstdlib>
template<typename T> T rand() requires(sizeof(T) == sizeof(ulong)) {
   ulong bits = std::rand();
   bits <<= (8 * sizeof(uint));
   bits |= std::rand();

   return std::bit_cast<T>(bits);
}

#include <ctime>
#include <typeinfo>
template<typename T> uint testType(uint count, uint seed = std::time(nullptr)) {
   std::srand(seed);
   {
      T a;
      std::cout << typeid(a).name() << ":\n\x1b[31m";
   }
   
   uint failures = 0;
   for (uint i = 0; i < count; ++i) {
      T num = rand<T>();
      if (!testNum(num)) {
         ++failures;
      }
   }

   std::cout << "\x1b[39m\nfailures: ";
   if (failures) {
      std::cout << "\x1b[31m";
   } else {
      std::cout << "\x1b[32m";
   }
   std::cout << failures << "\x1b[39m" << std::endl << std::endl;

   return failures;
}

int main() {
   uint failures = 0;
   failures += testType<ulong>(0x10, 1);
   failures += testType<slong>(0x10, 1);
   failures += testType<double>(0x10, 1);

   std::cout << "total failures: ";
   if (failures) {
      std::cout << "\x1b[31m";
   } else {
      std::cout << "\x1b[32m";
   }
   std::cout << failures << "\x1b[39m" << std::endl;

   return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}