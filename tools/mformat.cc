#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#define BUFSIZE 64000

void help(char *progName) {
  std::cerr << progName << " INPUT OUTPUT" << std::endl;
}

int numColsInCsv(std::stringstream ssLine) {
  double d;
  int n = 0;
  while (ssLine >> d) {
    n++;
    if (ssLine.peek() == ',') {
      ssLine.ignore();
    }
  }

  return n;
}

void readLineIntoDoubles(std::stringstream ssLine, double* out) {
  double d;
  int i = 0;
  while (ssLine >> d) {
    out[i++] = d;
    if (ssLine.peek() == ',') {
      ssLine.ignore();
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    help(argv[0]);
    return 1;
  }

  std::ifstream infile(argv[1], std::ios::in);
  std::ofstream outfile(argv[2], std::ios::out | std::ios::binary);

  char buffer[BUFSIZE];
  infile.getline(buffer, BUFSIZE);
  if (infile.fail()) {
    std::cerr << "Cannot read input csv" << std::endl;
    return 1;
  }

  int cols = numColsInCsv(std::stringstream(buffer));
  double* doubles = new double[cols];

  outfile.write((char*) &cols, sizeof(int));
  infile.seekg(0);
  while (!infile.eof()) {
    infile.getline(buffer, BUFSIZE);
    readLineIntoDoubles(std::stringstream(buffer), doubles);
    outfile.write((char*) doubles, cols * sizeof(double));
  }

}
