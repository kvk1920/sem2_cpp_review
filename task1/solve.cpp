#include <cstdlib>

namespace MemoryTools {

typedef long Word;

namespace MemoryHeap {

template <typename T>
inline T* New(size_t N = 1) noexcept {
    /**
     * This function doesn't construct objects.
     * It only gets memory for N objects from system.
     */
    return reinterpret_cast<T*>(malloc(N * sizeof(T)));
}

template <typename T>
inline void Delete(T* p) noexcept {
    ///It frees memory, that got by New().
    free(p);
}

}

}

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


template<typename T>
class StackAllocator {
 public:
  MemoryTools::SmartPointer<StackAllocatorUtility::Stack> pool;

  typedef T value_type;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef T& reference;
  typedef const T& const_reference;

  template<typename U>
  struct rebind { typedef StackAllocator<U> other; };

  ~StackAllocator() = default;

  explicit StackAllocator(StackAllocatorUtility::Stack* pool = nullptr) noexcept :
      pool(pool) {
    if (!pool) {
      this->pool = MemoryTools::CreateObject<StackAllocatorUtility::Stack>();
    }
  }

  T* allocate(size_t n) noexcept {
    return reinterpret_cast<T*>(pool->Allocate(n * sizeof(T)));
  }

  void deallocate(T*, size_t) noexcept {}

  template <typename U>
  StackAllocator(const StackAllocator<U>& other) : pool(other.pool) {}

  template <typename U, typename ...Args>
  void construct(U* p, Args&& ...args) {
    new(p) U(std::forward<Args>(args)...);
  };

  template <typename U>
  void destroy(U* p) {
    p->~U();
  }
};


#include <memory>
#include <iterator>

namespace XorListUtility {

template <typename T>
struct Node {
  T value;
  Node* ptr;
  template <typename ...Args>
  explicit Node(Args&& ...args);
};

template <typename T>
Node<T>* Xor(const Node<T>* p1, const Node<T>* p2);

template <typename T, typename Allocator>
class XorList;

template <typename mainT, typename T>
class Iterator : public std::iterator<std::bidirectional_iterator_tag, T, ptrdiff_t, T*, T&> {
 public:
  template <typename U, typename Allocator>
  friend class XorList;
 private:
  typedef Node<mainT> node_t;
  node_t* previous_node_;
  node_t* current_node_;
  node_t* next_node_;
  Iterator(node_t* previous_node, node_t* node, node_t* next_node) :
      previous_node_(previous_node),
      current_node_(node),
      next_node_(next_node)
  {}
 public:
  Iterator() : previous_node_(nullptr), current_node_(nullptr), next_node_(nullptr) {}
  Iterator(const Iterator& iterator) :
      previous_node_(iterator.previous_node_),
      current_node_(iterator.current_node_),
      next_node_(iterator.next_node_)
  {}
  Iterator operator++(int) {
    Iterator it(*this);
    ++*this;
    return it;
  }
  Iterator& operator++(){
    previous_node_ = current_node_;
    current_node_ = next_node_;
    if (current_node_) {
      next_node_ = Xor(current_node_->ptr, previous_node_);
    } else {
      next_node_ = nullptr;
    }
    return *this;
  }
  Iterator operator--(int) {
    Iterator it(*this);
    ++*this;
    return it;
  }
  Iterator& operator--() {
    next_node_ = current_node_;
    current_node_ = previous_node_;
    if (current_node_) {
      previous_node_ = Xor(current_node_->ptr, next_node_);
    } else {
      previous_node_ = nullptr;
    }
    return *this;
  }
  bool operator==(const Iterator& iterator) {
    return current_node_ == iterator.current_node_;
  }
  bool operator!=(const Iterator& iterator) {
    return !(*this == iterator);
  }
  T& operator*() const {
    return current_node_->value;
  }
  T* operator->() const {
    return &current_node_->value;
  }
  explicit operator bool() {
    return current_node_ != nullptr;
  }
};

template <typename T, typename Allocator = std::allocator<T>>
class XorList {
 public:
  typedef Iterator<T, T> iterator;
  typedef Iterator<T, const T> const_iterator;
  typedef std::reverse_iterator<iterator> reverse_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

 private:
  typedef Node<T> _Node;

  _Node* begin_;
  _Node* end_;
  size_t size_;
  typename Allocator::template rebind<_Node>::other allocator_;

