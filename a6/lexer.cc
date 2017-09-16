#include "lexer.h"
#include "kind.h"
using std::string;
using std::vector;

// Use the annonymous namespace to prevent external linking
namespace {
  // An array represents the Token kind that each state represents
  // The ERR Kind represents an invalid token
  ASM::Kind stateKinds[] = {
    ASM::ABORT,
    ASM::ERR, // ST_ERR,
    ASM::ERR, // ST_START
    ASM::WHITESPACE, // ST_WHITESPACE
    ASM::ID, // ST_ID,
    ASM::NUM, // ST_ZERO,
    ASM::NUM, // ST_NUM,
    ASM::LPAREN, // ST_LPAREN,
    ASM::RPAREN, // ST_RPAREN,
    ASM::LBRACE, // ST_LBRACE,
    ASM::RBRACE, // ST_RBRACE,
    ASM::BECOMES, // ST_BECOMES,
    ASM::EQ, // ST_EQ,
    ASM::NE, // ST_NE,
    ASM::LT, // ST_LT,
    ASM::GT, // ST_GT,
    ASM::LE, // ST_LE,
    ASM::GE, // ST_GE,
    ASM::PLUS, // ST_PLUS,
    ASM::MINUS, // ST_MINUS
    ASM::STAR, // ST_STAR,
    ASM::SLASH, // ST_SLASH,
    ASM::PCT, // ST_PCT,
    ASM::COMMA, // ST_COMMA
    ASM::SEMI, // ST_SEMI,
    ASM::NEW, // ST_NEW,
    ASM::DELETE, // ST_DELETE,
    ASM::LBRACK, // ST_LBRACK,
    ASM::RBRACK, // ST_RBRACK,
    ASM::AMP, // ST_AMP,
    ASM::NUL, // ST_NUL
    ASM::COMMENT,
    ASM::EXCLAMATION
  };
  const string whitespace = "\t\n\r ";
  const string letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  const string nonEnds = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
  const string equal = "=";
  const string lt = "<";
  const string gt = ">";
  const string exclamation = "!";
  const string digits = "0123456789";
  const string hexDigits = "0123456789ABCDEFabcdef";
  const string oneToNine =  "123456789";

}

ASM::Lexer::Lexer(){
  // Set default transitions to the Error state
  for(int i=0; i < maxStates; ++i){
    for(int j=0; j < maxTrans; ++j){
      delta[i][j] = ST_ERR;
    }
  }
  // Change transitions as appropriate for the MIPS recognizer
  setTrans( ST_START,      whitespace,     ST_WHITESPACE );
  setTrans( ST_WHITESPACE, whitespace,     ST_WHITESPACE );
  setTrans( ST_START,      letters,        ST_ID         );
  setTrans( ST_ID,         letters+digits, ST_ID         );
  setTrans( ST_START,      oneToNine,      ST_NUM        );
  setTrans( ST_NUM,        digits,         ST_NUM        );
  setTrans( ST_START,      "-",            ST_MINUS      );
  setTrans( ST_START,      "0",            ST_ZERO       );
  setTrans( ST_ZERO,       nonEnds,        ST_ABORT      );
  setTrans( ST_START,      ",",            ST_COMMA      );
  setTrans( ST_START,      "(",            ST_LPAREN     );
  setTrans( ST_START,      ")",            ST_RPAREN     );
  setTrans( ST_START,      "{",            ST_LBRACE     );
  setTrans( ST_START,      "}",            ST_RBRACE     );
  setTrans( ST_START,      "=",            ST_BECOMES    );
  setTrans( ST_BECOMES,    "=",            ST_EQ         );
  //setTrans( ST_BECOMES,    set2,           ST_ABORT      );
  setTrans( ST_START,      "!",            ST_EXCLAMATION);
  setTrans( ST_EXCLAMATION,"=",            ST_NE         );
  setTrans( ST_START,      "<",            ST_LT         );
  setTrans( ST_START,      ">",            ST_GT         );
  setTrans( ST_LT,         "=",            ST_LE         );
  setTrans( ST_GT,         "=",            ST_GE         );
  setTrans( ST_START,      "+",            ST_PLUS       );
  setTrans( ST_START,      "-",            ST_MINUS      );
  setTrans( ST_START,      "*",            ST_STAR       );
  setTrans( ST_START,      "/",            ST_SLASH      );
  setTrans( ST_SLASH,      "/",            ST_COMMENT    );
  setTrans( ST_START,      "%",            ST_PCT        );
  setTrans( ST_START,      ",",            ST_COMMA      );
  setTrans( ST_START,      ";",            ST_SEMI       );
  setTrans( ST_START,      "[",            ST_LBRACK     );
  setTrans( ST_START,      "]",            ST_RBRACK     );
  setTrans( ST_START,      "&",            ST_AMP        );

  // A comment can only ever lead to the comment state
  for(int j=0; j < maxTrans; ++j) delta[ST_COMMENT][j] = ST_COMMENT;
}

