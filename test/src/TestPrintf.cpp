#include "io.hpp"

// #define TMP_FILE "/tmp/testwritef.txt"
#define FMT mcsl::FMT

int main() {
   mcsl::File mcslout = mcsl::File::ReopenLibcFile(stdout);
   
   mcslout.printf(FMT("%#u %u %u %u %u\n"), 10, 124, 125325, 643, 0);
   mcslout.printf(FMT("%r %R %r %r\n%#r\n"), 10, 124, 125325, 643, 0);

   mcslout.printf(FMT("|%+ 15.1,16u|%-+ 15.1,16u|\n"), 0xAu, 0xAu);
   
   mcslout.printf(FMT("%-+ 4.1,16u=\n%- 4.3,8u|\n"), 0xA, 0124);

   mcslout.printf(FMT("%,16u\n%,2u\n"), 0x12AB325, 0b1001001010101010110101010);

   mcslout.printf(FMT("%.10,16u\n%#,2u\n"), 0x12AB325, 0b1001001010101010110101010);

   mcslout.printf(FMT("\n|%%|\n\n"));



   mcslout.printf(FMT("%f\n"), 1.0);
   mcslout.printf(FMT("%f\n"), 10.0);
   mcslout.printf(FMT("%.6f\n"), 0.01);
   mcslout.printf(FMT("%.0f\n"), 0.9);
   mcslout.printf(FMT("%.6e\n"), 1.0);
   mcslout.printf(FMT("%.6e\n"), 0.0);
   mcslout.printf(FMT("%.6e\n"), -1.0);
   mcslout.printf(FMT("%.0e\t|\t%.6e\n"), 0.9, 0.9);
   mcslout.printf(FMT("%.6e\n"), -11.0);
   mcslout.printf(FMT("|%20.6e|\n"), 10.0);
   mcslout.printf(FMT("|%-20.6e|\n"), 100.0);
   mcslout.printf(FMT("|%+20.6e|\n"), 0.1);
   mcslout.printf(FMT("|%020.6e|\n"), 9.9);
   mcslout.printf(FMT("%#.6e\n"), 3.0);
   mcslout.printf(FMT("%#.6e\n"), -33759071367523108756780.0);
   mcslout.printf(FMT("%#.6,16e\n"), 15.0);
   mcslout.printf(FMT("%#.6,16e\n"), 17.0);
   mcslout.printf(FMT("%#.6e\n"), -33759071367523108175691837658926580236573266899858265986328567826582365879365872365897326578756780.0);
   mcslout.printf(FMT("%#.6e\n"), -337590713675231081756918376589265802365732668998582659863285678265823658792658792365789623589762389756287565982365872365897326578756780.0);
   mcslout.printf(FMT("%#.6,2e\n"), -33759071367523108756780.0);
   mcslout.printf(FMT("%#.6,8e\n"), -33759071367523108756780.0);

   mcslout.printf(FMT("%f | %f | %f\n"), mcsl::Inf, -mcsl::Inf, mcsl::NaN);

   mcslout.printf(FMT("\n|%s|\n%c%c\n"), mcsl::str_slice::make_from_cstr("ayo this thing on?"), ':', ')');

   // mcslout.flush(); //should be handled by the destructor
}