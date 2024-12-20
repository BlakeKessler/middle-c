#include "str_to_num.hpp"

#include <cstdlib>
template<typename T> T rand() requires(sizeof(T) == sizeof(ulong)) {
   ulong bits = std::rand();
   bits <<= (8 * sizeof(uint));
   bits |= std::rand();

   return std::bit_cast<T>(bits);
}

#include <time.h>
ulong time() {
   struct timespec currTime;
   clock_gettime(CLOCK_REALTIME, &currTime);
   return currTime.tv_nsec;
}

#include "static_arr.hpp"
#include "raw_str.hpp"

int main() {
   mcsl::static_arr<mcsl::raw_str<16>, 0x40000> buf;
   for (uint i = 0; i < buf.size(); ++i) {
      std::sprintf(buf[i].data(), "%lx", rand<ulong>());
   }

   ulong mcslStart, mcslEnd;
   mcslStart = time();
   for (uint i = 0; i < buf.size(); ++i) {
      volatile ulong x = mcsl::str_to_uint(buf[i].data(), 16, 16);
   }
   mcslEnd = time();
   std::printf("mcsl::str_to_uint(): %lu nanoseconds\n", mcslEnd - mcslStart);
   
   ulong libcStart, libcEnd;
   libcStart = time();
   for (uint i = 0; i < buf.size(); ++i) {
      volatile ulong x = std::strtoul(buf[i].data(), nullptr, 16);
   }
   libcEnd = time();
   std::printf("libc::strtoul(): %lu nanoseconds\n", libcEnd - libcStart);


}