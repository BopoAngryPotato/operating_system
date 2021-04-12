#include <list>
#include <vector>
#include <string>

class Allocator{
  public:
    Allocator(unsigned int space): max(space){};
    void run();
  private:
    class Block{
      public:
        Block():first(0), last(0), used(false){};
        unsigned int getFirst() const{ return first; };
        unsigned int getLast() const{ return last; };
        std::string getProcess() const{ return process; };
        bool getUsed() const{ return used; };
      private:
        unsigned int first, last;
        std::string process;
        bool used;
    };

    std::vector<std::string> parseCmd(const std::string&);

    unsigned int max;
    std::list<Block> record;
};
