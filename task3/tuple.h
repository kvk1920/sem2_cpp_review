//
// Created by kvk1920 on 13.05.18.
//

#ifndef TASK3_TUPLE_H
#define TASK3_TUPLE_H

#include <type_traits>
#include <memory>

template<typename T, typename ...Args>
class Tuple {
 private:
  T object_;
  Tuple<Args...> tail_;
 public:

  template<typename U>
  decltype()

  const auto& operator[](size_t i) const& {
    if (i == 0) {
      return object_;
    } else {
      return tail_[i - 1];
    }
  }

  auto& operator[](size_t i) & {
    if (i == 0) {
      return object_;
    } else {
      return tail_[i - 1];
    }
  }

  auto&& operator[](size_t i) && {
    if (i == 0) {
      return object_;
    } else {
      return tail_[i - 1];
    }
  }

  Tuple() : object_(), tail_() {}

  explicit Tuple(const T& first, const Args& ...args) :
      object_(first),
      tail_(args...) {}

  template <typename U, typename ...UArgs>
  explicit Tuple(U&& first, UArgs&& ...args) :
      object_(std::forward<U>(first)),
      tail_(std::forward<UArgs>(args)...) {}

  Tuple(const Tuple& other) :
      object_(other.object_),
      tail_(other.tail_) {}

  Tuple(Tuple&& other) :
      object_(std::move(other.object_)),
      tail_(std::move(other.tail_)) {}

  ~Tuple() = default;

  Tuple& operator=(const Tuple& other) {
    object_ = other.object_;
    tail_ = other.tail_;
    return *this;
  }

  Tuple& operator=(Tuple&& other) {
    object_ = std::move(other.object_);
    tail_ = std::move(other.tail_);
    return *this;
  }

  void swap(Tuple& other) {
    std::swap(other.object_, object_);
    tail_.swap(other.tail_);
  }

  void swap(Tuple&& other) {
    std::swap(object_, other.object_);
    tail_.swap(other.tail_);
  }

  bool operator<(const Tuple& other) {
    if (object_ == other.object_) {
      return tail_ < other.tail_;
    }
    return object_ < other.object_;
  }

  bool operator>(const Tuple& other) {
    return !(*this < other);
  }

  bool operator<=(const Tuple& other) {
    return !(*this > other);
  }

  bool operator>=(const Tuple& other) {
    return !(*this < other);
  }

  bool operator==(const Tuple& other) {
    return !((*this < other) || (*this > other));
  }

  bool operator!=(const Tuple& other) {
    return !(*this == other);
  }
};

template<typename T>
class Tuple {
 private:
  T object_;
 public:
  Tuple() : object_() {}

  Tuple(const T& first) : object_(first) {}

  template <typename U>
  explicit Tuple(U&& first) :
      object_(std::forward<U>(first))
  {}
};

template<typename ...Args>
Tuple<Args...> makeTuple(Args&& ...args) {
  return Tuple<Args...>(std::forward<Args>(args)...);
}

template <size_t i, typename tupleType>
decltype(auto) get(tupleType&& tuple) -> decltype(tuple[i]) {
  return tuple[i];
};



#endif //TASK3_TUPLE_H
