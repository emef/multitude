#ifndef BLOCK_H
#define BLOCK_H

#include <future>
#include <memory>
#include <string>

namespace Multitude {

/**
 * Represents a location of a chunk of data in a file.
 */
class DataLocation {
 public:
  DataLocation(std::string path, long offset, long length)
      : path(path), offset(offset), length(length) {}

  /// Path to file containing block's data.
  std::string getPath() { return path; }

  /// Byte offset in file.
  long getOffset() { return offset; }

  /// Length of block data in bytes.
  long getLength() { return length; }

 private:
  std::string path;
  long offset;
  long length;
};

/**
 * Describes how to fully compute a block of data: it's location
 * and transformations.
 */
class BlockDescriptor {
 public:
  BlockDescriptor(std::shared_ptr<DataLocation> location)
      : location(location) {}

  /// Location of block's matrix data.
  DataLocation& getLocation() { return *location; }

 private:
  std::shared_ptr<DataLocation> location;
};

/**
 * Matrix data contained in a block.
 */
class BlockData {
 public:
  BlockData(long rows, long cols, std::unique_ptr<double[]> data)
      : rows(rows), cols(cols), data(std::move(data)) {}

  /// Number of rows in this block.
  long getRows() const { return rows; }

  /// Number of columns in this block.
  long getCols() const { return cols; }

  /// Immutable view of block's matrix data.
  const double* getData() const { return data.get(); }

 private:
  long rows;
  long cols;
  std::unique_ptr<double[]> data;
};

/**
 * Block of matrix data stored in memory.
 */
class MemoryBlock {
 public:
  MemoryBlock(std::string id, std::unique_ptr<BlockDescriptor> descriptor,
              std::unique_ptr<BlockData> blockData)
      : id(id), descriptor(std::move(descriptor)),
        blockData(std::move(blockData)) {}

  /// Unique block identifier.
  std::string getId() { return id; }

  /// Descriptor of block's data.
  const BlockDescriptor& getDescriptor() const { return *descriptor; }

  /// Immutable view of block's matrix data.
  const BlockData& getBlockData() const { return *blockData; }

 private:
  std::string id;
  std::unique_ptr<BlockDescriptor> descriptor;
  std::unique_ptr<BlockData> blockData;
};

/**
 * Block of matrix data stored on a remote worker. [NOT IMPLEMENTED]
 */
class RemoteBlock {
 public:
  RemoteBlock(std::string id, std::unique_ptr<BlockDescriptor> descriptor)
      : id(id), descriptor(std::move(descriptor)) {}

  template<typename T>
  std::future<typename T::BlockResult> apply(std::shared_ptr<T> op,
                                             typename T::Args& opArgs);

  template<typename T>
  std::future<RemoteBlock> transform(std::shared_ptr<T> op,
                                     typename T::Args& opArgs);

  std::string getId() { return id; }
  const BlockDescriptor& getDescriptor() const { return *descriptor; }

 private:
  std::string id;
  std::unique_ptr<BlockDescriptor> descriptor;
};

}

#endif
