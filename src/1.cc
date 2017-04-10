#include <cstring>
#include <iostream>
using namespace std;
int main() {
  const char text[] = "1€";
  cout << "sizeof(char): " << sizeof(char) << endl;
  cout << "        text: " << text         << endl;
  cout << "sizeof(text): " << sizeof(text) << endl;
  cout << "strlen(text): " << strlen(text) << endl;
  cout << " text(bytes):" << hex;
  for (size_t i = 0, n = strlen(text); i < n; ++i) {
    cout << " " << static_cast<unsigned>(static_cast<unsigned char>(text[i]));
  }
  cout << endl;
  return 0;
}
