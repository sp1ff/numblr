#include <cstring>
#include <iomanip>
#include <iostream>
#include <ext/codecvt_specializations.h>
using namespace std;
int main() {
  // The usual input text, "one euro" in Windows-1252...
  const char text[] = "1€";
  // a new type of codecvt facet specialized on encoding_state...
  typedef codecvt<char, char, encoding_state> my_codecvt;
  // create a locale from the global locale, with my code conversion
  // facet swapped in...
  locale loc(locale(""), new my_codecvt);
  // & grab a reference to it.
  const my_codecvt &cvt = use_facet<my_codecvt>(loc);

  cout << "              bytes:";
  for (size_t i = 0, n = sizeof(text); i < n; ++i) {
    cout << " " << hex << setw(2) << setfill('0') << 
      static_cast<unsigned int>(static_cast<unsigned char>(text[i]));
  }
  cout << endl;

  cout << "             noconv: " << cvt.always_noconv() << endl;
  cout << "       sizeof(text): " << sizeof(text) << endl;

  // internal is Windows-1252, external is UTF-8
  my_codecvt::state_type state("WINDOWS-1252", "UTF-8");

  char out[5] = { 0 };
  char *outn;
  const char *fromn;
  codecvt_base::result r = cvt.out(state, text, text + sizeof(text), 
				   fromn,
				   out, out + 5, 
				   outn);
  cout << "codecvt_base status: " << r << endl;
  cout << "              bytes:";
  for (size_t i = 0, n = sizeof(out); i < n; ++i) {
    cout << " " << hex << setw(2) << setfill('0') << 
      static_cast<unsigned int>(static_cast<unsigned char>(out[i]));
  }
  cout << endl;
  return 0;
}
