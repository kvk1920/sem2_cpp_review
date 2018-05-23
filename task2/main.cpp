#include <iostream>
#include "GlobalAllocatorSwitcher.h"
#include "Allocators.h"
#include "CAllocatedOn.h"
#include <string>

StackAllocator stack_allocator;

extern int* f_();

struct X{
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

void quick_exit(int __status) {}

X* g(int k)
{
  MemoryManagerSwitcher::Switch switcher(dynamic_cast<IMemoryManager*>(&stack_allocator));
  return new X[10];
}

/*struct alignas(16) S {

};*/

int main()
{
  MemoryManagerSwitcher::Switch Switch(/*<IMemoryManager*>(*/&stack_allocator/*)*/);
  //int*x = f_();
  //printf("\n%p", x);
  delete f();
  //X*t =  g(10);
  //delete []t;
}