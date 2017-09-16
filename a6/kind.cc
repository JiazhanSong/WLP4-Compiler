#include "kind.h"
#include <sstream>
using std::string;
using std::istringstream;

// Use the annonymous namespace to prevent external linking
namespace {
  template <typename T>
  T fromString(const string& s, bool hex = false){
    istringstream iss(s);
    T n;
    if(hex)
      iss >> std::hex;
    if(iss >> n)
      return n;
    else
      throw string("ERROR: Type not convertible from string.");
  }
  // Strings representing the Kind of Token for printing
  const string kindStrings[] ={
    "ID",           // ID
    "INT",          // INT
    "COMMA",        // COMMA
    "LPAREN",       // LPAREN
    "RPAREN",       // RPAREN
    "LABEL",        // LABEL
    "DOTWORD",      // DOTWORD
    "WHITESPACE",   // WHITESPACE
    "ERR",           // ERR

    "NUM",
    "LBRACE",
    "RBRACE",
    "RETURN",
    "IF",
    "ELSE",
    "WHILE",
    "PRINTLN",
    "WAIN",
    "BECOMES",
    "EQ",
    "NE",
    "LT",
    "GT",
    "LE",
    "GE",
    "PLUS",
    "MINUS",
    "STAR",
    "SLASH",
    "PCT",
    "SEMI",
    "NEW",
    "DELETE",
    "LBRACK",
    "RBRACK",
    "AMP",
    "NUL",
    "COMMENT",
    "ABORT",
    "EXCLAMATION"
  };
}

ASM::Token::Token(ASM::Kind kind, string lexeme)
: kind(kind), lexeme(lexeme){}

// Cannot convert Tokens that do not represent numbers
// to an Integer
int ASM::Token::toInt() const{
  throw "ERROR: attempt to convert non-integer token: " + lexeme;
}

string ASM::Token::toString() const{
  return ::kindStrings[kind];
}

string ASM::Token::getLexeme() const{
  return lexeme;
}

ASM::Kind ASM::Token::getKind() const{
  return kind;
}

ASM::IntToken::IntToken(ASM::Kind kind, string lexeme)
: Token(kind,lexeme){}

// Ensure that the IntToken is a valid Integer for conversion
// Arguably, we could do this check in the constructor

// Print the pertinent information to the ostream
std::ostream& ASM::operator<<(std::ostream& out, const ASM::Token& t){
  out << t.toString() << " " << t.getLexeme();
  return out;
}

int ASM::IntToken::toInt() const{
  long long l = ::fromString<long long>(lexeme);
  unsigned long long ul = static_cast<unsigned long long>(l);
  if(ul > 4294967295LL) {
    throw "ERROR: constant out of range: " + lexeme;
  }
  return l;
}

// Make a new Token depending on the Kind provided
// Integral Kinds correspond to the appropriate Token type
ASM::Token* ASM::Token::makeToken(ASM::Kind kind, string lexeme){
  switch(kind){
    case NUM:
      return new IntToken(kind,lexeme);
    default:
      return new Token(kind,lexeme);
  }
}
