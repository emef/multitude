#ifndef OPS_H
#define OPS_H

#include <algorithm>
#include <iostream>
#include <mutex>
#include <random>
#include <set>
#include <thread>
#include <vector>
#include "matrix.h"
#include "thread_pool.h"

namespace Multitude {

static ThreadPool pool(std::thread::hardware_concurrency());

/**
 * Operation on a distributed matrix that produces a value by combining
 * results of applying an operation to individual blocks.
 *
 *
 */
template<typename T>
class ValueOperation {
 public:
  /**
   * Apply the templated operation to matrix subject to operation args.
   *
   * @param matrix - Matrix to apply operation t.
   * @param args - Operation arguments.
   * @return - Operation result.
   */
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

  BlockResult apply(const MemoryBlock& block, const Args& args) {
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

  BlockResult apply(const MemoryBlock& block, const Args& args) {
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

  BlockResult apply(const MemoryBlock& block, const Args& args) {
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

class RandomSample {
 public:
  struct Args {
    Args(int col, int maxSamples) : col(col), maxSamples(maxSamples) {}
    const int col;
    const int maxSamples;
  };

  struct BlockResult {
    BlockResult(std::shared_ptr<std::vector<double>> sample) : sample(sample) {}
    const std::shared_ptr<std::vector<double>> sample;
  };

  struct Result {
    Result(std::shared_ptr<std::vector<double>> samples) : samples(samples) {}
    const std::shared_ptr<std::vector<double>> samples;
  };

  BlockResult apply(const MemoryBlock& block, const Args& args) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1);

    auto const &blockData = block.getBlockData();
    auto const data = blockData.getData();

    int rows = blockData.getRows();
    int cols = blockData.getCols();
    int size = std::min(rows, args.maxSamples);
    auto pSamples = std::make_shared<std::vector<double>>(size);
    std::vector<double> &samples = *pSamples;

    int i = 0;
    for (; i<size; i++) {
      samples[i] = data[i*cols + args.col];
    }

    for (; i<rows; i++) {
      int j = dis(gen) * (i+1);

      if (j < size) {
        samples[j] = data[i*cols + args.col];
      }
    }

    return {pSamples};
  }

  Result combine(std::vector<BlockResult> results) {
    auto samples = std::make_shared<std::vector<double>>();
    for (auto& result : results) {
      auto blockSamples = result.sample;
      samples->insert(samples->end(), blockSamples->begin(), blockSamples->end());
    }
    return {samples};
  }
};


ValueOperation<Count> COUNT;
ValueOperation<SumColumn> SUM;
ValueOperation<MaxColumn> MAX;
ValueOperation<MinColumn> MIN;
ValueOperation<RandomSample> SAMPLE;

}

#endif
