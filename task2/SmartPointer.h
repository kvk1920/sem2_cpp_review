//
// Created by kvk1920 on 24.04.18.
//

#ifndef TASK1_SMARTPOINTER_H
#define TASK1_SMARTPOINTER_H

#include "BasicMemoryTools.h"
#include <stdexcept>

namespace MemoryTools {

template <typename T>
class SmartPointer {
  /**
   * shared_ptr doesn't enough fast.
   */
 private:
  size_t* link_counter_;
  T* object_;

  void ClearWithoutDeleting() noexcept {
    link_counter_ = nullptr;
    object_ = nullptr;
  }

  void AddLink() noexcept {
    if (object_) {
      ++*link_counter_;
    }
  }

  void RemoveLink() noexcept {
    if (object_) {
      if (--(*link_counter_) == 0) {
        object_->~T();
        MemoryTools::MemoryHeap::Delete(object_);
        MemoryTools::MemoryHeap::Delete(link_counter_);
        object_ = nullptr;
        link_counter_ = nullptr;
      }
    }
  }

 public:

  explicit SmartPointer(T* p = nullptr) noexcept :
      object_(p),
      link_counter_(nullptr) {
    if (object_) {
      link_counter_ = MemoryTools::MemoryHeap::New<size_t>();
      *link_counter_ = 0;
    }
    AddLink();
  }

  SmartPointer(const SmartPointer& pointer) noexcept :
      object_(pointer.object_),
      link_counter_(pointer.link_counter_) {
    AddLink();
  }

  SmartPointer& operator=(const SmartPointer& pointer) noexcept {
    RemoveLink();
    object_ = pointer.object_;
    link_counter_ = pointer.link_counter_;
    AddLink();
    return *this;
  }

  SmartPointer(SmartPointer&& pointer) noexcept :
      object_(pointer.object_),
      link_counter_(pointer.link_counter_) {
    pointer.ClearWithoutDeleting();
  }

  SmartPointer& operator=(SmartPointer&& pointer) {
    RemoveLink();
    object_ = pointer.object_;
    link_counter_ = pointer.link_counter_;
    pointer.ClearWithoutDeleting();
    return *this;
  }

  template <typename AnotherT>
  explicit SmartPointer(const SmartPointer<AnotherT>& pointer) noexcept :
      object_((T*) pointer.object_),
      link_counter_(pointer.link_counter_) {
    AddLink();
  }

  ~SmartPointer() noexcept {
    RemoveLink();
  }

  T& operator*() const noexcept {
    return *object_;
  }

  T* operator->() const noexcept {
    return object_;
  }

};

template <typename T, typename ...Args>
SmartPointer<T> CreateObject(Args&& ...args) {
  T* object = MemoryTools::MemoryHeap::New<T>();
  new(object) T(std::forward<Args>(args)...);
  return SmartPointer<T>(object);
};

}

#endif //TASK1_SMARTPOINTER_H
