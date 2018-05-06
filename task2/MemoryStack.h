//
// Created by kvk1920 on 24.04.18.
//

#ifndef TASK1_MEMORYSTACK_H
#define TASK1_MEMORYSTACK_H

#include "MemoryBlock.h"
#include "BasicMemoryTools.h"

namespace StackAllocatorUtility {

class Stack {
  Stack(const Stack&) = delete;
  Stack& operator=(const Stack&) = delete;
 private:
  /**
   * Stack realised as one-directed list.
   */
  struct Node {
    MemoryTools::Block data;
    Node* next;
    Node(size_t n) noexcept;
  } * head_;
  size_t size_;
  void AddMemory() noexcept;
  void Destructor() noexcept;///<We use it in ~Stack() and operator=.
  void ClearWithoutDeleting() noexcept;
 public:
  Stack() noexcept;
  ~Stack() noexcept;
  Stack(Stack&& stack) noexcept;
  Stack& operator=(Stack&& stack) noexcept;
  void* Allocate(size_t number_of_bytes) noexcept;
};

}

#endif //TASK1_MEMORYSTACK_H
