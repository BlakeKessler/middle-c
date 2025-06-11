#include "set.hpp"
#include "io.hpp"
#include "str_slice.hpp"

ulong hash(const mcsl::str_slice& str) {
   constexpr uint p = 31;
   constexpr uint m = 1e9 + 9;
   ulong hash_value = 0;
   ulong p_pow = 1;
   for (char c : str) {
      hash_value = (hash_value + (c - 'a' + 1) * p_pow) % m;
      p_pow = (p_pow * p) % m;
   }
   return hash_value;
}
bool eq(const mcsl::str_slice& lhs, const mcsl::str_slice& rhs) {
   return lhs == rhs;
}

int main() {
   mcsl::set<mcsl::str_slice, hash, eq> set;
   set.emplace(FMT("vwxyz"));
   set.emplace(FMT("ABCDE"));
   set.emplace(FMT("qwertyuiop"));

   mcsl::printf(FMT("%s\n"), set.contains(FMT("vwxyz")));
   mcsl::printf(FMT("%s\n"), set.contains(FMT("ABCDE")));
   mcsl::printf(FMT("%s\n"), set.contains(FMT("qwertyuiop")));
   mcsl::printf(FMT("%s\n"), set.contains(FMT("I'm not in here")));
}