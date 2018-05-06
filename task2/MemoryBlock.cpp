//
// Created by kvk1920 on 24.04.18.
//

#include "MemoryBlock.h"

namespace MemoryTools {

Block::Block(size_t n) noexcept :
    used_words_(0),
    size_(n) {
  if (size_) {
    data_ = MemoryHeap::New<Word>(n);
  }
}

Block::~Block() noexcept {
  MemoryHeap::Delete(data_);
}

Block::Block(Block&& block) noexcept :
    used_words_(block.used_words_),
    size_(block.size_) {
  data_ = block.data_;
  block.data_ = nullptr;
  block.used_words_ = 0;
  block.size_ = 0;
}

Block& Block::operator=(Block&& block) noexcept {
  if (data_) {
    free(data_);
  }
  used_words_ = block.used_words_;
  size_ = block.size_;
  block.data_ = nullptr;
  block.used_words_ = 0;
  block.size_ = 0;
  return *this;
}

size_t Block::NumberOfFreeWords() const noexcept {
  return size_ - used_words_;
}

bool Block::CanAllocate(size_t number_of_bytes) const noexcept {
  return NumberOfFreeWords() * sizeof(Word) >= number_of_bytes;
}

void* Block::Allocate(size_t number_of_bytes) noexcept {
  size_t size_in_Words = (number_of_bytes % sizeof(Word) == 0)
      ? number_of_bytes / sizeof(Word)
      : number_of_bytes / sizeof(Word) + 1;
  used_words_ += size_in_Words;
  return reinterpret_cast<void*>(data_ + used_words_ - size_in_Words);
}

}