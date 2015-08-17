#include <chrono>
#include <fstream>
#include <future>
#include <functional>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>
#include "../block.h"
#include "../loader.h"
#include "../matrix.h"
#include "../thread_pool.h"
#include "../ops.h"

typedef std::chrono::high_resolution_clock Time;
typedef std::chrono::milliseconds ms;
typedef std::chrono::duration<float> fsec;

long millisSince(std::chrono::time_point<Time> t0) {
  fsec fs = Time::now() - t0;
  ms d = std::chrono::duration_cast<ms>(fs);
  return d.count();
}


void generateDataFile(std::string path, int rows, int cols) {
  std::ofstream file(path, std::ios::out | std::ios::binary);
  file.write((char*)&cols, sizeof(int));
  double row[cols];

  for (int i=0; i<rows; i++) {
    for (int j=0; j<cols; j++) {
      row[j] = i*cols + j;
    }
    file.write((char*)row, sizeof(row));
  }
}

bool verifyGeneratedFile(std::string path) {
  BlockLoader loader;
  auto mBlocks = loader.loadToMemory(path);
  double last = -1;
  for (auto const &block : mBlocks) {
    auto &bData = block->getBlockData();
    for (int i=0; i<bData.getRows(); i++) {
      for (int j=0; j<bData.getCols(); j++) {
        int idx = i*bData.getCols() + j;
        double next = bData.getData()[idx];
        if (next != 1 + last) {
          return false;
        }
        last = next;
      }
    }
  }

  return true;
}

int main(int argc, char *argv[]) {
  std::cout << std::fixed;

  std::string path = "/tmp/ex.mult";
  /*
  generateDataFile(path, 1000000, 200);
  if (!verifyGeneratedFile(path)) {
    std::cout << "FAILED" << std::endl;
  }
  */

  BlockLoader blockLoader;
  auto t0 = Time::now();
  auto matrix = loadFromFiles(blockLoader, {path, path, path, path});
  std::cout << "LOADED (" << millisSince(t0) << "ms)" << std::endl;

  t0 = Time::now();
  auto count = COUNT.apply(*matrix, {}).count;
  std::cout << "COUNT=" << count << " (" << millisSince(t0) << "ms)" << std::endl;

  t0 = Time::now();
  auto sum0 =0;
  for (int i=0; i<1000; i++)
    sum0 = SUM.apply(*matrix, {0}).sum;
  std::cout << "SUM0=" << sum0 << " (" << millisSince(t0) << "ms)" << std::endl;

  t0 = Time::now();
  auto max0 = MAX.apply(*matrix, {0}).max;
  std::cout << "MAX0=" << max0 << " (" << millisSince(t0) << "ms)" << std::endl;

  t0 = Time::now();
  auto min0 = MIN.apply(*matrix, {0}).min;
  std::cout << "MIN0=" << min0 << " (" << millisSince(t0) << "ms)" << std::endl;

  return 0;
}
