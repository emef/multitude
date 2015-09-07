#include <map>
#include <string>
#include <vector>
#include "../include/block.h"
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

}
