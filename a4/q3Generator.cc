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
  string state = "_state";

  cout << "11" << endl; // valid symbol inputs
  cout << "-" << endl;
  for (int i=0; i<10; i++) {
    cout << to_string(i) <<endl;
  }

  cout << "2002" << endl; // valid states 999 + 1 (zero) + 999 + 3
  cout << "init" << endl;
  cout << "lead0" << endl;
  cout << "neg" << endl;
  for (int i=-999; i <= 999; i++) {
    cout << to_string(i) + state << endl;
  }

  cout << "init" << endl; // start state

  cout << "256" << endl; // valid end states
  for (int i=-128; i <= 127; i++) {
    cout << to_string(i) + state << endl;
  }

  // state changes: POSITIVE then NEGATIVE
  cout << "2011" << endl;    // 1 + 20 + 10 + 180 + 1800
  cout << "init" << " - " << "neg" << endl;
  for (int i=0; i < 10; i++) { // init to single digit
    cout << "init" << " " + to_string(i) + " " << to_string(i) + state << endl;
    if (i != 0) {
      cout << "neg" << " " + to_string(i) + " " << "-" + to_string(i) + state << endl;
    }
    else {
      cout << "neg" << " " + to_string(i) + " " << "lead0" << endl;
    }
  }
  
  for (int j=0; j < 10; j++) { // zero + input = outBounds POSITIVE
    cout << "0" + state << " " + to_string(j) + " " << "lead0" << endl;
  }
  //for (int j=0; j < 10; j++) { // zero + input = outBounds NEGATIVE
  //  cout << "-0" + state << " " + to_string(j) + " " << "lead0" << endl;
  //}

  for (int i=1; i < 10; i++) { // single digit to double digit
    for (int j=0; j < 10; j++) {
      cout << to_string(i) + state << " " + to_string(j) + " " << to_string(i) + to_string(j) + state << endl;
      cout << "-" + to_string(i) + state << " " + to_string(j) + " " << "-" + to_string(i) + to_string(j) + state << endl;
    }
  }

  for (int i=10; i < 100; i++) {
    for (int j=0; j < 10; j++) {
      cout << to_string(i) + state << " " + to_string(j) + " " << to_string(i) + to_string(j) + state << endl;
      cout << "-" + to_string(i) + state << " " + to_string(j) + " " << "-" + to_string(i) + to_string(j) + state << endl;
    }
  }
}