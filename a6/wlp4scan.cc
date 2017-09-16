#include "kind.h"
#include "lexer.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <iostream>
// Use only the neeeded aspects of each namespace
using std::string;
using std::vector;
using std::endl;
using std::cerr;
using std::cout;
using std::cin;
using std::getline;
using ASM::Token;
using ASM::Lexer;


int main(int argc, char* argv[]){
  // Nested vector representing lines of Tokens
  // Needs to be used here to cleanup in the case
  // of an exception
  vector< vector<Token*> > tokLines;

  try{
    // Create a MIPS recognizer to tokenize
    // the input lines
    Lexer lexer;
    // Tokenize each line of the input
    string line;
    while(getline(cin,line)){
      tokLines.push_back(lexer.scan(line));
    }

    // Iterate over the lines of tokens and print them
    // to standard error
    vector<vector<Token*> >::iterator it;

    for(it = tokLines.begin(); it != tokLines.end(); ++it){
      vector<Token*>::iterator it2;
      it2 = it->begin();

      for(vector<Token*>::iterator it3= it->begin(); it3 != it->end(); ++it3){ // print all tokens
        if ((*it3)->getKind() == ASM::ABORT || (*it3)->getKind() == ASM::EXCLAMATION) {
          throw string("ERR");
        }
        if ((*it3)->getKind() == ASM::NUM) {
          uint64_t i = (*it3)->toInt();
          //cout << i;
          if (i > 2147483647) {
            throw string("ERR");
         }
        }
      }
    }

    for(it = tokLines.begin(); it != tokLines.end(); ++it){
      vector<Token*>::iterator it2;
      it2 = it->begin();

      for(vector<Token*>::iterator it3= it->begin(); it3 != it->end(); ++it3){ // print all tokens
        if ((*it3)->getKind() == ASM::COMMENT) {
        }
        else if ((*it3)->getKind() == ASM::ID) {
          if ((*it3)->getLexeme() == "return") {
            cout << "RETURN return" << endl;
          }
          else if ((*it3)->getLexeme() == "if") {
            cout << "IF if" << endl;
          }
          else if ((*it3)->getLexeme() == "else") {
            cout << "ELSE else" << endl;
          }
          else if ((*it3)->getLexeme() == "while") {
            cout << "WHILE while" << endl;
          }
          else if ((*it3)->getLexeme() == "println") {
            cout << "PRINTLN println" << endl;
          }
          else if ((*it3)->getLexeme() == "wain") {
            cout << "WAIN wain" << endl;
          }
          else if ((*it3)->getLexeme() == "int") {
            cout << "INT int" << endl;
          }
          else if ((*it3)->getLexeme() == "new") {
            cout << "NEW new" << endl;
          }
          else if ((*it3)->getLexeme() == "delete") {
            cout << "DELETE delete" << endl;
          }
          else if ((*it3)->getLexeme() == "NULL") {
            cout << "NULL NULL" << endl;
          }
          else {
            cout << *(*it3) << endl;
          }
        }
        else {        
          cout << *(*it3) << endl;
        }
      }
    }
      


  } catch(const string& msg){
    cerr << "ERROR in lexing" << endl;
    // If an exception occurs print the message and end the program
  }
  // Delete the Tokens that have been made
  vector<vector<Token*> >::iterator it;
  for(it = tokLines.begin(); it != tokLines.end(); ++it){
    vector<Token*>::iterator it2;
    for(it2 = it->begin(); it2 != it->end(); ++it2){
      delete *it2;
    }
  }
}
