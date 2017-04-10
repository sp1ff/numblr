#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
using namespace std;
int main() {
  const char text[] = u8"едц";
  cout << "     sizeof(char): " << sizeof(char) << endl;
  cout << "             text: " << text         << endl;
  cout << "     sizeof(text): " << sizeof(text) << endl;
  cout << "     strlen(text): " << strlen(text) << endl;
  cout << "      text(bytes):"  << hex;
  for (size_t i = 0, n = sizeof(text); i < n; ++i) {
    cout << " " << static_cast<unsigned int>(static_cast<unsigned char>(text[i]));
  }
  cout << endl << endl;

  const char16_t text16[] = u"едц";
  cout << " sizeof(char16_t): " << sizeof(char16_t) << endl;
  cout << "   sizeof(text16): " << sizeof(text16)   << endl;
  cout << "      text(bytes):"  << hex;
  for (size_t i = 0, n = sizeof(text16) / sizeof(char16_t); i < n; ++i) {
    cout << " " << setw(4) << setfill('0') <<
      static_cast<unsigned int>(static_cast<char16_t>(text16[i]));
  }
  cout << endl << endl;

  const char32_t text32[] = U"едц";
  cout << " sizeof(char32_t): " << sizeof(char32_t) << endl;
  cout << "   sizeof(text32): "   << sizeof(text32)   << endl;
  cout << "      text(bytes):"    << hex;
  for (size_t i = 0, n = sizeof(text32) / sizeof(char32_t); i < n; ++i) {
    cout << " " << setw(8) << setfill('0') << 
      static_cast<unsigned int>(static_cast<char32_t>(text32[i]));
  }
  cout << endl;
  return 0;
}
