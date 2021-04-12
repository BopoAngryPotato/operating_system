#include <iostream>
#include "allocator.h"

#define EXIT "X"
#define SPACE ' '

using namespace std;

void Allocator::run(){
  string buf;
  while(buf != EXIT){
    cout << "allocator>";
    getline(cin, buf);
    vector<string> params = parseCmd(buf);
    for(size_t idx = 0; idx < params.size(); idx++)
      cout << params[idx] << ' ';
    cout << '\n';
  }
}

vector<string> Allocator::parseCmd(const string& cmd){
  vector<string> ret;
  size_t pos = 0;
  size_t end = cmd.find(SPACE, pos);
  while(end != string::npos){
    ret.push_back(cmd.substr(pos, end - pos));
    pos = end + 1;
    end = cmd.find(SPACE, pos);
  }
  ret.push_back(cmd.substr(pos));
  return ret;
}
