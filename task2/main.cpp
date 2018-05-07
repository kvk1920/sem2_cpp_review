#include <iostream>
#include "GlobalAllocatorSwitcher.h"
#include "Allocators.h"
#include "CAllocatedOn.h"
#include <string>

struct A {
  ~A() {
    std::cout << "hmm..\n";
  }
};

struct B : CAllocatedOn<RuntimeHeap> {
  ~B() {
    std::cout << "Я удалился!\n";
  }
};

int main()
{
  int* a = new int[10];
  for (int i(0); i < 10; ++i)
    a[i] = i;
  CMemoryManagerSwitcher.Switch<stack_allocator>();
  int* b = new int[10];
  for (int i(0); i < 10; ++i)
    b[i] = 10 - i;
  for (int i(0); i < 10; ++i)
    std::cout << b[i] << ' ';
  std::cout << std::endl;
  for (int i(0); i < 10; ++i)
    std::cout << a[i] << ' ';
  delete [] a;
  delete [] b;
  A* x = new A;
  CMemoryManagerSwitcher.Switch<heap_allocator>();
  delete x;
  CMemoryManagerSwitcher.Switch<stack_allocator>();
  A* y = new A[10];
  delete [] y;
  y = new A[5];
  CMemoryManagerSwitcher.Switch<heap_allocator>();
  delete [] y;



  B* x_ = new B;
  CMemoryManagerSwitcher.Switch<heap_allocator>();
  delete x_;
  CMemoryManagerSwitcher.Switch<stack_allocator>();
  B* y_ = new B[10];
  delete [] y_;
  y_ = new B[5];
  CMemoryManagerSwitcher.Switch<heap_allocator>();
  delete [] y_;
}