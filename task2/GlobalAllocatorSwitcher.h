//
// Created by kvk1920 on 07.05.18.
//

#ifndef TASK2_GLOBALALLOCATORSWITCHER_H
#define TASK2_GLOBALALLOCATORSWITCHER_H

#include "Allocators.h"

namespace MemoryManagerSwitcher {

struct node {
  node* previous;
  IMemoryManager* current_allocator;
};

extern node* top_of_stack;

IMemoryManager* getCurrentAllocator() {
  if (!top_of_stack)
    return &heap_allocator;
  return top_of_stack->current_allocator;
}

class Switch {
 public:
  explicit Switch(IMemoryManager* allocator) {
    node* stack_node = reinterpret_cast<node*>(malloc(sizeof(node)));
    stack_node->current_allocator = allocator;
    stack_node->previous = top_of_stack;
    top_of_stack = stack_node;
  }
  ~Switch() {
    node* old_node = top_of_stack;
    top_of_stack = top_of_stack->previous;
    free(old_node);
  }
};

}

inline void* operator new(size_t size) {
  return MemoryManagerSwitcher::getCurrentAllocator()->Alloc(size);
}

inline void* operator new[](size_t size) {
  return ::operator new(size);
}

inline void* operator new(size_t size, const std::nothrow_t&) noexcept {
  try {
    return ::operator new(size);
  } catch(...) {}
}

inline void* operator new[](size_t size, const std::nothrow_t&) noexcept {
  return ::operator new(size, std::nothrow);
}

inline void operator delete(void* p) {
  (reinterpret_cast<__DataInfo*>(p) - 1)->allocator->Free(p);
}

inline void operator delete[](void* p) {
  ::operator delete(p);
}

inline void operator delete(void* p, const std::nothrow_t&) noexcept {
  try {
    ::operator delete(p);
  } catch(...) {}
}

inline void operator delete[](void* p, const std::nothrow_t&) noexcept {
  return ::operator delete(p, std::nothrow);
}

#endif //TASK2_GLOBALALLOCATORSWITCHER_H
