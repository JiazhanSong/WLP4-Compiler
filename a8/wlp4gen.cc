#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

using namespace std;

const vector<string> terminals = {"BOF", "BECOMES", "COMMA", "ELSE", "EOF", "EQ", "GE", "GT", "ID", "IF", "INT", "LBRACE", "LE", "LPAREN", "LT", "MINUS", "NE", "NUM", "PCT", "PLUS", "PRINTLN", "RBRACE", "RETURN", "RPAREN", "SEMI", "SLASH", "STAR", "WAIN", "WHILE", "AMP", "LBRACK", "RBRACK", "NEW", "DELETE", "NULL"};

struct Node {
  string rule;
  vector<string> tokens;
  vector<Node*> nodes;
  bool ifTerminal;

  Node(bool terminal): ifTerminal(terminal) {}
  ~Node() {
    for (int a=0; a < nodes.size(); a++) {
      delete nodes[a];
    }
  }
};

Node* createTree() {
  Node* currNode = new Node(false);

  string line;
  getline(cin, line);
  currNode->rule = line;
  istringstream iss(line);

  string word;
  iss >> word; // remove first part of rule
  while (iss >> word) {
    if (find(terminals.begin(), terminals.end(), word) != terminals.end()) {
      getline(cin, line);
      istringstream terminalstream(line);

      Node* terminalNode = new Node(true);
      terminalstream >> word;
      terminalNode->tokens.emplace_back(word);
      terminalstream >> word;
      terminalNode->tokens.emplace_back(word);
      currNode->nodes.emplace_back(terminalNode);
    }
    else {
      Node* ruleNode = createTree();
      currNode->nodes.emplace_back(ruleNode);
    }
  }

  return currNode;
}

