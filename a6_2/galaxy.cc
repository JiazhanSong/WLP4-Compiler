#include <vector>
#include <string>
#include <iostream>
#include <sstream>
// Use only the neeeded aspects of each namespace
using std::string;
using std::istream;
using std::ostream;
using std::endl;
using std::cout;
using std::cin;
using std::istringstream;

//Skip the grammar part of the input.
void skipLine(istream &in) {
  string s;
  getline(in, s);
}
 
void skipGrammar(istream &in) {
  int i, numTerm, numNonTerm, numRules;

  // read the number of terminals and move to the next line
  in >> numTerm;
  skipLine(in);
  
  // skip the lines containing the terminals
  for (i = 0; i < numTerm; i++) {
    skipLine(in);
  }
  
  // read the number of non-terminals and move to the next line
  in >> numNonTerm;
  skipLine(in);
  
  // skip the lines containing the non-terminals
  for (i = 0; i < numNonTerm; i++) {
    skipLine(in);
  }
  
  // skip the line containing the start symbol
  skipLine(in);
  
  // read the number of rules and move to the next line
  in >> numRules;
  skipLine(in);
  
  // skip the lines containing the production rules
  for (i = 0; i < numRules; i++) {
    skipLine(in);
  }
}

string trim(const string &str) {
  size_t begin = str.find_first_not_of(" \t\n");
  if (begin == string::npos) return "";

  size_t end = str.find_last_not_of(" \t\n");

  return str.substr(begin, end - begin + 1);
}

int recurse(int i){
  string line;
  getline(cin, line);
  line = trim(line);
  string checker;

  if (i == 0) {
    istringstream iss(line);
    iss >> checker;
    iss >> checker;

    if (checker == "expr") {
      return recurse(0) - recurse(1);
    }
    else {
      return recurse(1);
    }
  }
  else {
    istringstream iss(line);
    iss >> checker;
    iss >> checker;

    if (checker == "id") {
      return 42;
    }
    else {
      return recurse(0);
    }
  }
}


int main() {
  skipGrammar(std::cin);
  string extra;    // get rid of first line
  getline(cin, extra);

  cout << recurse(0) << endl;
}