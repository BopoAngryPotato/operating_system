#include <iostream>
#include <string>
#include "allocator.h"

#define EXIT "X"

using namespace std;

void Allocator::run(){
  string buf;
  while(buf != EXIT){
    cout << "allocator>";
    getline(cin, buf);
    cout << buf << '\n';
  }
}