// Set the transitions from one state to another state based upon characters in the
// given string
void ASM::Lexer::setTrans(ASM::State from, const string& chars, ASM::State to){
  for(string::const_iterator it = chars.begin(); it != chars.end(); ++it)
    delta[from][static_cast<unsigned int>(*it)] = to;
}

// Scan a line of input (as a string) and return a vector
// of Tokens representing the MIPS instruction in that line
vector<ASM::Token*> ASM::Lexer::scan(const string& line){
  // Return vector
  vector<Token*> ret;
  if(line.size() == 0) return ret;
  // Always begin at the start state
  State currState = ST_START;
  // startIter represents the beginning of the next Token
  // that is to be recognized. Initially, this is the beginning
  // of the line.
  // Use a const_iterator since we cannot change the input line
  string::const_iterator startIter = line.begin();
  // Loop over the the line
  for(string::const_iterator it = line.begin();;){
    // Assume the next state is the error state
    State nextState = ST_ERR;
    // If we aren't done then get the transition from the current
    // state to the next state based upon the current character of
    //input
    if(it != line.end())
      nextState = delta[currState][static_cast<unsigned int>(*it)];
    // If there is no valid transition then we have reach then end of a
    // Token and can add a new Token to the return vector
    if(ST_ERR == nextState){
      // Get the kind corresponding to the current state
      Kind currKind = stateKinds[currState];
      // If we are in an Error state then we have reached an invalid
      // Token - so we throw and error and delete the Tokens parsed
      // thus far
      if(ERR == currKind){
        currKind = ABORT;
        ret.push_back(Token::makeToken(currKind,string(startIter,it)));
      }
      if (currKind==EQ || currKind==NE || currKind == LT || currKind == LE
      || currKind == GT ||currKind == GE ||currKind == BECOMES){
        if (static_cast<unsigned int>(*it)==static_cast<unsigned int>(*(equal.begin())) || static_cast<unsigned int>(*it)==static_cast<unsigned int>(*(lt.begin()))
         || static_cast<unsigned int>(*it)==static_cast<unsigned int>(*(gt.begin())) ||static_cast<unsigned int>(*it)==static_cast<unsigned int>(*(exclamation.begin()))) {
          currKind = ASM::ABORT;
          ret.push_back(Token::makeToken(currKind,string(startIter,it)));
        }
      }
      // If we are not in Whitespace then we push back a new token
      // based upon the kind of the State we end in
      // Whitespace is ignored for practical purposes
      if(WHITESPACE != currKind)
        ret.push_back(Token::makeToken(currKind,string(startIter,it)));
      // Start of next Token begins here
      startIter = it;
      // Revert to start state to begin recognizing next token
      currState = ST_START;
      if(it == line.end()) break;
    } else {
      // Otherwise we proceed to the next state and increment the iterator
      currState = nextState;
      ++it;
    }
  }
  return ret;
}
