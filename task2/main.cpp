#include <iostream>
#include "GlobalAllocatorSwitcher.h"
#include "Allocators.h"
#include "CAllocatedOn.h"
#include <string>

StackAllocator stack_allocator;

struct X : CAllocatedOn<RuntimeHeap> {
  int x;
  X() {
    std::cout << "Создался\n";
  }
  ~X() {
    std::cout << "Удалился\n";
  }
};

X* f() {
  MemoryManagerSwitcher::Switch(dynamic_cast<IMemoryManager*>(&stack_allocator));
  X* t = new X;
  return t;
}

X* g(int k)
{
  MemoryManagerSwitcher::Switch(dynamic_cast<IMemoryManager*>(&stack_allocator));
  return new X[10];
}

struct alignas(16) S {

};

int main()
{
  delete f();
  X*t =  g(10);
  delete []t;
}