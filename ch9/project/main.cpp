#include <iostream>
#include "allocator.h"
extern "C" {
  #include "../../lib/parser.h"
}

using namespace std;

int main(int argc, char** argv){
  cout << "Run allocator!\n";
  if(argc <= 1){
    cout << "No argument!\n";
    return 1;
  }
  if(!is_integer(argv[1])){
    cout << "Invalid argument!\n";
    return 1;
  }
  unsigned int space;
  try{
    space = stoul(argv[1]);
  }
  catch(const exception& err){
    cout << err.what() << "\n";
    return 1;
  }
  Allocator alloc(space);
  alloc.run();
  return 0;
}
