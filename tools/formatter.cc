#include <fstream>
#include <iostream>

void help(char *progName) {
  std::cerr << progName << " INPUT OUTPUT" << std::endl;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    help(argv[0]);
    return 1;
  }

  std::string inputPath(argv[1]);
  std::string outputPath(argv[2]);

}
