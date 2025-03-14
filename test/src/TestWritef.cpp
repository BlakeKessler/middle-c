#include "fs.hpp"

// #define TMP_FILE "/tmp/testwritef.txt"

int main() {
   mcsl::File mcslout = mcsl::File::ReopenLibcFile(stdout);
   mcslout.writef<ulong>(10, 'u', {10});
   mcslout.write('\n');
   mcslout.writef<ulong>(124, 'u', {10});
   mcslout.write('\n');
   mcslout.writef<ulong>(125325, 'u', {10});
   mcslout.write('\n');
   mcslout.writef<ulong>(643, 'u', {10});
   mcslout.write('\n');
   mcslout.writef<ulong>(0, 'u', {10});
   mcslout.write('\n');

   mcslout.writef<ulong>(0xA, 'u', {16, 15, 1, false, true, true});
   mcslout.write('|');
   mcslout.write('\n');
   mcslout.writef<ulong>(0xA, 'u', {16, 5, 1, true, true, true});
   mcslout.write('/');
   mcslout.write('\n');
   
   mcslout.writef<ulong>(0xA, 'u', {16, 4, 1, true, true, true});
   mcslout.write('=');
   mcslout.write('\n');
   mcslout.writef<ulong>(0124, 'u', {8, 4, 3, true, false, true});
   mcslout.write('|');
   mcslout.write('\n');

   mcslout.writef<ulong>(0x12AB325, 'u', {16});
   mcslout.write('\n');
   mcslout.writef<ulong>(0b1001001010101010110101010, 'u', {2});
   mcslout.write('\n');

   // mcslout.flush(); //should be handled by the destructor
}