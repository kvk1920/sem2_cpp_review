//
// Created by kvk1920 on 24.04.18.
//

#include "MemoryStack.h"
#include <new>

namespace StackAllocatorUtility {

Stack::Node::Node(size_t n) noexcept : data(n), next(nullptr) {}

void Stack::AddMemory() noexcept {
  if (size_ == 0)
  {
    head_ = MemoryTools::MemoryHeap::New<Node>();
    size_ = 1;
    new(head_) Node(1);
  } else {
    Node *p = MemoryTools::MemoryHeap::New<Node>();
    new(p) Node(size_);
    p->next = head_;
    head_ = p;
    size_ += size_;
  }
}

Stack::Stack() noexcept : head_(nullptr), size_(0) {}

void Stack::Destructor() noexcept {
  for (Node* tmp; head_; ) {
    tmp = head_;
    head_ = head_->next;
    tmp->~Node();
    MemoryTools::MemoryHeap::Delete(tmp);
  }
}

Stack::~Stack() noexcept {
  Destructor();
}

void Stack::ClearWithoutDeleting() noexcept {
  size_ = 0;
  head_ = nullptr;
}

Stack::Stack(Stack&& stack) noexcept :
    head_(stack.head_),
    size_(stack.size_) {
  stack.ClearWithoutDeleting();
}

Stack& Stack::operator=(Stack &&stack) noexcept {
  Destructor();
  head_ = stack.head_;
  size_ = stack.size_;
  stack.ClearWithoutDeleting();
}

void* Stack::Allocate(size_t number_of_bytes) noexcept {
  if (!head_) {
    AddMemory();
  }
  while (!head_->data.CanAllocate(number_of_bytes)) {
    AddMemory();
  }
  return head_->data.Allocate(number_of_bytes);
}

}