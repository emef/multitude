#ifndef MATRIX_H
#define MATRIX_H

#include <map>
#include <string>
#include <vector>
#include "block.h"

class DMatrix {
 public:
  DMatrix(std::vector<std::shared_ptr<MemoryBlock>> memoryBlocks,
          std::vector<std::shared_ptr<RemoteBlock>> remoteBlocks);

  template<typename T>
  typename T::Result apply(std::shared_ptr<T> op, typename T::Args& opArgs);

  template<typename T>
  std::shared_ptr<DMatrix> transform(std::shared_ptr<T> op,
                                     typename T::Args& opArgs);

  std::map<std::string, std::shared_ptr<MemoryBlock>> getMemoryBlocks() {
    return memoryBlocks;
  }

  std::map<std::string, std::shared_ptr<RemoteBlock>> getRemoteBlocks() {
    return remoteBlocks;
  }

 private:
  std::map<std::string, std::shared_ptr<MemoryBlock>> memoryBlocks;
  std::map<std::string, std::shared_ptr<RemoteBlock>> remoteBlocks;
};

std::unique_ptr<DMatrix> loadFromFiles(BlockLoader& blockLoader,
                                       std::vector<std::string> paths);

#endif
