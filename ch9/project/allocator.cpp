#include <iostream>
#include "allocator.h"

#define STATUS "STAT"
#define EXIT "X"
#define SPACE ' '

using namespace std;

Allocator::Allocator(unsigned int space): max(space){
  record.push_back({0, max-1, "", false});
}

void Allocator::run(){
  string buf;
  while(buf != EXIT){
    cout << "allocator>";
    getline(cin, buf);
    vector<string> params = parseCmd(buf);
    while(!params.empty()){
      if(params[0].empty()) params.erase(params.begin());
      else break;
    }
    if(params.empty()) continue;
    if(params[0] == STATUS) status(params);
    else if(params[0] == EXIT) continue;
    else cout << "Invalid command\n";
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

void Allocator::status(const vector<string>& params) const{
  for(list<Block>::const_iterator itr = record.begin(); itr != record.end(); itr++){
    cout << "Addresses [" << itr->first << ":" << itr->last << "] ";
    if(itr->used) cout << "Process " << itr->process;
    else cout << "Unused";
    cout << "\n";
  }
}

Allocator::Block::Block(): first(0), last(0), used(false){}

Allocator::Block::Block(unsigned int pfirst, unsigned int plast, string pprocess = "", bool pused = false): first(pfirst), last(plast), process(pprocess), used(pused){
  if(first > last) throw runtime_error("Error");
}

Allocator::Block::Block(const Block& blk): first(blk.first), last(blk.last), process(blk.process), used(blk.used){
  if(first > last) throw runtime_error("Error");
}
