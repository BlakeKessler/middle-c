#include "io.hpp"
#include "list.hpp"
#include "rand.hpp"

#define PRINT(n) \
for (ubyte elem : list##n) {         \
   mcsl::printf(FMT("%u\n"), elem); \
}                                   \
mcsl::printf(FMT("\n")); mcsl::flush()

int main() {
   mcsl::list<ubyte> list1{};
   for (ubyte i = 0; i < 20; ++i) {
      list1.emplace_back((ubyte)(mcsl::rand() & 0xF));
   }

   // mcsl::printf(FMT("%u\n\n"), list1.size());
   
   PRINT(1);
   
   list1.sort();
   
   PRINT(1);

   list1.unique();

   PRINT(1);

   list1.reverse();

   PRINT(1);
   
   mcsl::list<ubyte> list2{};
   for (ubyte i = 0; i < 10; ++i) {
      list2.push_back((ubyte)mcsl::rand());
   }

   PRINT(2);

   list1.splice(list1.end(), list2);

   PRINT(1);

   list1.reverse().sort();

   PRINT(1);

   mcsl::list<ubyte> list3{};
   for (ubyte i = 0; i < 10; ++i) {
      list3.push_back((ubyte)mcsl::rand());
   }
   list3.sort();

   PRINT(3);

   mcsl::list<ubyte> list4{};
   for (ubyte i = 0; i < 10; ++i) {
      list4.push_back((ubyte)mcsl::rand());
   }
   list4.sort();

   PRINT(4);

   list3.merge(list4);

   PRINT(3);
}