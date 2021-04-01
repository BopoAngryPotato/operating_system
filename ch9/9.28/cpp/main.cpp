#include <string>
#include <iostream>
extern "C" {
  #include "../../../lib/parser.h"
}

using namespace std;
unsigned int PAGE_SIZE = 1 << 12;

int main(int argc, char** argv){
  cout << "Run address interpretor!\n";
  if(argc <= 1){
    cout << "No argument!\n";
    return 1;
  }
  if(!is_integer(argv[1])){
    cout << "Invalid argument!\n";
    return 1;
  }
  unsigned int address;
  try{
    address = stoul(argv[1]);
  }
  catch(const exception& err){
    cout << err.what() << "\n";
    return 1;
  }
  unsigned int page_number = address / PAGE_SIZE;
  unsigned int offset = address % PAGE_SIZE;
  cout << "The address " << address << " contains (page size " << PAGE_SIZE <<"):\n";
  cout << "Page number = " << page_number << "\n";
  cout << "Offset = " << offset << "\n";
  return 0;
}
