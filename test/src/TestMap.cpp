#include "map.hpp"
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
   mcsl::map<mcsl::str_slice, uint, hash, eq> map;
   map.emplace(mcsl::make_tuple((char*)("vwxyz"),5), mcsl::make_tuple(1));
   map.emplace(mcsl::make_tuple((char*)("ABCDE"),5), mcsl::make_tuple(2));
   map.emplace(mcsl::make_tuple((char*)("qwertyuiop"),10), mcsl::make_tuple(3));

   mcsl::printf(FMT("%s: %u\n"), map.contains(FMT("vwxyz")), map[FMT("vwxyz")]);
   mcsl::printf(FMT("%s: %u\n"), map.contains(FMT("ABCDE")), map[FMT("ABCDE")]);
   mcsl::printf(FMT("%s: %u\n"), map.contains(FMT("qwertyuiop")), map[FMT("qwertyuiop")]);
   mcsl::printf(FMT("%s\n"), map.contains(FMT("I'm not in here")));
}