  template <typename ...Args>
  _Node* makeNode(Args&& ...args);
  void deleteNode(_Node* node);

  template <typename ...Args>
  iterator _insert_after(iterator it, Args&& ...args);

  template <typename ...Args>
  iterator _insert_before(iterator it, Args&& ...args);

  template <typename ...Args>
  _Node* _insert_between(_Node* left_node, _Node* right_node, Args&& ...args);

  template <typename ...Args>
  void _insert_to_empty(Args&& ...args);

  template <typename ...Args>
  void _push_front(Args&& ...args);

  //template <typename T_>
  //void _push_back(T_&& value);

  template <typename ...Args>
  void _push_back(Args&& ...args);
 public:
  explicit XorList(const Allocator& alloc = Allocator());
  explicit XorList(size_t n, const T& value = T());
  XorList(const XorList& list);
  XorList(XorList&& list) noexcept;

  XorList& operator=(const XorList& list);
  XorList& operator=(XorList&& list);

  iterator begin();
  const_iterator cbegin() const;
  const_iterator begin() const;
  reverse_iterator rbegin();
  const_reverse_iterator crbegin() const;
  const_reverse_iterator rbegin() const;

  T& front();
  const T& front() const;

  void push_front(T&& value);
  void push_front(const T& value);

  void pop_front();

  iterator end();
  const_iterator cend() const;
  const_iterator end() const;
  reverse_iterator rend();
  const_reverse_iterator crend() const;
  const_reverse_iterator rend() const;

  T& back();
  const T& back() const;

  void push_back(T&& value);
  void push_back(const T& value);

  void pop_back();

  iterator insert_before(iterator it, const T& value);
  iterator insert_before(iterator it, T&& value);

  iterator insert_after(iterator it, const T& value);
  iterator insert_after(iterator it, T&& value);

  void erase(iterator it);
  void clear();

