#include <string>
#include <iostream>
// Use only the neeeded aspects of each namespace
using std::string;
using std::to_string;
using std::endl;
using std::cerr;
using std::cin;
using std::cout;

int main() {
  int a = 2;
  int b = 4;
  int counter = 0;
  counter = b;
  int base = 0;
  base = a;
  int product = 0;
  product = 1;
  int modulus = 0;

  while (counter > 0) {
    modulus = counter % 2;
    if (modulus == 1) {
      product = product * a;
      a = a * a;
      counter = counter - 1;
      counter = counter / 2;
    }
    else {
      a = a * a;
      counter = counter / 2;
    }
  }
  cout << to_string(product);
  return product;
}