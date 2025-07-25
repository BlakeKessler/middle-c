#include <stdint.h>
#include <uchar.h>

//!FIXED WIDTH INTEGER TYPES
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef __uint128_t uint128;

typedef int8_t sint8;
typedef int16_t sint16;
typedef int32_t sint32;
typedef int64_t sint64;
typedef __int128_t sint128;

//!ARCHITECTURE-DEPENDANT INTEGER TYPES
typedef uint8 ubyte;
typedef uint16 ushort;
typedef uint32 uint;
typedef uint64 ulong;
typedef uint128 uoverlong;

typedef uintptr_t uptr;
typedef uint64 uword;

typedef sint8 sbyte;
typedef sint16 sshort;
typedef sint32 sint;
typedef sint64 slong;
typedef sint128 soverlong;

typedef intptr_t sptr;
typedef sint64 sword;

//!FIXED-WIDTH FLOATING POINT TYPES
typedef _Float16 float16;
typedef float float32;
typedef double float64;
typedef __float80 float80;
typedef __float128 float128;

typedef float16 half;
// typedef float32 float;
// typedef double32 double;
typedef float128 quad;

//!ARCHITECTURE-DEPENDANT FLOATING POINT TYPES
typedef float16 fshort;
typedef float64 flong;
typedef long double flext;

//!FIXED-WIDTH CHAR TYPES
typedef char8_t char8;
typedef char16_t char16;
typedef char32_t char32;

//!ARCHITECTURE-DEPENDANT CHAR TYPES
typedef __WCHAR_TYPE__ wchar;

//!user code
