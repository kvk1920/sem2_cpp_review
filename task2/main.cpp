#include <iostream>
#include "GlobalAllocatorSwitcher.h"
#include "Allocators.h"
//#include "AllocationStrategy.h"
#include <string>

struct A {
  ~A() {
    std::cout << "hmm..\n";
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
}