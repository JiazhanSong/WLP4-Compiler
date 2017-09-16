#include <string>
#include <iostream>
// Use only the neeeded aspects of each namespace
using std::string;
using std::to_string;
using std::endl;
using std::cerr;
using std::cin;
using std::cout;

int binsearch(int* arr, int size, int value) {
  int partitionSize = 0;
  int index = 0;
  int returnIndex = 0;

  partitionSize = size;
  index = size / 2;
  returnIndex = 0 - 1;

  while (partitionSize > 0) {
    if (*(arr + index) == value) {
      returnIndex = index; 
      partitionSize = 0;
    }
    else {
      if (*(arr + index) < value) {
        if (partitionSize % 2 == 0) {
          partitionSize = partitionSize / 2 - 1;
        }
        else {
          partitionSize = partitionSize / 2;
        }
        index = index + partitionSize / 2 + 1;     
      }
      else {
        partitionSize = partitionSize / 2;
        index = index - partitionSize + (partitionSize / 2);
      }
    }
  }

  return returnIndex;
}

int main() {
  int stack_array[4] = { 4 ,7, 10, 42};
  std::cout << binsearch(stack_array, 5, 4) << endl;
  std::cout << binsearch(stack_array, 5, 7) << endl;
  std::cout << binsearch(stack_array, 5, 42) << endl;
  std::cout << binsearch(stack_array, 5, 10) << endl;
}