int derivationScan(Node* Tree, vector<vector<string>>* table) {
  // 0: type int, 1: type int*
  string rule = Tree->rule;
  istringstream iss(rule);
  string word;
  iss >> word;
  if (word == "expr") {
    if (rule == "expr term") {
      return derivationScan(Tree->nodes[0], table);
    }
    else if (rule == "expr expr MINUS term") {
      int check1 = derivationScan(Tree->nodes[0], table);
      int check2 = derivationScan(Tree->nodes[2], table);
      if (check1 == 0 && check2 == 1) {// if either variable in MINUS is not int
        throw(string("subtract with int - int*"));
      }
      if (check1 == 1 && check2 == 0) { // int* and int variables
        return 1;
      }
      else { // two int variables
        return 0;
      }
    }
    else {
      int check1 = derivationScan(Tree->nodes[0], table);
      int check2 = derivationScan(Tree->nodes[2], table);
      if (check1 == 1 && check2 == 1) {// if either variable in PLUS is not int
        throw(string("plus with two int* variable"));
      }
      if (check1 == 1 || check2 == 1) { // int* and int variables
        return 1;
      }
      else { // two int variables
        return 0;
      }
    }
  }
  else if (word == "lvalue") {
    if (rule == "lvalue ID") {
      for (int i=1; i < (*table).size(); i++) {
        if ((*table)[i][0] == Tree->nodes[0]->tokens[1]) {
          if ((*table)[i][1] == "int") {
            return 0;
          }
          return 1;
        }
      }
    }
    else if (rule == "lvalue STAR factor") {
      int check = derivationScan(Tree->nodes[1], table);
      if (check != 1) {
        throw(string("STAR factor rule, factor does not have type int* to dereference"));
      }
      else {
        return 0;
      }
    }
    else {
      return derivationScan(Tree->nodes[1], table);
    }
  }
  else if (word == "term") {
    if (rule == "term factor") {
      return derivationScan(Tree->nodes[0], table);
    }
    else {
      int check1 = derivationScan(Tree->nodes[0], table);
      int check2 = derivationScan(Tree->nodes[2], table);
      if (check1 != 0 || check2 != 0) {// if either variable in mult/div/mod is not int
        throw(string("mult/div/mod with non int variable"));
      }
      return 0;
    }
  }
  else if (word == "factor") {
    if (rule == "factor ID") {
      for (int i=1; i < (*table).size(); i++) {
        if ((*table)[i][0] == Tree->nodes[0]->tokens[1]) {
          if ((*table)[i][1] == "int") {
            return 0;
          }
          return 1;
        }
      }
    }
    else if (rule == "factor NUM") {
      return 0;
    }
    else if (rule == "factor NULL") {
      return 1;
    }
    else if (rule == "factor LPAREN expr RPAREN") {
      return derivationScan(Tree->nodes[1], table);
    }
    else if (rule == "factor AMP lvalue") {
      int check = derivationScan(Tree->nodes[1], table);
      if (check != 0) {
        throw(string("Ampersand ID rule, ID does not have type int"));
      }
      else {
        return 1;
      }
    }
    else if (rule == "factor STAR factor") {
      int check = derivationScan(Tree->nodes[1], table);
      if (check != 1) {
        throw(string("STAR ID rule, ID does not have type int* to dereference"));
      }
      else {
        return 0;
      }
    }
    else if (rule == "factor NEW INT LBRACK expr RBRACK") {
      int check = derivationScan(Tree->nodes[3], table);
      if (check != 0) {
        throw(string("NEW expr rule, expr does not have type int"));
      }
      else {
        return 1;
      }
    }
    else { // factor is function call
      return 0;
    }
  }
  else if (word == "type") {
    if (rule == "type INT") {
      return 0;
    }
    return 1;
  }
  else if (word == "dcl") {
    return derivationScan(Tree->nodes[0], table);
  }
  else if (word == "test") {
    int check1 = derivationScan(Tree->nodes[0], table);
    int check2 = derivationScan(Tree->nodes[2], table);
    //cout << "check1: " << check1 << "  check2: " << check2 << endl;
    if (check1 != check2) {// if either variable in mult/div/mod is not int
      throw(string("Comparison operation between int and int*"));
    }
  }
  else if (rule == "statement DELETE LBRACK RBRACK expr SEMI") {
    int check = derivationScan(Tree->nodes[3], table);
    if (check != 1) {
      throw(string("Delete operation called on non int*"));
    }
  }
  else if (rule == "statement PRINTLN LPAREN expr RPAREN SEMI") {
    int check = derivationScan(Tree->nodes[2], table);
    if (check != 0) {
      throw(string("Print operation called on non int"));
    }
  }
  else if (rule == "statement lvalue BECOMES expr SEMI") {
    int check1 = derivationScan(Tree->nodes[0], table);
    int check2 = derivationScan(Tree->nodes[2], table);
    if (check1 != check2) {
      throw(string("Assignment operation between int and int*"));
    }
  }
  else if (rule == "dcls dcls dcl BECOMES NUM SEMI") {
    derivationScan(Tree->nodes[0], table);
    int check = derivationScan(Tree->nodes[1], table);
    if (check != 0) {
      throw(string("Setting type int* to NUM"));
    }
  }
  else if (rule == "dcls dcls dcl BECOMES NULL SEMI") {
    derivationScan(Tree->nodes[0], table);
    int check = derivationScan(Tree->nodes[1], table);
    if (check != 1) {
      throw(string("Setting type int to NULL"));
    }
  }
  else if (rule == "main INT WAIN LPAREN dcl COMMA dcl RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE") {
    int check1 = derivationScan(Tree->nodes[5], table);
    int check2 = derivationScan(Tree->nodes[11], table);
    //cout << Tree->nodes[5]->nodes[0]->rule;
    if (check1 != 0) {
      throw(string("Second parameter of Wain is not type int"));
    }
    if (check2 != 0) {
      throw(string("Return value of Wain is not type int"));
    }
    derivationScan(Tree->nodes[8], table); // check dcls
    derivationScan(Tree->nodes[9], table); // check statements
  }
  else if (rule == "procedure INT ID LPAREN params RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE") {
    int check = derivationScan(Tree->nodes[9], table);
    if (check != 0) {
      throw(string("Return value of a function is not type int"));
    }
    derivationScan(Tree->nodes[6], table); // check dcls
    derivationScan(Tree->nodes[7], table); // check statements
  }
  else {
    for (int i=0; i < Tree->nodes.size(); i++) {
      if (!(Tree->nodes[i]->ifTerminal)) {
        derivationScan(Tree->nodes[i], table);
      }
    }
  }
}

