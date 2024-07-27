#ifndef ENUMS_CPP
#define ENUMS_CPP

#include "ENUMS.hpp"
#include <utility>

constexpr auto operator+(const clef::TokenType t) noexcept { return std::to_underlying(t); }

#endif //ENUMS_CPP