//
// Created by kvk1920 on 07.05.18.
//

#ifndef TASK2_CALLOCATEDON_H
#define TASK2_CALLOCATEDON_H

#include "Allocators.h"

template <typename ManagerType>
class CAllocatedOn {
 private:
  ManagerType allocator;
 public:
  inline void* operator new(size_t size) {
    return allocator->Alloc(size);
  }

  inline void* operator new[](size_t size) {
    return operator new(size);
  }

  inline void* operator new(size_t size, const std::nothrow_t&) noexcept {
    void* p;
    try {
      p = allocator->Alloc(size);
    } catch (...) { p = nullptr; }
    return p;
  }

  inline void* operator new[](size_t size, const std::nothrow_t&) noexcept {
    return operator new(size, std::nothrow);
  }

  inline void operator delete(void* p) {
    static Manager allocator;
    allocator = (reinterpret_cast<AllocatorTools::DataInfo*>(p) - 1)->allocator;
    allocator->Free(p);
  }

  inline void operator delete[](void* p) {
    ::operator delete(p);
  }

  inline void operator delete(void* p, const std::nothrow_t&) noexcept {
    static Manager allocator;
    try {
      allocator = (reinterpret_cast<AllocatorTools::DataInfo*>(p) - 1)->allocator;
      allocator->Free(p);
    } catch(...) {}
  }

  inline void operator delete[](void* p, const std::nothrow_t&) noexcept {
    ::operator delete(p, std::nothrow);
  }
};

#endif //TASK2_CALLOCATEDON_H
