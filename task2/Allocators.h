//
// Created by kvk1920 on 06.05.18.
//

#ifndef TASK2_ALLOCATORS_H
#define TASK2_ALLOCATORS_H

#include "StackAllocator.h"
#include <cstdlib>

namespace AllocatorTools {

class IMemoryManager;

struct DataInfo {
  IMemoryManager* allocator;
  size_t size;

  DataInfo(IMemoryManager* allocator, size_t size) :
      allocator(allocator), size(size)
  {}
};

const size_t kSizeOfDataInfo = sizeof(DataInfo);

typedef unsigned char Byte;

class IMemoryManager {
 private:
  size_t link_counter;
  virtual Byte* getMemory(size_t size) = 0;
  virtual void freeMemory(Byte* p) = 0;
 public:
  bool isUsing() const { return link_counter > 0; }
  virtual ~IMemoryManager() = default;
  virtual void* Alloc(size_t size) {
    DataInfo* p = reinterpret_cast<DataInfo*>(getMemory(size + kSizeOfDataInfo));
    new(p) DataInfo(this, size);
    return ++p;
  }
  virtual void Free(void* ptr) {
    DataInfo* p = reinterpret_cast<DataInfo*>(ptr);

  }
};

}

#endif //TASK2_ALLOCATORS_H
