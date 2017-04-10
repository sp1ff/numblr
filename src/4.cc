#include <fstream>
#include <iostream>
#include <iomanip>
#include <boost/locale.hpp>
using namespace boost::locale;
using namespace std;
int main() {
  wchar_t text[] = L"1€";

  cout << "sizeof(wchar_t): " << sizeof(wchar_t) << endl;
  cout << "           text: " << text         << endl;
  cout << "   sizeof(text): " << sizeof(text) << endl;
  cout << "   wcslen(text): " << wcslen(text) << endl;
  cout << "    text(bytes):" << hex;
  for (size_t i = 0, n = wcslen(text); i < n; ++i) {
    cout << " " << setw(4) << setfill('0') << static_cast<unsigned>(text[i]);
  }
  cout << endl;

  locale loc = generator().generate("en_US.UTF-8");

  wofstream ofs;
  ofs.imbue(loc);
  ofs.open("4.txt");
  ofs << text << endl;
  
  return 0;
}
