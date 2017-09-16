#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <stack>
// Use only the neeeded aspects of each namespace
using std::stack;
using std::vector;
using std::string;
using std::istream;
using std::ostream;
using std::endl;
using std::cout;
using std::cin;
using std::cerr;
using std::istringstream;

//Skip the grammar part of the input.
void skipLine(istream &in) {
  string s;
  getline(in, s);
}
 
vector<vector<string>> skipGrammar(istream &in) {
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
  
  vector<vector<string>> rules;
  // skip the lines containing the production rules
  for (i = 0; i < numRules; i++) {
    vector<string> subRule;
    string rule;
    string word;

    getline(in, rule);
    istringstream iss(rule);
    while(iss >> word) {
      subRule.emplace_back(word);
    }
    rules.emplace_back(subRule);
  }
  return rules;
}

string trim(const string &str) {
  size_t begin = str.find_first_not_of(" \t\n");
  if (begin == string::npos) return "";

  size_t end = str.find_last_not_of(" \t\n");

  return str.substr(begin, end - begin + 1);
}

vector<string> nextState(vector<vector<string>> transitions, string word, int state) { // returns instructions for next step
  vector<string> next;
  for (int i = 0; i < transitions.size(); i++) {
    if (transitions[i][0] == std::to_string(state) && transitions[i][1] == word) {
      for (int j = 2; j < transitions[i].size(); j++) {
        next.emplace_back(transitions[i][j]);
      }
    }
  }
  return next;
}

vector<vector<string>> getTransitions(int transNum) { // returns unorganized 2D vector of all transitions
  vector<vector<string>> transitions;
  string line;
  string word;
  while (transNum > 0) {
    vector<string> words;
    getline(cin, line);
    istringstream iss(line);

    while(iss >> word) {
      words.emplace_back(word);
    }

    transitions.emplace_back(words);
    transNum--;
  }
  return transitions;
}


int main() {
  vector<vector<string>> rules = skipGrammar(cin);
  string line;    // get rid of first line: # of states
  getline(cin, line);

  int transNum; // # of LR transitions
  cin >> transNum;  
  skipLine(cin);

  vector<vector<string>> transitions = getTransitions(transNum);
  
  
  try {
    int wordsRead = 1;
    vector<vector<string>> output; // setup output
    string lines;  // read derivation
    string line;
    while (getline(cin, line)) {
      lines = lines + " " + line;
    }
    istringstream iss(lines);

    stack<int> states;
    stack<string> symbols;
    
    string word;
    iss >> word;
    symbols.push(word); // push BOF

    vector<string> next = nextState(transitions, word, 0); // 0 is starting state
    if (next.empty()) {
      throw("ERROR at " + std::to_string(wordsRead));
    }
    if (next[0] == "shift") { // check if shift or reduce operation
      states.push(std::stoi(next[1]));
    }

    while(iss >> word) {
      wordsRead++;
      // find (state, word) combination in transitions table
      
      next.clear();
      vector<string> next = nextState(transitions, word, states.top());
      if (next.empty()) {
        throw("ERROR at " + std::to_string(wordsRead));
      }
      
      while (next[0] == "reduce") {
        int ruleNum = std::stoi(next[1]);
        output.emplace_back(rules[ruleNum]);
        int size = rules[ruleNum].size();

        for (int i = size-1; i > 0; i--) {
          states.pop();
          symbols.pop();
        }

        symbols.push(rules[ruleNum][0]); // add replacement onto the stack
        next.clear();
        next = nextState(transitions, symbols.top(), states.top());
        if (next.empty()) {
        throw("ERROR at " + std::to_string(wordsRead));
      }
        if (next[0] == "shift") { // check if shift or reduce operation
          states.push(std::stoi(next[1]));
        }
        next.clear();
        next = nextState(transitions, word, states.top());
        if (next.empty()) {
         throw("ERROR at " + std::to_string(wordsRead));
        }
      }

      symbols.push(word);
      if (next.empty()) {
        throw("ERROR at " + std::to_string(wordsRead));
      }
      else {
        states.push(std::stoi(next[1]));
      }
    }

    // print as necessary
    output.emplace_back(rules[0]); // add rule 0 for BOF and EOF
    for (int i = 0; i < output.size(); i++) {
      for (int j = 0; j < output[i].size(); j++) {
        if (j!=0) {
          cout << " ";
        }
        cout << output[i][j];
      }
      cout << endl;
    }

    
  }
  catch (string msg) {
    cerr << msg << endl;
  }
}