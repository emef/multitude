#ifndef CONTEXT_H
#define CONTEXT_H

#include <memory>
#include <string>
#include "matrix.h"

namespace Multitude {

/**
 * Distributed system context, used to load/save matrices.
 */
class DContext {
 public:
  std::unique_ptr<DMatrix> binaryFile(std::string path);
};

}
#endif
