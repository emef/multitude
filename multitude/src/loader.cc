#include <cmath>
#include <fstream>
#include <future>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "../include/block.h"
#include "../include/loader.h"

#define MIN_BLOCK 64000
#define HEADER_SIZE sizeof(int)
#define ID_LENGTH 64

namespace Multitude {

/**
 * File statistics of multitude-encoded binary file.
 */
struct FileStats {
  FileStats(int cols, long size) : cols(cols), size(size) {}
  int cols;   ///< Number of columns in the matrix.
  long size;  ///< Total size of file in bytes.
};

// File stats utilities.
std::unique_ptr<FileStats> stat(std::string path);
std::string nextBlockId();
int determineNumBlocks(FileStats& fileStats);
long getBlockSize(FileStats& fileStats, int numBlocks);


/**
 * Load a memory block from file and block descriptor.
 */
std::shared_ptr<MemoryBlock> loadFromDescriptor(
    const FileStats& fileStats,
    std::unique_ptr<BlockDescriptor> descriptor) {

  auto location = descriptor->getLocation();

  long rowBytes = fileStats.cols * sizeof(double);
  long rows = location.getLength() / rowBytes;
  long doubles = location.getLength() / sizeof(double);

  auto data = std::make_unique<double[]>(doubles);
  std::ifstream file(location.getPath(), std::ios::in | std::ios::binary);
  file.seekg(location.getOffset());
  file.read((char*)data.get(), location.getLength());

  auto blockData = std::make_unique<BlockData>(rows, fileStats.cols,
                                               std::move(data));
  return std::make_shared<MemoryBlock>(nextBlockId(),
                                       std::move(descriptor),
                                       std::move(blockData));
}

/**
 * Load a local file into memory as a sequence of MemoryBlocks.
 *
 * @param path - Path to binary matrix file.
 * @return - Vector of memory blocks.
 */
std::vector<std::shared_ptr<MemoryBlock>> BlockLoader::loadToMemory(
    std::string path) {

  std::unique_ptr<FileStats> fileStats = stat(path);
  FileStats& statsRef = *fileStats;
  int numBlocks = determineNumBlocks(*fileStats);
  long blockSize = getBlockSize(*fileStats, numBlocks);
  std::vector<std::future<std::shared_ptr<MemoryBlock>>> blockFutures;

  for (int i=0; i<numBlocks; i++) {
    long offset = HEADER_SIZE + i * blockSize;
    long length = (i == numBlocks-1) ? fileStats->size - offset : blockSize;
    auto location = std::make_shared<DataLocation>(path, offset, length);
    auto descriptor = std::make_unique<BlockDescriptor>(location);
    auto blockFuture = std::async(std::launch::async, loadFromDescriptor,
                                  statsRef, std::move(descriptor));
    std::cout << i << std::endl;
    blockFutures.push_back(std::move(blockFuture));
  }

  std::vector<std::shared_ptr<MemoryBlock>> blocks;
  for (auto &blockFuture : blockFutures) {
    blocks.push_back(blockFuture.get());
  }

  return blocks;
}

/**
 * Get stats of multitude-encoded binary file.
 */
std::unique_ptr<FileStats> stat(std::string path) {
  std::ifstream file(path, std::ios::in | std::ios::binary);
  if (!file.is_open()) {
    return NULL;
  }

  int cols;
  file.read((char*)&cols, sizeof(int));
  file.seekg(0, file.end);
  long size = file.tellg();

  return std::make_unique<FileStats>(cols, size);
}

/**
 * Determine best number of blocks to load a file into.
 */
int determineNumBlocks(FileStats& fileStats) {
  long numCores = std::thread::hardware_concurrency();
  if (fileStats.size > numCores * MIN_BLOCK) {
    return numCores;
  }

  return (int)(ceil(fileStats.size / MIN_BLOCK));
}

/**
 * Determine the next unique block ID.
 *
 * @return - Unique block identifier.
 */
std::string nextBlockId() {
  static const char alphanum[] =
      "0123456789"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz";

  static const int n = sizeof(alphanum) - 1;

  char s[ID_LENGTH+1];
  for (int i = 0; i < ID_LENGTH; ++i) {
    int idx = rand() / (RAND_MAX / n + 1);
    s[i] = alphanum[idx];
  }

  s[ID_LENGTH] = 0;
  return std::string(s);
}

/**
 * Determine the block size for a file and target number of blocks.
 */
long getBlockSize(FileStats& fileStats, int numBlocks) {
  long rowBytes = fileStats.cols * sizeof(double);
  long blockSize = fileStats.size / numBlocks;

  // Round up to nearest multiple of rowBytes
  return ((blockSize + rowBytes - 1) / rowBytes) * rowBytes;
}

}
