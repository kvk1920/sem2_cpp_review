//
// Created by kvk1920 on 24.04.18.
//

#ifndef TASK1_MEMORYBLOCK_H
#define TASK1_MEMORYBLOCK_H

#include "BasicMemoryTools.h"

namespace MemoryTools {

class Block {
  /**
   * This class may be used only like a low-level storage.
   * So it can't be copied.
   */
  Block(const Block&) = delete;
  Block& operator=(const Block&) = delete;
 private:
  size_t used_words_;
  size_t size_;
  Word* data_;
  size_t NumberOfFreeWords() const noexcept;
 public:
  explicit Block(size_t n = 1) noexcept;
  ~Block();
  Block(Block&& block) noexcept;
  Block& operator=(Block&& block) noexcept;
  ///Checks if this number of bytes can be allocated.
  bool CanAllocate(size_t number_of_bytes) const noexcept;
  ///Returns pointer to an allocated bytes.
  void* Allocate(size_t number_of_bytes) noexcept;
};

}

#endif //TASK1_MEMORYBLOCK_H
