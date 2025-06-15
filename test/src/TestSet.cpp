#include "set.hpp"
#include "io.hpp"
#include "str_slice.hpp"

void print(const mcsl::set<mcsl::str_slice>& set) {
   mcsl::printf(FMT("%s\n"), set.contains(FMT("vwxyz")));
   mcsl::printf(FMT("%s\n"), set.contains(FMT("ABCDE")));
   mcsl::printf(FMT("%s\n"), set.contains(FMT("qwertyuiop")));
   mcsl::printf(FMT("%s\n"), set.contains(FMT("I'm not in here")));
}

int main() {
   mcsl::set<mcsl::str_slice> set;
   set.emplace(FMT("vwxyz"));
   set.emplace(FMT("ABCDE"));
   set.emplace(FMT("qwertyuiop"));

   print(set);
}