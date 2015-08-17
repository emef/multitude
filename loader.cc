#include <fstream>
#include <future>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "block.h"
#include "loader.h"

#define HEADER_SIZE sizeof(int)
#define ID_LENGTH 64

struct FileStats {
  FileStats(int cols, long size) : cols(cols), size(size) {}
  int cols;
  long size;
};

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

int determineNumBlocks(FileStats& fileStats) {
  return std::thread::hardware_concurrency();
}

std::string nextId() {
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
  return std::make_shared<MemoryBlock>(nextId(),
                                       std::move(descriptor),
                                       std::move(blockData));
}

long getBlockSize(FileStats& fileStats, int numBlocks) {
  long rowBytes = fileStats.cols * sizeof(double);
  long blockSize = fileStats.size / numBlocks;

  // Round up to nearest multiple of rowBytes
  return ((blockSize + rowBytes - 1) / rowBytes) * rowBytes;
}

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
    blockFutures.push_back(std::move(blockFuture));
  }

  std::vector<std::shared_ptr<MemoryBlock>> blocks;
  for (auto &blockFuture : blockFutures) {
    blocks.push_back(blockFuture.get());
  }

  return blocks;
}
