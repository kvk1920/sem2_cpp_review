//
// Created by kvk1920 on 24.04.18.
//

#ifndef TASK1_STACKALLOCATOR_H
#define TASK1_STACKALLOCATOR_H

#include "SmartPointer.h"
#include "MemoryStack.h"

namespace StackAllocatorClass {

template<typename T>
class StackAllocator {
 public:
  MemoryTools::SmartPointer<StackAllocatorUtility::Stack> pool;

  typedef T value_type;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef T& reference;
  typedef const T& const_reference;

  template<typename U>
  struct rebind {
    typedef StackAllocator<U> other;
  };

  ~StackAllocator() = default;

  explicit StackAllocator(StackAllocatorUtility::Stack* pool = nullptr) noexcept :
      pool(pool) {
    if (!pool) {
      this->pool = MemoryTools::CreateObject<StackAllocatorUtility::Stack>();
    }
  }

  T* allocate(size_t n) noexcept {
    return reinterpret_cast<T*>(pool->Allocate(n * sizeof(T)));
  }

  void deallocate(T*, size_t) noexcept {}

  template<typename U>
  StackAllocator(const StackAllocator<U>& other) : pool(other.pool) {}

  template<typename U, typename ...Args>
  void construct(U* p, Args&& ...args) {
    new(p) U(std::forward<Args>(args)...);
  };

  template<typename U>
  void destroy(U* p) {
    p->~U();
  }
};

}

#endif //TASK1_STACKALLOCATOR_H
