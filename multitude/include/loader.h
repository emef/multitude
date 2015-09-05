#ifndef LOADER_H
#define LOADER_H

#include <future>
#include <memory>
#include <string>
#include <vector>

class BlockLoader {
 public:
  std::vector<std::shared_ptr<MemoryBlock>> loadToMemory(std::string path);
};

#endif
