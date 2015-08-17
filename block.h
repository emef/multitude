#ifndef BLOCK_H
#define BLOCK_H

#include <future>
#include <memory>
#include <string>

class DataLocation {
 public:
  DataLocation(std::string path, long offset, long length)
      : path(path), offset(offset), length(length) {}

  std::string getPath() { return path; }
  long getOffset() { return offset; }
  long getLength() { return length; }

 private:
  std::string path;
  long offset;
  long length;
};

class BlockDescriptor {
 public:
  BlockDescriptor(std::shared_ptr<DataLocation> location)
      : location(location) {}

  DataLocation& getLocation() { return *location; }

 private:
  std::shared_ptr<DataLocation> location;
};

class BlockData {
 public:
  BlockData(long rows, long cols, std::unique_ptr<double[]> data)
      : rows(rows), cols(cols), data(std::move(data)) {}

  long getRows() const { return rows; }
  long getCols() const { return cols; }
  const double* getData() const { return data.get(); }

 private:
  long rows;
  long cols;
  std::unique_ptr<double[]> data;
};

class MemoryBlock {
 public:
  MemoryBlock(std::string id, std::unique_ptr<BlockDescriptor> descriptor,
              std::unique_ptr<BlockData> blockData)
      : id(id), descriptor(std::move(descriptor)),
        blockData(std::move(blockData)) {}

  template<typename T>
  std::future<typename T::BlockResult> apply(std::shared_ptr<T> op,
                                             typename T::Args& opArgs);

  template<typename T>
  std::future<MemoryBlock> transform(std::shared_ptr<T> op,
                                     typename T::Args& opArgs);

  std::string getId() { return id; }
  const BlockDescriptor& getDescriptor() const { return *descriptor; }
  const BlockData& getBlockData() const { return *blockData; }

 private:
  std::string id;
  std::unique_ptr<BlockDescriptor> descriptor;
  std::unique_ptr<BlockData> blockData;
};

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

#endif
