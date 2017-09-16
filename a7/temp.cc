#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stack>
// Use only the neeeded aspects of each namespace
using std::stack;
using std::vector;
using std::string;
using std::istream;
using std::ostream;
using std::ofstream;
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

vector<vector<string>> getTransitions(int transNum, std::ifstream& data) { // returns unorganized 2D vector of all transitions
  vector<vector<string>> transitions;
  string line;
  string word;
  while (transNum > 0) {
    vector<string> words;
    getline(data, line);
    istringstream iss(line);

    while(iss >> word) {
      words.emplace_back(word);
    }

    transitions.emplace_back(words);
    transNum--;
  }
  return transitions;
}

struct Token{
  string type;
  string name;
  bool ifterminal;
  vector<Token> tokens;
};

int main() {
  remove("cfgr");
  ofstream a_file ( "cfgr" );

  string line;
  std::ifstream copy;
  copy.open("wlp4_data");
  while(getline(copy, line)) {
    a_file << line << endl;
  }

  std::ifstream data;
  data.open("wlp4_data");

  vector<vector<string>> rules = skipGrammar(data); // get rid of first line: # of states
  getline(data, line);

  int transNum; // # of LR transitions
  data >> transNum;  
  skipLine(data);

  vector<vector<string>> transitions = getTransitions(transNum, data);
  
  
  try {
    int wordsRead = 0;
    vector<Tokens> output; // setup output
    string lines;  // read derivation
    string line;
    while (getline(cin, line)) {
      lines = lines + " " + line;
    }
    lines = "BOF BOF " + lines + " EOF EOF";
    istringstream iss(lines);

    stack<int> states;
    stack<Token> symbols;
    
    string wordtype;
    string wordname;
    iss >> wordtype >> wordname;
    Token word = {wordtype,wordname,true};
    symbols.push(word); // push BOF

    vector<string> next = nextState(transitions, word.type, 0); // 0 is starting state
    if (next.empty()) {
      throw("ERROR at " + std::to_string(wordsRead));
    }
    if (next[0] == "shift") { // check if shift or reduce operation
      states.push(std::stoi(next[1]));
    }

    while(iss >> wordtype >> wordname) {
      Token word = {wordtype,wordname,true};
      wordsRead++;
      // find (state, word) combination in transitions table
      
      next.clear();
      vector<string> next = nextState(transitions, word.type, states.top());
      if (next.empty()) {
        throw("ERROR at " + std::to_string(wordsRead));
      }
      
      while (next[0] == "reduce") {
        int ruleNum = std::stoi(next[1]);
        Rule newRule;
        newRule.rulevector = rules[ruleNum];
        int size = rules[ruleNum].size();

        for (int i = size-1; i > 0; i--) {
          states.pop();
          newRule.tokens.emplace_back(symbols.top());
          symbols.pop();
        }
        output.emplace_back(newRule);

        Token replacement = {rules[ruleNum][0],rules[ruleNum][0],false};
        symbols.push(replacement); // add replacement onto the stack
        next.clear();
        next = nextState(transitions, symbols.top().type, states.top());
        if (next.empty()) {
        throw("ERROR at " + std::to_string(wordsRead));
      }
        if (next[0] == "shift") { // check if shift or reduce operation
          states.push(std::stoi(next[1]));
        }
        next.clear();
        next = nextState(transitions, word.type, states.top());
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
    a_file << "BOF BOF" << endl;
    for (int i = 0; i < output.size(); i++) {
      for (int j = 0; j < output[i].tokens.size(); j++) {
        if (output[i].tokens[j].ifterminal) {
          a_file << output[i].tokens[j].type << " " << output[i].tokens[j].name << endl;
        }
      }
      for (int j = 0; j < output[i].rulevector.size(); j++) {
        if (j!=0) {
          a_file << " ";
        }
        a_file << output[i].rulevector[j];
      }
      a_file << endl;
    }
    a_file << "EOF EOF" << endl;
    a_file << "start BOF procedures EOF" << endl;
    
    
  }
  catch (string msg) {
    cerr << msg << endl;
  }
}