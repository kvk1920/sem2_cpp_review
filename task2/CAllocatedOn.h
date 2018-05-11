//
// Created by kvk1920 on 07.05.18.
//

#ifndef TASK2_CALLOCATEDON_H
#define TASK2_CALLOCATEDON_H

#include "Allocators.h"
#include "GlobalAllocatorSwitcher.h"

class RuntimeHeap {
 public:
  static void* Alloc(size_t size) { return malloc(size); }
  static void Free(void* ptr) { return free(ptr); }
};

class CurrentMemoryManager {
 public:
  static void* Alloc(size_t size) { ::operator new(size); }
  static void Free(void* ptr) { ::operator delete(ptr); }
};

template <typename allocator_>
class CAllocatedOn {
 public:
  inline void* operator new(size_t size) {
    return allocator_::Alloc(size);
  }

  inline void* operator new[](size_t size) {
    return operator new(size);
  }

  inline void* operator new(size_t size, const std::nothrow_t&) noexcept {
    void* p;
    try {
      p = allocator_::Alloc(size);
    } catch (...) { p = nullptr; }
    return p;
  }

  inline void* operator new[](size_t size, const std::nothrow_t&) noexcept {
    return operator new(size, std::nothrow);
  }

  inline void operator delete(void* p) {
    allocator_::Free(p);
  }

  inline void operator delete[](void* p) {
    ::operator delete(p);
  }

  inline void operator delete(void* p, const std::nothrow_t&) noexcept {
    try {
      allocator_::Free(p);
    } catch(...) {}
  }

  inline void operator delete[](void* p, const std::nothrow_t&) noexcept {
    ::operator delete(p, std::nothrow);
  }
};

#endif //TASK2_CALLOCATEDON_H
