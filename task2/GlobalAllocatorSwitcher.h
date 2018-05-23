//
// Created by kvk1920 on 07.05.18.
//

#ifndef TASK2_GLOBALALLOCATORSWITCHER_H
#define TASK2_GLOBALALLOCATORSWITCHER_H

#include "Allocators.h"
/*
void* operator new(size_t size);
void* operator new[](size_t size) ;
void* operator new(size_t size, const std::nothrow_t&) noexcept ;
void* operator new[](size_t size, const std::nothrow_t&) noexcept ;

void operator delete(void* p);
void operator delete[](void* p);
void operator delete(void* p, const std::nothrow_t&) noexcept;
void operator delete[](void* p, const std::nothrow_t&) noexcept;*/



namespace MemoryManagerSwitcher {

struct node {
  node* previous;
  IMemoryManager* current_allocator;
};

IMemoryManager* getCurrentAllocator();

class Switch {
 public:
  explicit Switch(IMemoryManager* allocator);
  ~Switch();
};

}



inline void* operator new(size_t size) {
  return MemoryManagerSwitcher::getCurrentAllocator()->Alloc(size);
}

inline void* operator new[](size_t size) {
  return MemoryManagerSwitcher::getCurrentAllocator()->Alloc(size);
}

inline void* operator new(size_t size, const std::nothrow_t&)noexcept {
  try {
    return MemoryManagerSwitcher::getCurrentAllocator()->Alloc(size);
  } catch(...) { return nullptr; }
}

inline void* operator new[](size_t size, const std::nothrow_t&) noexcept {
  try {
    return MemoryManagerSwitcher::getCurrentAllocator()->Alloc(size);
  } catch(...) { return nullptr; }
}

inline void operator delete(void* p) noexcept {
  (reinterpret_cast<__DataInfo*>(p) - 1)->allocator->Free(p);
}

inline void operator delete[](void* p) noexcept {
  (reinterpret_cast<__DataInfo*>(p) - 1)->allocator->Free(p);
}

inline void operator delete(void* p, const std::nothrow_t&) noexcept {
  try {
    (reinterpret_cast<__DataInfo*>(p) - 1)->allocator->Free(p);
  } catch(...) {}
}

inline void operator delete[](void* p, const std::nothrow_t&) noexcept {
  try {
    (reinterpret_cast<__DataInfo*>(p) - 1)->allocator->Free(p);
  } catch(...) {}
}

#endif //TASK2_GLOBALALLOCATORSWITCHER_H
