//
// Created by kvk1920 on 07.05.18.
//

#ifndef TASK2_GLOBALALLOCATORSWITCHER_H
#define TASK2_GLOBALALLOCATORSWITCHER_H

#include "Allocators.h"

namespace __MemoryManagerSwitcherTools {

class __AllocatorStorage {
  friend class __CMemoryManagerSwitcher;

 private:

  template<typename ManagerType, typename ...Args>
  static Manager makeManager(Args&& ...args) {
    auto manager = reinterpret_cast<ManagerType*>(malloc(sizeof(ManagerType)));
    new(manager) ManagerType(std::forward<Args>(args)...);
    return manager;
  }

  static void destroyManager(Manager manager) {
    manager->AllocatorTools::IMemoryManager::~IMemoryManager();
    free(manager);
  }

  __AllocatorStorage() = delete;
};

class __CMemoryManagerSwitcher {
  /**
   * Don't create another CMemoryManagerSwitcher!!!
   */
 public:
  Manager current_manager;
  __CMemoryManagerSwitcher() {
    current_manager = __AllocatorStorage::makeManager<heap_allocator>();
  }
  template <typename NextAllocator, typename ...Args>
  void Switch(Args&& ...args) {
    if (current_manager->isNoUsing()) {
      __AllocatorStorage::destroyManager(current_manager);
      current_manager = __AllocatorStorage
      ::makeManager<NextAllocator>(std::forward<Args>(args)...);
    }
  }
  void Check(Manager allocator) {
    if (allocator->isNoUsing() && current_manager != allocator) {
      __AllocatorStorage::destroyManager(allocator);
    }
  }
} CMemoryManagerSwitcher;

}

using __MemoryManagerSwitcherTools::CMemoryManagerSwitcher;


inline void* operator new(size_t size) {
  return CMemoryManagerSwitcher.current_manager->Alloc(size);
}

inline void* operator new[](size_t size) {
  return operator new(size);
}

inline void* operator new(size_t size, const std::nothrow_t&) noexcept {
  void* p;
  try {
    p = CMemoryManagerSwitcher.current_manager->Alloc(size);
  } catch (...) { p = nullptr; }
  return p;
}

inline void* operator new[](size_t size, const std::nothrow_t&) noexcept {
  return operator new(size, std::nothrow);
}

inline void operator delete(void* p) {
  static Manager allocator;
  allocator = (reinterpret_cast<AllocatorTools::DataInfo*>(p) - 1)->allocator;
  allocator->Free(p);
  CMemoryManagerSwitcher.Check(allocator);
}

inline void operator delete[](void* p) {
  ::operator delete(p);
}

inline void operator delete(void* p, const std::nothrow_t&) noexcept {
  static Manager allocator;
  try {
    allocator = (reinterpret_cast<AllocatorTools::DataInfo*>(p) - 1)->allocator;
    allocator->Free(p);
    CMemoryManagerSwitcher.Check(allocator);
  } catch(...) {}
}

inline void operator delete[](void* p, const std::nothrow_t&) noexcept {
  ::operator delete(p, std::nothrow);
}

#endif //TASK2_GLOBALALLOCATORSWITCHER_H
