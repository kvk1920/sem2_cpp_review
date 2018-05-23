//
// Created by kvk1920 on 14.05.18.
//

#include "GlobalAllocatorSwitcher.h"

namespace MemoryManagerSwitcher {


node* top_of_stack = nullptr;

IMemoryManager* getCurrentAllocator() {
  if (!top_of_stack) {
    return &heap_allocator;
  }
  return top_of_stack->current_allocator;
}

Switch::Switch(IMemoryManager* allocator) {
  node* stack_node = reinterpret_cast<node*>(malloc(sizeof(node)));
  stack_node->current_allocator = allocator;
  stack_node->previous = top_of_stack;
  top_of_stack = stack_node;
}

Switch::~Switch() {
  node* old_node = top_of_stack;
  top_of_stack = top_of_stack->previous;
  free(old_node);
}

}
