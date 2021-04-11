#include <list>

class Allocator{
  public:
    Allocator(unsigned int space): max(space){};
    void run();
  private:
    unsigned int max;
};
