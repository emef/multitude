#ifndef LOADER_H
#define LOADER_H

#include <future>
#include <memory>
#include <string>
#include <vector>

namespace Multitude {

/**
 * Loads binary data from input paths into data blocks.
 */
class BlockLoader {
 public:
  std::vector<std::shared_ptr<MemoryBlock>> loadToMemory(std::string path);
};

}
#endif