  bool empty() const;
  size_t size() const;
};

template<typename T, typename Allocator>
XorList<T, Allocator>& XorList<T, Allocator>::operator=(XorList&& list) {
  clear();
  begin_ = list.begin_;
  end_ = list.end_;
  size_ = list.size_;
  list.size_ = 0;
  list.begin_ = list.end_ = nullptr;
  return *this;
};

template<typename T, typename Allocator>
XorList<T, Allocator>& XorList<T, Allocator>::operator=(const XorList& list) {
  clear;
  std::copy(list.begin(), list.end(), std::back_inserter<XorList<T, Allocator>>(*this));
  return *this;
};

template <typename T, typename Allocator>
XorList<T, Allocator>::XorList(const Allocator& alloc) :
    allocator_(alloc),
    size_(0),
    begin_(nullptr),
    end_(nullptr)
{};

template <typename T, typename Allocator>
XorList<T, Allocator>::XorList(XorList&& list) noexcept :
    begin_(list.begin_),
    end_(list.end_),
    size_(list.size_) {
  list.size_ = 0;
  list.end_ = list.begin_ = nullptr;
};

template <typename T, typename Allocator>
XorList<T, Allocator>::XorList(const XorList& list) :
    begin_(nullptr),
    end_(nullptr),
    size_(0) {
  std::copy(list.begin(), list.end(), std::back_inserter<XorList<T, Allocator>>(*this));
};

template <typename T, typename Allocator>
XorList<T, Allocator>::XorList(size_t n, const T& value) {
  end_ = begin_ = nullptr;
  size_ = 0;
  for (size_t i(0); i < n; ++i) {
    push_back(value);
  }
}

template<typename T, typename Allocator>
typename XorList<T, Allocator>::iterator XorList<T, Allocator>::begin() {
  if (empty()) {
    return XorList<T, Allocator>::iterator(nullptr, nullptr, nullptr);
  } else {
    return XorList<T, Allocator>::iterator(nullptr, begin_, begin_->ptr);
  }
};

template<typename T, typename Allocator>
typename XorList<T, Allocator>::const_iterator XorList<T, Allocator>::begin() const {
  if (empty()) {
    return XorList<T, Allocator>::const_iterator(0LL, nullptr, nullptr);
  } else {
    return XorList<T, Allocator>::const_iterator(nullptr, begin_, begin_->ptr);
  }
}

template<typename T, typename Allocator>
typename XorList<T, Allocator>::const_iterator XorList<T, Allocator>::cbegin() const { return begin(); }

template<typename T, typename Allocator>
typename XorList<T, Allocator>::reverse_iterator XorList<T, Allocator>::rbegin() {
  return std::reverse_iterator<iterator>(end());
}

template<typename T, typename Allocator>
typename XorList<T, Allocator>::const_reverse_iterator XorList<T, Allocator>::rbegin() const {
  return std::reverse_iterator<const_iterator>(end());
}

template<typename T, typename Allocator>
typename XorList<T, Allocator>::const_reverse_iterator XorList<T, Allocator>::crbegin() const {
  return rbegin();
}

template<typename T, typename Allocator>
T& XorList<T, Allocator>::front() {
  return begin_->value;
}

template<typename T, typename Allocator>
 const T& XorList<T, Allocator>::front() const {
  return begin_->value;
}

template<typename T, typename Allocator>
 const T& XorList<T, Allocator>::back() const {
  return end_->value;
};

template<typename T, typename Allocator>
 T& XorList<T, Allocator>::back() {
  return end_->value;
};

template<typename T, typename Allocator>
 void XorList<T, Allocator>::push_back(T&& value) {
  _push_back(std::move(value));
}

template<typename T, typename Allocator>
void XorList<T, Allocator>::push_back(const T& value) {
  _push_back(value);
}

template<typename T, typename Allocator>
template<typename...Args>
void XorList<T, Allocator>::_push_back(Args&& ... args) {
  if (empty()) {
    _insert_to_empty(std::forward<Args>(args)...);
  } else {
    Node<T>* new_node(makeNode(std::forward<Args>(args)...));
    new_node->ptr = end_;
    end_->ptr = Xor(end_->ptr, new_node);
    end_ = new_node;
  }
}

template<typename T, typename Allocator>
template<typename...Args>
void XorList<T, Allocator>::_push_front(Args&& ... args) {
  if (empty()) {
    _insert_to_empty(std::forward<Args>(args)...);
  } else {
    Node<T>* new_node(makeNode(std::forward<Args>(args)...));
    new_node->ptr = begin_;
    begin_->ptr = Xor(begin_->ptr, new_node);
    begin_ = new_node;
  }
}

template<typename T, typename Allocator>
void XorList<T, Allocator>::push_front(const T& value) {
  _push_front(value);
};

template<typename T, typename Allocator>
void XorList<T, Allocator>::push_front(T&& value) {
  _push_front(std::move(value));
}

template<typename T, typename Allocator>
void XorList<T, Allocator>::pop_back() {
  if (size() == 1) {
    clear();
  } else {
    Node<T>* deleted = end_;
    end_ = end_->ptr;
    end_->ptr = Xor(end_->ptr, deleted);
    deleteNode(deleted);
  }
};

template<typename T, typename Allocator>
void XorList<T, Allocator>::pop_front() {
  if (size() == 1) {
    clear();
  } else {
    Node<T>* deleted = begin_;
    begin_ = begin_->ptr;
    begin_->ptr = Xor(begin_->ptr, deleted);
    deleteNode(deleted);
  }
}

template<typename T, typename Allocator>
template<typename...Args>
typename XorList<T, Allocator>::iterator XorList<T, Allocator>::_insert_before(typename XorList<T, Allocator>::iterator it, Args&& ... args) {
  if (it == begin()) {
    push_front(std::forward<Args>(args)...);
    return begin();
  } else if (it == end()) {
    push_back(std::forward<Args>(args)...);
    return rbegin();
  } else {
    return iterator(it.previous_node_, _insert_between(it.previous_node_, it.current_node_, std::forward<Args>(args)...), it.current_node_);
  }
}

template<typename T, typename Allocator>
typename XorList<T, Allocator>::iterator XorList<T, Allocator>::insert_before(iterator it, const T& value) {
  _insert_before(it, value);
}

template<typename T, typename Allocator>
typename XorList<T, Allocator>::iterator XorList<T, Allocator>::insert_before(iterator it, T&& value) {
  _insert_before(it, std::move(value));
}

template <typename T, typename Allocator>
template <typename ...Args>
Node<T>* XorList<T, Allocator>::_insert_between(_Node* left_node, _Node* right_node, Args&& ...args) {
  _Node* new_node(makeNode(std::forward<Args>(args)...));
  left_node->ptr = Xor(left_node->ptr, right_node);
  left_node->ptr = Xor(left_node->ptr, new_node);
  right_node->ptr = Xor(right_node->ptr, left_node);
  right_node->ptr = Xor(right_node->ptr, new_node);
  new_node->ptr = Xor(left_node, right_node);
  return new_node;
}

template <typename T, typename Allocator>
template <typename ...Args>
typename XorList<T, Allocator>::iterator XorList<T, Allocator>::_insert_after(iterator it, Args&& ... args) {
  if (it == rend()) {
    push_front(std::forward<Args>(args)...);
    return begin();
  } else if (it == rbegin()) {
    push_back(std::forward<Args>(args)...);
    return rbegin();
  } else {
    return iterator(it.current_node_, _insert_between(it.current_node_, it.next_node_, std::forward<Args>(args)...), it.next_node_);
  }
}

template<typename T, typename Allocator>
typename XorList<T, Allocator>::iterator XorList<T, Allocator>::insert_after(iterator it, T&& value) {
  return _insert_after(it, std::move(value));
}

template<typename T, typename Allocator>
typename XorList<T, Allocator>::iterator XorList<T, Allocator>::insert_after(iterator it, const T& value) {
  return _insert_after(it, value);
}

template<typename T, typename Allocator>
void XorList<T, Allocator>::erase(typename XorList<T, Allocator>::iterator it) {
  if (it == begin()) {
    pop_front();
  } else if (it == --end()) {
    pop_back();
  } else if (!it) {
    return;
  } else {
    _Node* left_node(it.previous_node_),* target_node(it.current_node_),* right_node(it.next_node_);
    left_node->ptr = Xor(left_node->ptr, target_node);
    left_node->ptr = Xor(left_node->ptr, right_node);
    right_node->ptr = Xor(right_node->ptr, target_node);
    right_node->ptr = Xor(right_node->ptr, left_node);
    deleteNode(target_node);
  }
}

template<typename T, typename Allocator>
template<typename ...Args>
Node<T>* XorList<T, Allocator>::makeNode(Args&& ...args) {
  _Node* new_node(allocator_.allocate(1));
  allocator_.construct(new_node, std::forward<Args>(args)...);
  ++size_;
  return new_node;
}

template<typename T, typename Allocator>
void XorList<T, Allocator>::deleteNode(Node<T>* node) {
  allocator_.destroy(node);
  allocator_.deallocate(node, 1);
  --size_;
}

template<typename T>
template<typename ...Args>
Node<T>::Node(Args&& ... args) :
    value(std::forward<Args>(args)...),
    ptr(0LL)
{}

template <typename T>
Node<T>* Xor(const Node<T>* p1, const Node<T>* p2) {
  return reinterpret_cast<Node<T>*>(reinterpret_cast<long long>(p1) ^ reinterpret_cast<long long>(p2));
}

template<typename T, typename Allocator>
bool XorList<T, Allocator>::empty() const {
  return size_ == 0;
}

template<typename T, typename Allocator>
size_t XorList<T, Allocator>::size() const {
  return size_;
}

template<typename T, typename Allocator>
void XorList<T, Allocator>::clear() {
  if (!empty()) {
    Node<T>* current_node(begin_);
    Node<T>* next_node;
    while (current_node) {
      if (next_node) {
        next_node = current_node->ptr;
        next_node->ptr = Xor(next_node->ptr, current_node);
      }
      deleteNode(current_node);
      current_node = next_node;
    }
  }
}

template<typename T, typename Allocator>
typename XorList<T, Allocator>::const_reverse_iterator XorList<T, Allocator>::rend() const {
  return std::reverse_iterator<const_iterator>(begin());
}

template<typename T, typename Allocator>
template<typename... Args>
void XorList<T, Allocator>::_insert_to_empty(Args&& ... args) {
  end_ = begin_ = makeNode(std::forward<Args>(args)...);
}

template<typename T, typename Allocator>
typename XorList<T, Allocator>::const_reverse_iterator XorList<T, Allocator>::crend() const {
  return rend();
}

template<typename T, typename Allocator>
typename XorList<T, Allocator>::iterator XorList<T, Allocator>::end() {
  if (empty()) {
    return XorList<T, Allocator>::iterator(nullptr, nullptr, nullptr);
  } else {
    return XorList<T, Allocator>::iterator(end_, nullptr, nullptr);
  }
}

template<typename T, typename Allocator>
typename XorList<T, Allocator>::const_iterator XorList<T, Allocator>::end() const {
  if (empty()) {
    return XorList<T, Allocator>::const_iterator(nullptr, nullptr, nullptr);
  } else {
    return XorList<T, Allocator>::const_iterator(end_, nullptr, nullptr);
  }
}

template<typename T, typename Allocator>
typename XorList<T, Allocator>::const_iterator XorList<T, Allocator>::cend() const {
  return end();
}

template<typename T, typename Allocator>
typename XorList<T, Allocator>::reverse_iterator XorList<T, Allocator>::rend() {
  return std::reverse_iterator<iterator>(begin());
}

}


