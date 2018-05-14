#ifndef AHALAIMAHALAI
#define AHALAIMAHALAI

typedef long long size_t;

template <bool condition, size_t a, size_t b>
struct value_if<condition, a, b> {
  constexpr static size_t value = a;
};

template <size_t a, size_t b>
struct value_if<false, a, b> {
  constexpr static size_t value = b;
};

template<size_t value, size_t ...args>
constexpr size_t Xor() noexcept {
  return value_if<sizeof...() == 0, value, Xor()
}

#endif