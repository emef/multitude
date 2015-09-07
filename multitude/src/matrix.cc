#include <map>
#include <string>
#include <vector>
#include "../include/block.h"
#include "../include/loader.h"
#include "../include/matrix.h"

namespace Multitude {

DMatrix::DMatrix(std::vector<std::shared_ptr<MemoryBlock>> memoryBlocks,
                 std::vector<std::shared_ptr<RemoteBlock>> remoteBlocks) {

  for (auto block : memoryBlocks) {
    this->memoryBlocks[block->getId()] = block;
  }

  for (auto block : remoteBlocks) {
    this->remoteBlocks[block->getId()] = block;
  }
}

std::unique_ptr<DMatrix> loadFromFiles(BlockLoader& blockLoader,
                                       std::vector<std::string> paths) {
  std::vector<std::shared_ptr<MemoryBlock>> memoryBlocks;
  for (auto path : paths) {
    auto blocks = blockLoader.loadToMemory(path);
    memoryBlocks.insert(memoryBlocks.end(), blocks.begin(), blocks.end());
  }

  std::vector<std::shared_ptr<RemoteBlock>> empty;
  return std::make_unique<DMatrix>(memoryBlocks, empty);
}

}