#include <list>
#include <algorithm>
#include <iostream>

using XorListUtility::XorList;

void f1() {
  std::cout << "std::list with std::allocator" << std::endl;
  std::list<int> a;
  double s = clock();
  for (int i(0); i < 10000000; ++i)
    a.push_back(i);
  std::cout << (clock() - s) / CLOCKS_PER_SEC << std::endl;
}

void f2() {
  std::cout << "std::list with StackAllocator" << std::endl;
  std::list<int, StackAllocator<int>> a;
  double s = clock();
  for (int i(0); i < 10000000; ++i)
    a.push_back(i);
  std::cout << (clock() - s) / CLOCKS_PER_SEC  << std::endl;
}

void f3() {
  std::cout << "XorList with std::allocator" << std::endl;
  XorList<int> a;
  double s = clock();
  for (int i(0); i < 10000000; ++i)
    a.push_back(i);
  std::cout << (clock() - s) / CLOCKS_PER_SEC  << std::endl;
}

void f4() {
  std::cout << "XorList with StackAllocator" << std::endl;
  XorList<int, StackAllocator<int>> a;
  double s = clock();
  for (int i(0); i < 10000000; ++i)
    a.push_back(i);
  std::cout << (clock() - s) / CLOCKS_PER_SEC  << std::endl;
}

