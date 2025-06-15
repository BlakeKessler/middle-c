#include "map.hpp"
#include "io.hpp"
#include "str_slice.hpp"

void print(const mcsl::map<mcsl::str_slice, uint>& map) {
   mcsl::printf(FMT("%u\n"), map.size());
   for (auto it = map.begin(); it != map.end(); ++it) {
      mcsl::flush();
      mcsl::printf(FMT("map[\"%s\"] = %u\n"), it->first, it->second);
   }
   mcsl::printf(FMT("\n"));
   mcsl::printf(FMT("%s: %u\n"), map.contains(FMT("vwxyz")), map[FMT("vwxyz")]);
   mcsl::printf(FMT("%s: %u\n"), map.contains(FMT("ABCDE")), map[FMT("ABCDE")]);
   mcsl::printf(FMT("%s: %u\n"), map.contains(FMT("qwertyuiop")), map[FMT("qwertyuiop")]);
   mcsl::printf(FMT("%s\n"), map.contains(FMT("I'm not in here")));
}

int main() {
   mcsl::map<mcsl::str_slice, uint> map;
   map.emplace(mcsl::make_tuple((char*)("vwxyz"),5), mcsl::make_tuple(1));
   map.emplace(mcsl::make_tuple((char*)("ABCDE"),5), mcsl::make_tuple(2));
   map.emplace(mcsl::make_tuple((char*)("qwertyuiop"),10), mcsl::make_tuple(3));

   print(map);
}