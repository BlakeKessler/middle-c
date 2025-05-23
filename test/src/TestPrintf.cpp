#include "io.hpp"

// #define TMP_FILE "/tmp/testwritef.txt"
#define FMT mcsl::FMT

int main() {
   mcsl::printf(FMT("%#u %u %u %u %u\n"), 10, 124, 125325, 643, 0);
   mcsl::printf(FMT("%r %R %r %r\n%#r\n"), 10, 124, 125325, 643, 0);

   mcsl::printf(FMT("|%+ 15.1,16u|%-+ 15.1,16u|\n"), 0xAu, 0xAu);
   
   mcsl::printf(FMT("%-+ 4.1,16u=\n%- 4.3,8u|\n"), 0xA, 0124);

   mcsl::printf(FMT("%,16u\n%,2u\n"), 0x12AB325, 0b1001001010101010110101010);

   mcsl::printf(FMT("%.10,16u\n%#,2u\n"), 0x12AB325, 0b1001001010101010110101010);

   mcsl::printf(FMT("\n|%%|\n\n"));



   mcsl::printf(FMT("%f\n"), 1.0);
   mcsl::printf(FMT("%f\n"), 10.0);
   mcsl::printf(FMT("%.6f\n"), 0.01);
   mcsl::printf(FMT("%.0f\n"), 0.9);
   mcsl::printf(FMT("%.6e\n"), 1.0);
   mcsl::printf(FMT("%.6e\n"), 0.0);
   mcsl::printf(FMT("%.6e\n"), -1.0);
   mcsl::printf(FMT("%.0e\t|\t%.6e\n"), 0.9, 0.9);
   mcsl::printf(FMT("%.6e\n"), -11.0);
   mcsl::printf(FMT("|%20.6e|\n"), 10.0);
   mcsl::printf(FMT("|%-20.6e|\n"), 100.0);
   mcsl::printf(FMT("|%+20.6e|\n"), 0.1);
   mcsl::printf(FMT("|%020.6e|\n"), 9.9);
   mcsl::printf(FMT("%#.6e\n"), 3.0);
   mcsl::printf(FMT("%#.6e\n"), -33759071367523108756780.0);
   mcsl::printf(FMT("%#.6,16e\n"), 15.0);
   mcsl::printf(FMT("%#.6,16e\n"), 17.0);
   mcsl::printf(FMT("%#.6e\n"), -33759071367523108175691837658926580236573266899858265986328567826582365879365872365897326578756780.0);
   mcsl::printf(FMT("%#.6e\n"), -337590713675231081756918376589265802365732668998582659863285678265823658792658792365789623589762389756287565982365872365897326578756780.0);
   mcsl::printf(FMT("%#.6,2e\n"), -33759071367523108756780.0);
   mcsl::printf(FMT("%#.6,8e\n"), -33759071367523108756780.0);

   mcsl::printf(FMT("%f | %f | %f\n"), mcsl::Inf, -mcsl::Inf, mcsl::NaN);

   mcsl::printf(FMT("\n|%s|\n%c%c\n"), mcsl::str_slice::make_from_cstr("ayo this thing on?"), ':', ')');

   // mcsl::flush(); //should be handled by the destructor
}