#include <vector>

void f5() {
  std::cout << "std::vector with std::allocator" << std::endl;
  std::vector<int> a;
  double s = clock();
  for (int i(0); i < 10000000; ++i)
    a.push_back(i);
  std::cout << clock() - s / CLOCKS_PER_SEC  << std::endl;
}

void f6() {
  std::cout << "std::vector with StackAllocator" << std::endl;
  std::vector<int, StackAllocator<int>> a;
  double s = clock();
  for (int i(0); i < 10000000; ++i)
    a.push_back(i);
  std::cout << clock() - s / CLOCKS_PER_SEC  << std::endl;
}

template <typename list>
void show_list(const list& a) {
  std::cout << a.size() << std::endl;
  for (auto& x : a)
    std::cout << x << ' ';
  std::cout << std::endl;
}

template <class list1, class list2>
bool compare_lists(const list1& a, const list2& b) {
  if (a.size() != b.size())
    return false;
  typename list1::const_iterator it_a(a.begin());
  typename list2::const_iterator it_b(b.begin());
  while (it_a != a.end())
  {
    if (*it_a != *it_b)
      return false;
    ++it_a, ++it_b;
  }
  return true;
}



void f7() {
  using namespace std;
  list<int> a;
  XorList<int> b;
  int q; cin >> q;
  while (q--)
  {
    int t; cin >> t;
    if (t == 1) {
      int k; cin >> k;
      a.push_back(k);
      b.push_back(k);
    } else {
      int k; cin >> k;
      a.erase(find(begin(a), end(a), k));
      b.erase(find(begin(b), end(b), k));
    }
    if (!compare_lists(a, b))
      throw "ERROR!!!";
    show_list(a);
    show_list(b);
  }
}

void f8() {
  using namespace std;
  list<int> a;
  XorList<int> b;
  int q(10000); //cin >> q;
  while (q--)
  {
    int t; //cin >> t;
    if (a.empty())
      t = 1;
    else t = (rand() % 100 > 30 ? 0 : 1);
    if (t == 1) {
      int k; k = rand() % 100;//cin >> k;
      a.push_back(k);
      b.push_back(k);
    } else {
      int k; k = rand() % 100;//cin >> k;
      if (find(begin(a), end(a), k) != end(a)) {
        a.erase(find(begin(a), end(a), k));
        b.erase(find(begin(b), end(b), k));
      }
    }
    if (!compare_lists(a, b))
      throw "ERROR!!!";
    //show_list(a);
    //show_list(b);
  }
  cout << "Lists are OK." << endl;
}

int main()
{
  f1();
  f2();
  f3();
  f4();
  //f7();
  f8();
}
