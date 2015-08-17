#ifndef OPS_H
#define OPS_H

#include <algorithm>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <set>
#include "matrix.h"
#include "thread_pool.h"

static ThreadPool pool(std::thread::hardware_concurrency());

template<typename T>
class ValueOperation {
 public:
  typename T::Result apply(DMatrix& matrix, typename T::Args args) {
    std::vector<std::future<typename T::BlockResult>> resultFutures;
    for (auto const &entry : matrix.getMemoryBlocks()) {
      auto const &block = entry.second;
      std::function<typename T::BlockResult ()> producer = [&]() {
        return t.apply(*block, args);
      };

      resultFutures.push_back(pool.schedule(producer));
    }

    std::vector<typename T::BlockResult> results;
    for (auto &resultFuture : resultFutures) {
      results.push_back(resultFuture.get());
    }

    return t.combine(results);
  }

 private:
  T t;
};

class Count {
 public:
  struct Args {};

  struct Result {
    Result(long count) : count(count) {}
    const long count;
  };

  struct BlockResult {
    BlockResult(long count) : count(count) {}
    const long count;
  };

  BlockResult apply(const MemoryBlock& block, Args& args) {
    auto const &data = block.getBlockData();
    return {data.getRows()};
  }

  Result combine(std::vector<BlockResult> results) {
    long count = 0;
    for (auto const &result : results) {
      count += result.count;
    }
    return {count};
  }
};

class SumColumn {
 public:
  struct Args {
    Args(int col) : col(col) {}
    const int col;
  };

  struct BlockResult {
    BlockResult(double sum) : sum(sum) {}
    const double sum;
  };

  struct Result {
    Result(double sum) : sum(sum) {}
    const double sum;
  };

  BlockResult apply(const MemoryBlock& block, Args& args) {
    auto const &blockData = block.getBlockData();
    auto const data = blockData.getData();
    double sum = 0;
    int idx = args.col;
    int cols = blockData.getCols();
    int rows = blockData.getRows();
    for (int i=0; i<rows; i++) {
      sum += data[idx];
      idx += cols;
    }
    return {sum};
  }

  Result combine(std::vector<BlockResult> results) {
    double sum = 0;
    for (auto const &result : results) {
      sum += result.sum;
    }

    return sum;
  }
};

class MaxColumn {
 public:
  struct Args {
    Args(int col) : col(col) {}
    const int col;
  };

  struct BlockResult {
    BlockResult(double max) : max(max) {}
    const double max;
  };

  struct Result {
    Result(double max) : max(max) {}
    const double max;
  };

  BlockResult apply(const MemoryBlock& block, Args& args) {
    auto const &blockData = block.getBlockData();
    auto const data = blockData.getData();
    double max = data[0];
    for (int i=1; i<blockData.getRows(); i++) {
      int idx = i * blockData.getCols() + args.col;
      max = std::max(max, data[idx]);
    }

    return {max};
  }

  Result combine(std::vector<BlockResult> results) {
    double max = results[0].max;
    for (int i=1; i<results.size(); i++) {
      max = std::max(max, results[i].max);
    }

    return {max};
  }
};

class MinColumn {
 public:
  struct Args {
    Args(int col) : col(col) {}
    const int col;
  };

  struct BlockResult {
    BlockResult(double min) : min(min) {}
    const double min;
  };

  struct Result {
    Result(double min) : min(min) {}
    const double min;
  };

  BlockResult apply(const MemoryBlock& block, Args& args) {
    auto const &blockData = block.getBlockData();
    auto const data = blockData.getData();
    double min = data[0];
    for (int i=1; i<blockData.getRows(); i++) {
      int idx = i * blockData.getCols() + args.col;
      min = std::min(min, data[idx]);
    }

    return {min};
  }

  Result combine(std::vector<BlockResult> results) {
    double min = results[0].min;
    for (int i=1; i<results.size(); i++) {
      min = std::min(min, results[i].min);
    }

    return {min};
  }
};


ValueOperation<Count> COUNT;
ValueOperation<SumColumn> SUM;
ValueOperation<MaxColumn> MAX;
ValueOperation<MinColumn> MIN;

#endif