void declarationScan(Node* Tree, vector<vector<string>>* table, vector<string> functionList) {
  if (!(Tree->ifTerminal) && (Tree->rule == "factor ID LPAREN RPAREN" || Tree->rule == "factor ID LPAREN arglist RPAREN")) {
    string functionID = Tree->nodes[0]->tokens[1];
    //if (functionID != (*table)[0][0]) { // if not a recursive function call
      bool inTable = false;
      for (int i=1; i < table->size(); i++) { // Check if function ID is used as a local variable, thus cannot be called
        if ((*table)[i][0] == functionID) {
          inTable = true;
        }
      }
      if (inTable) {
        throw("Function ID " + functionID + " is declared as a local variable");
      }
    //}
    inTable = false;
    if (functionID != "wain") { // Check if function ID has been defined previously in functionList
      //cout<< "check wain for id: " << functionID << endl; 
      for (int i=0; i < functionList.size(); i++) {
        if (functionList[i] == functionID) {
          inTable = true;
        }
      }
    }
    if (!inTable) { // Check if ID has already been declared
      throw("ID " + functionID + " has not been declared");
    }
  }
  else if (!(Tree->ifTerminal)) {
    //cout << "rule:" << Tree->rule << endl;
    if (Tree->rule == "dcl type ID") {
      
      bool inTable = false;
      for (int i=1; i < table->size(); i++) {
        if ((*table)[i][0] == Tree->nodes[1]->tokens[1]) {
          inTable = true;
        }
      }
      
      if (inTable) { // Check if ID has already been declared
        throw("Redeclaration of ID " + Tree->nodes[1]->tokens[1]);
      }
      //cout << "ID:" << Tree->nodes[1]->tokens[1] << endl;

      vector<string> newDec;
      newDec.emplace_back(Tree->nodes[1]->tokens[1]);
      if (Tree->nodes[0]->rule == "type INT") {
        newDec.emplace_back("int");
      }
      else {
        newDec.emplace_back("int*");
      }
      table->emplace_back(newDec);
    }
    else {
      for (int a=0; a < Tree->nodes.size(); a++) {
        declarationScan(Tree->nodes[a], table, functionList);
      }
    }
  }
  else if (Tree->tokens[0] == "ID") {
    //cout << "ID:" << Tree->tokens[1] << endl;
    bool inTable = false;
    for (int i=0; i < table->size(); i++) {
      if ((*table)[i][0] == Tree->tokens[1]) {
        inTable = true;
      }
    }
    if (!inTable) { // Check if ID has already been declared
      throw("ID " + Tree->tokens[1] + " has not been declared");
    }
  }
}

void treeParse(Node* Tree, vector<Node*>* vec) {
  if (!(Tree->ifTerminal)) {
    istringstream iss(Tree->rule);
    string word1;
    string word2;
    iss >> word1 >> word2;
    if ((word1 == "procedures" && word2 == "main") || (word1 == "procedure" && word2 == "INT")) {
      vec->emplace_back(Tree);
    }
    else {
      for (int i=0; i < Tree->nodes.size() ; i++) {
        treeParse(Tree->nodes[i], vec);
      }
    }
  }
}

vector<string> generateFunctions(vector<Node*>* procedures) {
  vector<string> functionList;
  string function;
  for (int i=0; i < procedures->size(); i++) {
    if ((*procedures)[i]->rule == "procedures main") {
      function = "wain";
    }
    else {
      function = (*procedures)[i]->nodes[1]->tokens[1];
    }

    if (find(functionList.begin(), functionList.end(), function) != functionList.end()) {
      throw("redeclaration of function " + function);
    }
    else {
      functionList.emplace_back(function);
    }
  }
  return functionList;
}

vector<string> generateWainParameters(vector<Node*>* procedures) {
  vector<string> wainParameters;
  string parameterType;
  Node* wain;
  for (int i=0; i < procedures->size(); i++) {
    if ((*procedures)[i]->rule == "procedures main") {
      wain = (*procedures)[i]->nodes[0];
    }
  }
  if (wain->nodes[3]->nodes[0]->rule == "type INT") {
    parameterType = "int";
    wainParameters.emplace_back(parameterType);
  }
  else {
    parameterType = "int*";
    wainParameters.emplace_back(parameterType);
  }
  if (wain->nodes[5]->nodes[0]->rule == "type INT") {
    parameterType = "int";
    wainParameters.emplace_back(parameterType);
  }
  else {
    parameterType = "int*";
    wainParameters.emplace_back(parameterType);
  }

  return wainParameters;
}

