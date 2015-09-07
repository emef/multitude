#ifndef MATRIX_H
#define MATRIX_H

#include <map>
#include <string>
#include <vector>
#include "block.h"

namespace Multitude {

/**
 * Distributed matrix represented as a collection of "blocks". Each
 * block contains a sequential subset of the rows in the
 * matrix. Blocks can be local to the current program (memory blocks)
 * or located in another process potentially running on another
 * machine (network blocks, not implemented yet).
 */
class DMatrix {
 public:
  DMatrix(std::vector<std::shared_ptr<MemoryBlock>> memoryBlocks,
          std::vector<std::shared_ptr<RemoteBlock>> remoteBlocks);

  /// Map of all memory blocks indexed by their unique block ID.
  std::map<std::string, std::shared_ptr<MemoryBlock>> getMemoryBlocks() {
    return memoryBlocks;
  }

  /// Map of all network blocks indexed by their unique block ID.
  std::map<std::string, std::shared_ptr<RemoteBlock>> getRemoteBlocks() {
    return remoteBlocks;
  }

 private:
  std::map<std::string, std::shared_ptr<MemoryBlock>> memoryBlocks;
  std::map<std::string, std::shared_ptr<RemoteBlock>> remoteBlocks;
};

}

#endif
