/*constexpr int value_if(bool cond, int a, int b) noexcept {
    if constexpr (cond) {
        return a;
    } else {
        return b;
    }
}*/

template <bool cond, int a, int b>
struct value_if {
    constexpr static int value = a;
};

template <int a, int b>
struct value_if<false, a, b> {
    constexpr static int value = b;
};
/*
constexpr int get_xor(int value, int ...values) noexcept {
    if constexpr (sizeof...(values) == 0) {
        return value;
    } else {
        return value ^ get_xor(values...);
    }
}*/

template <int value_, int ...values>
struct get_xor {
    constexpr static int value = value_ ^ get_xor<values...>::value;
};

template <int value_>
struct get_xor<value_> {
    constexpr static int value = value_;
};
/*
constexpr int get_heap_number(int xor_all, int current_number, int current_n, int ...n) noexcept {
    if constexpr (xor_all ^ current_n < current_n) {
        return current_number;
    } else {
        return get_heap_number(xor_all, current_number + 1, n...);
    }
}*/
template <int nf, int ...n>
constexpr int get_first() noexcept { return nf; };

template <int xor_all, int current_number, bool ok, int current_n, int ...n>
struct get_heap_number {
    constexpr static int value = current_number;
};

template <int xor_all, int current_number, int current_n, int ...n>
struct get_heap_number <xor_all, current_number, false, current_n, n...> {
    constexpr static int value = get_heap_number<
            xor_all, current_number + 1, (get_first<n...>() > (get_first<n...>() ^ xor_all)), n...
    >::value;
};

template <int xor_all, int current_number, int current_n>
struct get_heap_number <xor_all, current_number, false, current_n> {
    constexpr static int value = 1;
};
/*
constexpr int get_heap_by_number(int i, int heap, int heaps...) noexcept {
    if constexpr (i == 1) {
        return heap;
    } else {
        return get_heap_by_number(i - 1, heaps...);
    }
}*/

template <int i, int heap, int ...heaps>
struct get_heap_by_number {
    constexpr static int value = get_heap_by_number<i - 1, heaps...>::value;
};

template <int heap, int ...heaps>
struct get_heap_by_number <0, heap, heaps...> {
    constexpr static int value = 0;
};

template <int heap, int ...heaps>
struct get_heap_by_number<1, heap, heaps...> {
    constexpr static int value = heap;
};

constexpr int get_k(int xor_all, int current) noexcept {
    return current - (xor_all ^ current);
}

template <int ...vals>
struct AhalaiMahalai {
    constexpr static int who = value_if<get_xor<vals...>::value == 0, 1 + 1, 1>::value;
    constexpr static int whence = value_if<who == 1,
                                           get_heap_number<get_xor<vals...>::value, 1,
                                                   (get_first<vals...>() > (get_first<vals...>() ^ get_xor<vals...>::value))
                                                   , vals...>::value,
                                           0
    >::value;
    constexpr static int how = value_if<who == 1,
                                        get_k(get_xor<vals...>::value, get_heap_by_number<whence, vals...>::value),
                                        0
    >::value;
};