vector<string> generateFuncParameters(vector<Node*>* procedures, string functionName) {
  vector<string> funcParameters;
  funcParameters.emplace_back(functionName);
  string parameterType;
  Node* func;
  for (int i=0; i < procedures->size(); i++) {
    if ((*procedures)[i]->nodes.size() != 1 && // if not wain
    (*procedures)[i]->nodes[1]->tokens[1] == functionName) { // if same function name
      func = (*procedures)[i]->nodes[3]; // set func to function parameters node
      if (func->nodes.size() ==0) {
        return funcParameters;
      }
    }
  }

  Node* paramList = func->nodes[0];
  while (paramList->nodes.size() == 3) { // while paramList is recursively defining parameters
    if (paramList->nodes[0]->nodes[0]->nodes.size() == 1) {
      parameterType = "int";
      funcParameters.emplace_back(parameterType);
    }
    else {
      parameterType = "int*";
      funcParameters.emplace_back(parameterType);
    }
    paramList = paramList->nodes[2];
  }
  if (paramList->nodes[0]->nodes[0]->nodes.size() == 1) {
    parameterType = "int";
    funcParameters.emplace_back(parameterType);
  }
  else {
    parameterType = "int*";
    funcParameters.emplace_back(parameterType);
  }

  return funcParameters;
}

int main() {
  Node* Tree = createTree();
  vector<Node*>* procedures = new vector<Node*>;
  treeParse(Tree, procedures);
  
  vector<vector<vector<string>>*> tables;
  try { 
    vector<string> functionList = generateFunctions(procedures); // generate function list and check for function redeclarations

    for (int i=0; i < procedures->size(); i++) {
      vector<vector<string>>* table = new vector<vector<string>>;
      Tree = (*procedures)[i];
      // Add functions names to table before scanning
      vector<string> functionSignature;
      string signatureID;
      if (Tree->rule != "procedures main") {
        signatureID = Tree->nodes[1]->tokens[1];
        functionSignature.emplace_back(signatureID);
        table->emplace_back(functionSignature);
      }
      else {
        signatureID = "wain";
        functionSignature.emplace_back(signatureID);
        table->emplace_back(functionSignature);
      }

      vector<string> currentFunctions;
      for (int k=0; k < functionList.size(); k++) {
        if (functionList[k] == signatureID) {
          currentFunctions.emplace_back(functionList[k]);
          break;
        }
        currentFunctions.emplace_back(functionList[k]);
      }
      declarationScan(Tree, table, currentFunctions);
      derivationScan(Tree, table);
      tables.emplace_back(table);
    }

    for (int i=0; i < tables.size() ; i++) {
      if (i != 0) {
        cerr << endl;
      }

      if ((*(tables[i]))[0][0] == "wain") { // if Wain procedure print this way
        vector<string> wainParameters = generateWainParameters(procedures);
        cerr << "wain " << wainParameters[0] << " " << wainParameters[1] << endl;

        for (int j=1; j < tables[i]->size() ; j++) {
          cerr << (*(tables[i]))[j][0] << " " << (*(tables[i]))[j][1] << endl;
        }
      }
      else { // if any other procedure print this way
        string functionName = (*(tables[i]))[0][0];

        vector<string> functionParameters = generateFuncParameters(procedures,functionName);
        // print all function parameters
        for (int j=0; j < functionParameters.size() ; j++) {
          if (j !=0 ) {cerr << " ";}
          cerr << functionParameters[j];
        }
        cerr << endl;

        for (int j=1; j < tables[i]->size() ; j++) {
          cerr << (*(tables[i]))[j][0] << " " << (*(tables[i]))[j][1] << endl;
        }
      }
    }
  }
  catch (string msg) { cerr << "ERROR: " << msg << endl; }

  delete Tree;
  delete procedures;
  for (int j=0; j < tables.size() ; j++) {
    delete tables[j];
  }
}
