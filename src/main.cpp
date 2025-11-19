#include <iostream>

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <idl_file>" << std::endl;
    return 1;
  }
  return 0;
}