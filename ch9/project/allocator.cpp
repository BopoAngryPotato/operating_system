#include <iostream>
#include <set>
#include "allocator.h"
extern "C" {
  #include "../../lib/parser.h"
}

#define REQUEST "RQ"
#define RELEASE "RL"
#define COMPACT "C"
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
    std::vector<string> params = parseCmd(buf);
    while(!params.empty()){
      if(params[0].empty()) params.erase(params.begin());
      else break;
    }
    if(params.empty()) continue;
    if(params[0] == REQUEST) request(params);
    else if(params[0] == RELEASE) release(params);
    else if(params[0] == COMPACT) compact(params);
    else if(params[0] == STATUS) status(params);
    else if(params[0] == EXIT) continue;
    else cout << "Invalid command\n";
    if(!checkConsistent()){
      cout << "Corrupt memory. Abort!\n";
      break;
    }
  }
}

std::vector<string> Allocator::parseCmd(const string& cmd){
  std::vector<string> ret;
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

void Allocator::request(const std::vector<string>& params){
  if(!checkConsistent()) return;
  if(params.size() < 4){
    cout << "Insufficient arguments.\n";
    return;
  }
  bool duplicate = false;
  for(list<Block>::iterator itr = record.begin(); itr != record.end(); itr++){
    if(itr->process == params[1]){
      duplicate = true;
      break;
    }
  }
  if(duplicate){
    cout << "Duplicated process name.\n";
    return;
  }
  if(!is_integer(params[2].c_str())){
    cout << "Invalid memory size.\n";
    return;
  }
  unsigned int space;
  try{
    space = stoul(params[2]);
  }
  catch(const exception& err){
    cout << "Memory size error: " << err.what() << "\n";
    return;
  }
  set<string> strategy_set{{"F", "B", "W"}};
  if(strategy_set.find(params[3]) == strategy_set.end()){
    cout << "Invalid stragety.\n";
    return;
  }
  list<Block>::iterator blk = record.end();
  for(list<Block>::iterator itr = record.begin(); itr != record.end(); itr++){
    if(itr->used) continue;
    if(itr->last - itr->first + 1 >= space){
      if(params[3] == "F"){
        blk = itr;
        break;
      }
      else if(params[3] == "B"){
        if(blk == record.end()) blk = itr;
        else if(blk->last - itr->first + 1 < itr->last - itr->first + 1) blk = itr;
      }
      else if(params[3] == "W"){
        if(blk == record.end()) blk = itr;
        else if(blk->last - itr->first + 1 > itr->last - itr->first + 1) blk = itr;
      }
      else return;
    }
  }
  if(blk == record.end()){
    cout << "No block available. Reject.\n";
    return;
  }
  Block n_blk{blk->first, blk->first + space - 1, params[1], true};
  blk->first = n_blk.last + 1;
  record.insert(blk, n_blk);
  cout << "Allocated.\n";
}

void Allocator::release(const std::vector<string>& params){
  if(!checkConsistent()) return;
  if(params.size() < 2){
    cout << "Insufficient arguments.\n";
    return;
  }
  list<Block>::iterator target = record.end();
  for(list<Block>::iterator itr = record.begin(); itr != record.end(); itr++){
    if((*itr).process == params[1]){
      target = itr;
      break;
    }
  }
  if(target == record.end()){
    cout << "No process found.\n";
    return;
  }
  target->used = false;
  target->process = "";
  if(target != record.begin()){
    list<Block>::iterator pre = target;
    pre--;
    if(pre->used == false){
      target->first = pre->first;
      record.erase(pre);
    }
  }
  if(target != --record.end()){
    list<Block>::iterator su = target;
    su++;
    if(su->used == false){
      su->first = target->first;
      record.erase(target);
    }
  }
}

void Allocator::compact(const std::vector<string>& params){
  if(!checkConsistent()) return;
  list<Block> compacted;
  for(list<Block>::const_iterator itr = record.begin(); itr != record.end(); itr++){
    if(itr->used){
      list<Block>::const_reverse_iterator tail = compacted.crbegin();
      unsigned int first, last;
      if(tail != compacted.crend()){
        first = tail->last + 1;
        last = first + itr->last - itr->first;
      }
      else{
        first = 0;
        last = itr->last - itr->first;
      }
      compacted.push_back({first, last, itr->process, true});
    }
  }
  list<Block>::const_reverse_iterator tail = compacted.crbegin();
  unsigned int remain = 0;
  if(tail != compacted.crend()) remain = tail->last + 1;
  if(remain < max) compacted.push_back({remain, max - 1, "", false});
  record = move(compacted);
}

void Allocator::status(const std::vector<string>& params) const{
  if(!checkConsistent()) return;
  for(list<Block>::const_iterator itr = record.begin(); itr != record.end(); itr++){
    cout << "Addresses [" << itr->first << ":" << itr->last << "] ";
    if(itr->used) cout << "Process " << itr->process;
    else cout << "Unused";
    cout << "\n";
  }
}

bool Allocator::checkConsistent() const{
  for(list<Block>::const_iterator itr = record.begin(); itr != record.end(); itr++){
    if(itr == record.begin()){
      if(itr->first != 0) return false;
    }
    else{
      list<Block>::const_iterator tmp = itr;
      list<Block>::const_iterator pre = --tmp;
      if(itr->first != pre->last+1) return false;
    }
    if(itr->first > itr->last) return false;
    if(itr == --record.end()){
      if(itr->last != max-1) return false;
    }
  }
  return true;
}

Allocator::Block::Block(): first(0), last(0), used(false){}

Allocator::Block::Block(unsigned int pfirst, unsigned int plast, string pprocess = "", bool pused = false): first(pfirst), last(plast), process(pprocess), used(pused){
  if(first > last) throw runtime_error("Error");
}

Allocator::Block::Block(const Block& blk): first(blk.first), last(blk.last), process(blk.process), used(blk.used){
  if(first > last) throw runtime_error("Error");
}
