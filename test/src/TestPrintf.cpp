#include "fs.hpp"

// #define TMP_FILE "/tmp/testwritef.txt"
#define _FMT_ mcsl::str_slice::make_from_cstr

int main() {
   mcsl::File mcslout = mcsl::File::ReopenLibcFile(stdout);
   
   mcslout.printf(_FMT_("%#u %u %u %u %u\n"), 10, 124, 125325, 643, 0);
   mcslout.printf(_FMT_("%r %R %r %r\n%#r\n"), 10, 124, 125325, 643, 0);

   mcslout.printf(_FMT_("|%+ 15.1,16u|%-+ 15.1,16u|\n"), 0xAu, 0xAu);
   
   mcslout.printf(_FMT_("%-+ 4.1,16u=\n%- 4.3,8u|\n"), 0xA, 0124);

   mcslout.printf(_FMT_("%,16u\n%,2u\n"), 0x12AB325, 0b1001001010101010110101010);

   mcslout.printf(_FMT_("%.10,16u\n%#,2u\n"), 0x12AB325, 0b1001001010101010110101010);

   // mcslout.flush(); //should be handled by the destructor
}