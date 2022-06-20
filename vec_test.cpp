#include <iostream>
#include <vector>

int main() {
  std::vector< int > vec;
  for (std::vector< int >::iterator it = vec.begin(); it != vec.end(); ++it) {
    std::cout << *it << std::endl;
  }
}
