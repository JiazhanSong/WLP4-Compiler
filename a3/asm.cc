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
using std::cin;
using std::getline;
using ASM::Token;
using ASM::Lexer;


int main(int argc, char* argv[]){
  // Nested vector representing lines of Tokens
  // Needs to be used here to cleanup in the case
  // of an exception
  vector< vector<Token*> > tokLines;
  std::unordered_map<string, int> table;
  vector< vector<Token*> > instructions;

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
    int locationCounter = 0;
    for(it = tokLines.begin(); it != tokLines.end(); ++it){
      vector<Token*>::iterator it2;
      it2 = it->begin();

      //for(vector<Token*>::iterator it3= it->begin(); it3 != it->end(); ++it3){ // print all tokens
      //  cerr << "  Token: "  << *(*it3) << endl;
      //}

      // PHASE 1
      // Check for label, add to symbol table
      while (it2 != it->end() && (*it2)->getKind() == ASM::LABEL) {
        if (table.find((*it2)->getLexeme()) != table.end()) {
          throw string("ERROR: repeated label");
        }

        table[(*it2)->getLexeme()] = locationCounter;
        it2++;
      }
      // Check for OPCODE, instruction vector
      if (it2 == it->end()) {
        // do nothing if empty line
      }
      else if ((*it2)->getKind() == ASM::DOTWORD) {
        if (it2+1 == it->end() || it2+2 != it->end()) { // check .word is followed by only one token, throw otherwise
          throw string("ERROR: .word not followed by one token");
        }
        if ((*(it2+1))->getKind() != ASM::INT && (*(it2+1))->getKind() != ASM::HEXINT 
            && (*(it2+1))->getKind() != ASM::ID) { // if not valid .word input, throw
          throw string("ERROR: Invalid .word input");
        }

        vector<Token*> lineInstruction;
        for(vector<Token*>::iterator it3= it2; it3 != it->end(); ++it3){
          //cerr << "  Token: "  << *(*it3) << endl;
          lineInstruction.push_back(*it3);
        }
        instructions.push_back(lineInstruction);

        locationCounter += 4;
      }
      else if ((*it2)->getKind() == ASM::ID && ((*it2)->getLexeme() == "jr" || (*it2)->getLexeme() == "jalr")) {
        if (it2+1 == it->end() || it2+2 != it->end()) { // check opcode is followed by only one token, throw otherwise
          throw string("ERROR: jr or jalr not followed by one token");
        }
        if ((*(it2+1))->getKind() != ASM::REGISTER) { // if not valid .word input, throw
          throw string("ERROR: Invalid jr or jalr input");
        }

        vector<Token*> lineInstruction;
        for(vector<Token*>::iterator it3= it2; it3 != it->end(); ++it3){
          //cerr << "  Token: "  << *(*it3) << endl;
          lineInstruction.push_back(*it3);
        }
        instructions.push_back(lineInstruction);

        locationCounter += 4;
      }
      else if ((*it2)->getKind() == ASM::ID && ((*it2)->getLexeme() == "add" || (*it2)->getLexeme() == "sub"
        || (*it2)->getLexeme() == "slt" || (*it2)->getLexeme() == "sltu")) {

        if (it2+6 != it->end()) { // check opcode is followed by 5 tokens (commas), throw otherwise
          throw string("ERROR: add, sub, slt or sltu not followed by 5 (commas) tokens");
        }
        for (int i=1; i < 6; i++) {
          if (i%2 == 0) {
            if ((*(it2+i))->getKind() != ASM::COMMA) { // if not valid register input, throw
              throw string("ERROR: Invalid add, sub, slt or sltu input");
            }
          }
          else {
            if ((*(it2+i))->getKind() != ASM::REGISTER) { // if not valid register input, throw
              throw string("ERROR: Invalid add, sub, slt or sltu input");
            }
          }
        }

        vector<Token*> lineInstruction;
        for(vector<Token*>::iterator it3= it2; it3 != it->end(); ++it3){
          //cerr << "  Token: "  << *(*it3) << endl;
          lineInstruction.push_back(*it3);
        }
        instructions.push_back(lineInstruction);

        locationCounter += 4;
      }
      else if ((*it2)->getKind() == ASM::ID && ((*it2)->getLexeme() == "beq" || (*it2)->getLexeme() == "bne")) {
        if (it2+6 != it->end()) { // check opcode is followed by 5 tokens (commas), throw otherwise
          throw string("ERROR: add, sub, slt or sltu not followed by 5 (commas) tokens");
        }
        for (int i=1; i < 6; i++) {
          if (i == 5) {
            if ((*(it2+i))->getKind() != ASM::INT && (*(it2+i))->getKind() != ASM::HEXINT
              && (*(it2+i))->getKind() != ASM::ID) {
                throw string("ERROR: Invalid beq or bne input for branch value");
              }
            if ((*(it2+i))->getKind() == ASM::INT) {
              int32_t value32 = (*(it2+i))->toInt();
              int32_t lowBound = -32768;
              int32_t upBound = 32767;
              if (value32 > upBound || value32 < lowBound) {
                throw string("ERROR: beq or bne branch value not in range");
              }
            }
            else if ((*(it2+i))->getKind() == ASM::HEXINT) {
              int32_t value32 = (*(it2+i))->toInt();
              int32_t upBound = 65535;
              if (!(value32 <= upBound && value32 >=0/*!(lowBound <= value && value <= upBound*/)) {
                throw string("ERROR: beq or bne branch value not in range");
              }
            }
          }
          else if (i%2 == 0) {
            if ((*(it2+i))->getKind() != ASM::COMMA) { // if not valid register input, throw
              throw string("ERROR: Invalid beq or bne input");
            }
          }
          else {
            if ((*(it2+i))->getKind() != ASM::REGISTER) { // if not valid register input, throw
              throw string("ERROR: Invalid beq or bn input");
            }
          }
        }

        vector<Token*> lineInstruction;
        for(vector<Token*>::iterator it3= it2; it3 != it->end(); ++it3){
          //cerr << "  Token: "  << *(*it3) << endl;
          lineInstruction.push_back(*it3);
        }
        instructions.push_back(lineInstruction);

        locationCounter += 4;
      }
      else if ((*it2)->getKind() == ASM::ID && ((*it2)->getLexeme() == "lis" || (*it2)->getLexeme() == "mflo"
      || (*it2)->getLexeme() == "mfhi")) {
        if (it2+1 == it->end() || it2+2 != it->end()) { // check opcode is followed by only one token, throw otherwise
          throw string("ERROR: lis, mflo or mhfi not followed by one token");
        }
        if ((*(it2+1))->getKind() != ASM::REGISTER) { // if not valid input, throw
          throw string("ERROR: Invalid lis, mflo or mhfi input");
        }

        vector<Token*> lineInstruction;
        for(vector<Token*>::iterator it3= it2; it3 != it->end(); ++it3){
          //cerr << "  Token: "  << *(*it3) << endl;
          lineInstruction.push_back(*it3);
        }
        instructions.push_back(lineInstruction);

        locationCounter += 4;
      }
      else if ((*it2)->getKind() == ASM::ID && ((*it2)->getLexeme() == "mult" || (*it2)->getLexeme() == "multu"
      || (*it2)->getLexeme() == "div" || (*it2)->getLexeme() == "divu")) {
        if (it2+4 != it->end()) { // check opcode is followed by three tokens, throw otherwise
          throw string("ERROR: mult, multu, div or divu not followed by three token");
        }
        if ((*(it2+1))->getKind() != ASM::REGISTER || (*(it2+2))->getKind() != ASM::COMMA || (*(it2+3))->getKind() != ASM::REGISTER) { // if not valid input, throw
          throw string("ERROR: Invalid mult, multu, div or divu input");
        }

        vector<Token*> lineInstruction;
        for(vector<Token*>::iterator it3= it2; it3 != it->end(); ++it3){
          //cerr << "  Token: "  << *(*it3) << endl;
          lineInstruction.push_back(*it3);
        }
        instructions.push_back(lineInstruction);

        locationCounter += 4;
      }
      else if ((*it2)->getKind() == ASM::ID && ((*it2)->getLexeme() == "sw" || (*it2)->getLexeme() == "lw")) {
        if (it2+7 != it->end()) { // check opcode is followed by 6 tokens (commas), throw otherwise
          throw string("ERROR: add, sub, slt or sltu not followed by 5 (commas) tokens");
        }
        for (int i=1; i < 7; i++) {
          if (i == 3) {
            if ((*(it2+i))->getKind() != ASM::INT && (*(it2+i))->getKind() != ASM::HEXINT) {
                throw string("ERROR: Invalid sw or lw input for numeric address value");
              }
            if ((*(it2+i))->getKind() == ASM::INT) {
              int32_t value32 = (*(it2+i))->toInt();
              int32_t lowBound = -32768;
              int32_t upBound = 32767;
              if (value32 > upBound || value32 < lowBound) {
                throw string("ERROR: sw or lw branch value not in range");
              }
            }
            else if ((*(it2+i))->getKind() == ASM::HEXINT) {
              int32_t value32 = (*(it2+i))->toInt();
              int32_t upBound = 65535;
              if (!(value32 <= upBound && value32 >=0/*!(lowBound <= value && value <= upBound*/)) {
                throw string("ERROR: sw or lw branch value not in range");
              }
            }
          }
          else if (i == 2 && (*(it2+i))->getKind() != ASM::COMMA) {
            throw string("ERROR: Invalid sw or lw input (COMMA check)");
          }
          else if (i == 1 || i ==5) {
            if ((*(it2+i))->getKind() != ASM::REGISTER) { // if not valid register input, throw
              throw string("ERROR: Invalid sw or lw input (REGISTER check)");
            }
          }
          else if (i == 4) {
            if ((*(it2+i))->getKind() != ASM::LPAREN) { // if not valid register input, throw
              throw string("ERROR: Invalid sw or lw input (LPAREN check)");
            }
          }
          else if (i == 6) {
            if ((*(it2+i))->getKind() != ASM::RPAREN) { // if not valid register input, throw
              throw string("ERROR: Invalid sw or lw input (RPAREN check)");
            }
          }
        }

        vector<Token*> lineInstruction;
        for(vector<Token*>::iterator it3= it2; it3 != it->end(); ++it3){
          //cerr << "  Token: "  << *(*it3) << endl;
          lineInstruction.push_back(*it3);
        }
        instructions.push_back(lineInstruction);

        locationCounter += 4;
      }
      else {
        throw string("ERROR: Invalid start of line (label, dotword, or opcode)");
      }
      


        
    }
    //  PHASE 2: cout instructions using bitwise & four (comparing two binaries, 0=0, 1&1=1) times and shifting as needed.
    int counter = -1;
    for(it = instructions.begin(); it != instructions.end(); ++it){
      counter++;
      vector<Token*>::iterator it2;
      it2 = it->begin();

      if ((*it2)->getKind() == ASM::DOTWORD) {
        uint32_t word;
        if ((*(it2+1))->getKind() == ASM::ID) {
          if (table.find((*(it2+1))->getLexeme() + ":") == table.end()) { // check .word label is in symbol table
            throw string("ERROR: .word label not found in symbol table");
          }
          word = table[(*(it2+1))->getLexeme() + ":"];
        }
        else {
          word = (*(it2+1))->toInt();
        }

        uint32_t word_1 = word & 0xFF;
        uint32_t word_2 = word & 0xFF00;
        uint32_t word_3 = word & 0xFF0000;
        uint32_t word_4 = word & 0xFF000000;
        uint8_t c_1 = word_1;
        uint8_t c_2 = word_2 >> 8;
        uint8_t c_3 = word_3 >> 16;
        uint8_t c_4 = word_4 >> 24;

        //std::cout << word << endl;
        //std::cout << word_4 << endl << word_3 << endl << word_2 << endl << word_1 << endl;
        std::cout << c_4 << c_3 << c_2 << c_1;// << endl;
      }
      else if ((*it2)->getKind() == ASM::ID && ((*it2)->getLexeme() == "jr" || (*it2)->getLexeme() == "jalr")) {
        uint32_t word = 9;
        if ((*it2)->getLexeme() == "jr") {
          word = 8;
        }

        uint32_t reg = (*(it2+1))->toInt() << 21;
        word = word | reg;
        uint32_t word_1 = word & 0xFF;
        uint32_t word_2 = word & 0xFF00;
        uint32_t word_3 = word & 0xFF0000;
        uint32_t word_4 = word & 0xFF000000;
        uint8_t c_1 = word_1;
        uint8_t c_2 = word_2 >> 8;
        uint8_t c_3 = word_3 >> 16;
        uint8_t c_4 = word_4 >> 24;

        //std::cout << word << endl;
        //std::cout << word_4 << endl << word_3 << endl << word_2 << endl << word_1 << endl;
        std::cout << c_4 << c_3 << c_2 << c_1;// << endl;
      }
      else if ((*it2)->getKind() == ASM::ID && ((*it2)->getLexeme() == "add" || (*it2)->getLexeme() == "sub"
        || (*it2)->getLexeme() == "slt" || (*it2)->getLexeme() == "sltu")) {
        
        uint32_t word = 32;
        if ((*it2)->getLexeme() == "sub") {
          word = 34;
        }
        else if ((*it2)->getLexeme() == "slt") {
          word = 42;
        }
        else if ((*it2)->getLexeme() == "sltu") {
          word = 43;
        }

        uint32_t reg_1 = (*(it2+1))->toInt() << 11;
        uint32_t reg_2 = (*(it2+3))->toInt() << 21;
        uint32_t reg_3 = (*(it2+5))->toInt() << 16;
        word = word | reg_1 | reg_2 | reg_3;
        uint32_t word_1 = word & 0xFF;
        uint32_t word_2 = word & 0xFF00;
        uint32_t word_3 = word & 0xFF0000;
        uint32_t word_4 = word & 0xFF000000;
        uint8_t c_1 = word_1;
        uint8_t c_2 = word_2 >> 8;
        uint8_t c_3 = word_3 >> 16;
        uint8_t c_4 = word_4 >> 24;

        //std::cout << word << endl;
        //std::cout << word_4 << endl << word_3 << endl << word_2 << endl << word_1 << endl;
        std::cout << c_4 << c_3 << c_2 << c_1;// << endl;

      }
      else if ((*it2)->getKind() == ASM::ID && ((*it2)->getLexeme() == "beq" || (*it2)->getLexeme() == "bne")) {
        uint32_t word = 268435456;
        if ((*it2)->getLexeme() == "bne") {
          word = 335544320;
        }

        uint32_t reg_1 = (*(it2+1))->toInt() << 21;
        uint32_t reg_2 = (*(it2+3))->toInt() << 16;
        uint16_t reg_3;
        int16_t reg_4;
        if ((*(it2+5))->getKind() == ASM::ID) {
          if (table.find((*(it2+5))->getLexeme() + ":") == table.end()) { // check label is in symbol table
            throw string("ERROR: beq or bne label not found in symbol table");
          }
          int rangeCheck = (table[(*(it2+5))->getLexeme() + ":"] / 4) - counter - 1;
          if (rangeCheck > 32767 || rangeCheck < -32768) {
            throw string("ERROR: beq or bne branch value has 16 bit overflow");
          }
          reg_4 = rangeCheck;
          word = word | reg_1 | reg_2;
        }
        else {
          reg_3 = (*(it2+5))->toInt();
          word = word | reg_1 | reg_2 | reg_3;
        }
        //std::cout << "reg_4: " << reg_4 << endl;

        uint32_t word_1 = word & 0xFF;
        uint32_t word_2 = word & 0xFF00;
        uint32_t word_3 = word & 0xFF0000;
        uint32_t word_4 = word & 0xFF000000;
        if ((*(it2+5))->getKind() == ASM::ID) {
          word_1 = reg_4 & 0xFF;
          word_2 = reg_4 & 0xFF00;
        }
        uint8_t c_1 = word_1;
        uint8_t c_2 = word_2 >> 8;
        uint8_t c_3 = word_3 >> 16;
        uint8_t c_4 = word_4 >> 24;

        //std::cout << word << endl;
        //std::cout << word_4 << endl << word_3 << endl << word_2 << endl << word_1 << endl;
        std::cout << c_4 << c_3 << c_2 << c_1;// << endl;
      }
      else if ((*it2)->getKind() == ASM::ID && ((*it2)->getLexeme() == "lis" || (*it2)->getLexeme() == "mflo"
      || (*it2)->getLexeme() == "mfhi")) {

        uint32_t word = 16;
        if ((*it2)->getLexeme() == "mflo") {
          word = 18;
        }
        else if ((*it2)->getLexeme() == "lis") {
          word = 20;
        }

        uint32_t reg_1 = (*(it2+1))->toInt() << 11;
        word = word | reg_1;
        uint32_t word_1 = word & 0xFF;
        uint32_t word_2 = word & 0xFF00;
        uint32_t word_3 = word & 0xFF0000;
        uint32_t word_4 = word & 0xFF000000;
        uint8_t c_1 = word_1;
        uint8_t c_2 = word_2 >> 8;
        uint8_t c_3 = word_3 >> 16;
        uint8_t c_4 = word_4 >> 24;

        //std::cout << word << endl;
        //std::cout << word_4 << endl << word_3 << endl << word_2 << endl << word_1 << endl;
        std::cout << c_4 << c_3 << c_2 << c_1;// << endl;
      }
      else if ((*it2)->getKind() == ASM::ID && ((*it2)->getLexeme() == "mult" || (*it2)->getLexeme() == "multu"
      || (*it2)->getLexeme() == "div" || (*it2)->getLexeme() == "divu")) {

        uint32_t word = 24;
        if ((*it2)->getLexeme() == "multu") {
          word = 25;
        }
        else if ((*it2)->getLexeme() == "div") {
          word = 26;
        }
        else if ((*it2)->getLexeme() == "divu") {
          word = 27;
        }

        uint32_t reg_1 = (*(it2+1))->toInt() << 21;
        uint32_t reg_2 = (*(it2+3))->toInt() << 16;
        word = word | reg_1 | reg_2;
        uint32_t word_1 = word & 0xFF;
        uint32_t word_2 = word & 0xFF00;
        uint32_t word_3 = word & 0xFF0000;
        uint32_t word_4 = word & 0xFF000000;
        uint8_t c_1 = word_1;
        uint8_t c_2 = word_2 >> 8;
        uint8_t c_3 = word_3 >> 16;
        uint8_t c_4 = word_4 >> 24;

        //std::cout << word << endl;
        //std::cout << word_4 << endl << word_3 << endl << word_2 << endl << word_1 << endl;
        std::cout << c_4 << c_3 << c_2 << c_1;// << endl;
      }
      else if ((*it2)->getKind() == ASM::ID && ((*it2)->getLexeme() == "sw" || (*it2)->getLexeme() == "lw")) {
        uint32_t word = 2348810240;
        if ((*it2)->getLexeme() == "sw") {
          word = 2885681152;
        }

        uint32_t reg_1 = (*(it2+1))->toInt() << 16;
        uint32_t reg_2 = (*(it2+5))->toInt() << 21;
        uint16_t reg_3;
        reg_3 = (*(it2+3))->toInt();
        word = word | reg_1 | reg_2 | reg_3;
        //std::cout << "reg_4: " << reg_4 << endl;

        uint32_t word_1 = word & 0xFF;
        uint32_t word_2 = word & 0xFF00;
        uint32_t word_3 = word & 0xFF0000;
        uint32_t word_4 = word & 0xFF000000;

        uint8_t c_1 = word_1;
        uint8_t c_2 = word_2 >> 8;
        uint8_t c_3 = word_3 >> 16;
        uint8_t c_4 = word_4 >> 24;

        //std::cout << word << endl;
        //std::cout << word_4 << endl << word_3 << endl << word_2 << endl << word_1 << endl;
        std::cout << c_4 << c_3 << c_2 << c_1;// << endl;
      }
    }

    for (auto it = table.begin(); it != table.end(); it++) {
      cerr << (it->first).substr(0, (it->first).size()-1) << " " << it->second << endl;
    }
      


  } catch(const string& msg){
    // If an exception occurs print the message and end the program
    cerr << msg << endl;
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
