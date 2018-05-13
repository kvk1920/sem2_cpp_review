//
// Created by kvk1920 on 06.05.18.
//

#ifndef TASK2_ALLOCATORS_H
#define TASK2_ALLOCATORS_H

#include "StackAllocator.h"
#include <cstdlib>

class IMemoryManager;

struct __DataInfo {
  IMemoryManager* allocator;
  size_t size;

  __DataInfo(IMemoryManager* allocator, size_t size) :
      allocator(allocator),
      size(size)
  {}
};

const size_t __kSizeOfData = sizeof(__DataInfo);

typedef unsigned char __Byte;

class IMemoryManager {
 private:
  virtual __Byte* getMemory(size_t size) = 0;
  virtual void freeMemory(__Byte* p, size_t size) = 0;
 public:
  virtual ~IMemoryManager() = default;
  void *Alloc(size_t size) {
    auto* p(reinterpret_cast<__DataInfo*>(getMemory(size + __kSizeOfData)));
    new(p) __DataInfo(this, size + __kSizeOfData);
    return ++p;
  }
  void Free(void* ptr) {
    auto* p(reinterpret_cast<__Byte*>(ptr) - __kSizeOfData);
    freeMemory(p, reinterpret_cast<__DataInfo*>(p)->size);
  }
};

class HeapAllocator : public IMemoryManager {
 private:
  __Byte* getMemory(size_t size) final {
    return reinterpret_cast<__Byte*>(malloc(size));
  }
  void freeMemory(__Byte* p, size_t) final {
    free(p);
  }
};

extern HeapAllocator heap_allocator;

template<template <typename> typename StdAllocator>
class AllocatorLikeStd : public IMemoryManager {
 private:
  StdAllocator<__Byte> allocator_;
  __Byte* getMemory(size_t size) final {
    return allocator_.allocate(size);
  }
  void freeMemory(__Byte* p, size_t size) final{
    allocator_.deallocate(p, size);
  }
 public:
  template<typename ...Args>
  explicit AllocatorLikeStd(Args&& ...args) : allocator_(std::forward<Args>(args)...) {}
};

typedef AllocatorLikeStd <StackAllocatorClass::StackAllocator> StackAllocator;

#endif //TASK2_ALLOCATORS_H
