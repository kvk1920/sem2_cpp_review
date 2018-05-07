//
// Created by kvk1920 on 06.05.18.
//

#ifndef TASK2_ALLOCATORS_H
#define TASK2_ALLOCATORS_H

#include "StackAllocator.h"
//#include "GlobalAllocatorSwitcher.h"
#include <cstdlib>


/*template <typename ManagerType>
friend class CAllocatedOn;
*/
namespace __MemoryManagerSwitcherTools {

class __AllocatorStorage;

}

namespace AllocatorTools {

class IMemoryManager;

struct DataInfo {
  IMemoryManager* allocator;
  size_t size;

  DataInfo(IMemoryManager* allocator, size_t size) :
      allocator(allocator),
      size(size)
  {}
};

typedef unsigned char Byte;
const size_t kSizeOfInfo = sizeof(DataInfo);
typedef IMemoryManager* Manager;

class IMemoryManager {
  friend class __MemoryManagerSwitcherTools::__AllocatorStorage;
  template <typename ManagerType>
  friend class CAllocatedOn;
 protected:
  virtual Byte* getMemory(size_t size) = 0;
  virtual void freeMemory(Byte* p, size_t size) = 0;
  virtual ~IMemoryManager() = default;
  size_t allocations_counter_;
 public:
  bool isNoUsing() { return allocations_counter_ == 0; }
  void* Alloc(size_t size) {
    Byte* ptr(getMemory(size + kSizeOfInfo));
    new(ptr) DataInfo(this, size);
    ++allocations_counter_;
    return ptr + kSizeOfInfo;
  }
  void Free(void* ptr) {
    Byte* p(reinterpret_cast<Byte*>(ptr) - kSizeOfInfo);
    freeMemory(p, reinterpret_cast<DataInfo*>(p)->size + kSizeOfInfo);
    --allocations_counter_;
  }
};

class HeapAllocator : public IMemoryManager {
  friend class __MemoryManagerSwitcherTools::__AllocatorStorage;
  template <typename ManagerType>
  friend class CAllocatedOn;
 protected:
  Byte* getMemory(size_t size) final { return reinterpret_cast<Byte*>(malloc(size)); }
  void freeMemory(Byte* p, size_t size) final { free(p); }
  HeapAllocator() { allocations_counter_ = 0; }
 public:
  ~HeapAllocator() = default;
};

template <template <typename> typename Allocator>
class AllocatorLikeStd : public IMemoryManager {
  friend class __MemoryManagerSwitcherTools::__AllocatorStorage;
  template <typename ManagerType>
  friend class CAllocatedOn;
 protected:
  Allocator<Byte> allocator_;
  Byte* getMemory(size_t size) final { return allocator_.allocate(size); }
  void freeMemory(Byte* p, size_t size) final { allocator_.deallocate(p, size); }
  template <typename ...Args>
  AllocatorLikeStd(Args&& ...args) :
      allocator_(std::forward<Args>(args)...)
  {
    allocations_counter_ = 0;
  }
};

}

using AllocatorTools::Manager;
using stack_allocator = AllocatorTools::AllocatorLikeStd<StackAllocatorClass::StackAllocator>;
using heap_allocator = AllocatorTools::HeapAllocator;

#endif //TASK2_ALLOCATORS_H
