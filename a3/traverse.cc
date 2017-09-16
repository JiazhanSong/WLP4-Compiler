#include <iostream>
#include <vector>
#include <string>

using namespace std;

struct Node {
  int value;
  int count;
  vector<Node*> nodes;

  Node(int num, int childCount): value(num), count(childCount) {}
  ~Node() {
    for (int a=0; a < count; a++) {
      delete nodes[a];
    }
  }
};

Node* createTree() {
  int value;
  cin >> value;
  int childCount;
  cin >> childCount;

  Node* node = new Node(value, childCount);

  int counter = 0;
  while(childCount != counter) {
    node->nodes.push_back(createTree());
    counter++;
  }

  return node;
}

void postOrder(Node* Tree) {
  if (Tree->count == 0) {
    cout << Tree->value << " 0" << endl;
  }
  else {
    for (int a=0; a < Tree->count; a++) {
      postOrder(Tree->nodes[a]);
    }
    cout << Tree->value << " " << Tree->count << endl;
  }
}


int main() {
  Node* Tree = createTree();
  postOrder(Tree);
  delete Tree;
}
