#include <list>
#include <vector>
#include <string>

class Allocator{
  public:
    Allocator(unsigned int space);
    void run();
  private:
    class Block{
      public:
        Block();
        Block(unsigned int, unsigned int, std::string, bool);
        Block(const Block&);
        unsigned int first, last;
        std::string process;
        bool used;
    };

    std::vector<std::string> parseCmd(const std::string&);
    void request(const std::vector<std::string>&);
    void status(const std::vector<std::string>&) const;

    bool checkConsistent() const;

    unsigned int max;
    std::list<Block> record;
};
