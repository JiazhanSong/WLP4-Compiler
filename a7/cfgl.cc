/*
   CFGrl Version 1 -- converts .cfg-r file to equivalent .cfg file

   Author:   Adam Roegiest
   Version:  1.0

   Input:  .cfg-r file with a single derivation
   Output: .cfg file with abbreviated forward leftmost version of the derivation

   Literal translation of CFGrl Version 2 by Ondrej Lhotak. Available (currently) at:

     http://www.student.cs.uwaterloo.ca/~cs241/a08/CFGrl.java

*/


#include <set>
#include <string>
#include <iostream>
#include <list>
#include <sstream>
#include <stack>
#include "wlp4parse.cc"
using namespace std;

set<string> terms;
set<string> nonterms;
set<string> prods;
string start;

struct Tree {
    string rule;
    list<Tree*> children;

    ~Tree() {
        for(list<Tree*>::iterator it=children.begin(); it != children.end(); it++) {  // delete all subtrees
            delete (*it);
        }
    }
};

void readsyms(set<string> &t,std::ifstream& data) {
    int n;
    string temp;
    getline(data,temp);
    istringstream iss(temp);
    iss >> n;
    if (iss.fail()) return;
    for(int i = 0; i < n; i++) {
        getline(data,temp);
        t.insert(temp);
    }
}

void traverse(Tree *t, int d) {
    for(int i = 0; i < d; i++) cout << " ";
    cout << t->rule << endl; // print root
    for(list<Tree*>::iterator it=(t->children).begin(); it != (t->children).end(); it++) {  // print all subtrees
        traverse(*it, d+1);
    }
}

void dump(set<string> &h) {
    cout << h.size() << endl;
    for(set<string>::iterator it=h.begin(); it != h.end(); it++) {
        cout << *it << endl;
    }
}

void popper(stack<Tree *> &myStack, list<string> &rhs, string rule) {
    Tree *n = new Tree();
    n->rule = rule;
    for(list<string>::iterator it=rhs.begin(); it != rhs.end(); it++){
        Tree *tmp = myStack.top();
        n->children.push_front(tmp);
        myStack.pop();
    }
    myStack.push(n);
}

Tree* lrdo(std::ifstream& data) {
    stack<Tree*> myStack;
    string l; // lhs symbol
    do {
        string f;
        getline(data,f);
        list<string> r; // rhs symbols
        istringstream iss(f);
        iss >> l; // lhs symbol
        string s;
        while(iss >> s) {
            if(nonterms.count(s) > 0) r.push_back(s); // only non-terminals
        }
        popper(myStack, r, f); // reduce rule
    } while(start != l);
    return myStack.top();
}

int main(){
    runMe();
    /*std::ifstream data;
    data.open("cfgr");
    readsyms(terms,data); // read terminals
    readsyms(nonterms,data); // read nonterminals
    getline(data,start); // read start symbol
    readsyms(prods,data); // read production rules
    
    Tree *parsetree = lrdo(data); // read reverse rightmost derivation
    traverse(parsetree, 0); // write forward leftmost derivation
    delete parsetree;*/
}