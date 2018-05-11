#include <iostream>
#include "GlobalAllocatorSwitcher.h"
#include "Allocators.h"
#include "CAllocatedOn.h"
#include <string>

struct _node_ : CAllocatedOn<RuntimeHeap> {
  _node_* next;
  IMemoryManager* allocator;
};


int* [[deprecated]] get_array(size_t size) {
  MemoryManagerSwitcher::Switch(stac)
  int* a = new int[size];
  return a;
}