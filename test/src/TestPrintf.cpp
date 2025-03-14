#include "fs.hpp"

// #define TMP_FILE "/tmp/testwritef.txt"

int main() {
   mcsl::File mcslout = mcsl::File::ReopenLibcFile(stdout);
   
   mcslout.printf(mcsl::str_slice::make_from_cstr("%u %u %u %u %u\n"), 10, 124, 125325, 643, 0);

   // mcslout.writef<ulong>(0xA, 'u', {16, 15, 1, false, true, true});
   // mcslout.write('|');
   // mcslout.write('\n');
   // mcslout.writef<ulong>(0xA, 'u', {16, 5, 1, true, true, true});
   // mcslout.write('/');
   // mcslout.write('\n');
   
   // mcslout.writef<ulong>(0xA, 'u', {16, 4, 1, true, true, true});
   // mcslout.write('=');
   // mcslout.write('\n');
   // mcslout.writef<ulong>(0124, 'u', {8, 4, 3, true, false, true});
   // mcslout.write('|');
   // mcslout.write('\n');

   // mcslout.writef<ulong>(0x12AB325, 'u', {16});
   // mcslout.write('\n');
   // mcslout.writef<ulong>(0b1001001010101010110101010, 'u', {2});
   // mcslout.write('\n');

   // mcslout.flush(); //should be handled by the destructor
}