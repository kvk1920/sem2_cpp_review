#include <type_traits>

namespace TupleUtils {

  template <size_t i, typename current_type, typename ...Args>
  struct __get_type {
      typedef typename __get_type<i - 1, Args...>::type type;
  };

  template <typename current_type, typename ...Args>
  struct __get_type<0, current_type, Args...> {
      typedef current_type type;
  };

  template <typename T>
  struct __remove_reference {
      typedef T type;
  };

  template <typename T>
  struct __remove_reference<T&> {
      typedef T type;
  };

  template <typename T>
  struct __remove_reference<T&&> {
      typedef T type;
  };

  template <typename T>
  using __remove_reference_t = typename __remove_reference<T>::type;

  template <typename T>
  struct __remove_const {
      typedef T type;
  };

  template <typename T>
  struct __remove_const<const T> {
      typedef T type;
  };

  template <typename T>
  using __remove_const_t = typename __remove_const<T>::type;

  template <typename T>
  struct __remove__reference_wrapper {
     typedef T type;
  };

  template <typename T>
  struct __remove__reference_wrapper<std::reference_wrapper <T>> {
      typedef T& type;
  };

  template <typename T>
  using __remove_reference_wrapper_t = typename __remove__reference_wrapper<T>::type;

  template <typename T>
  struct __remove_array {
      typedef T type;
  };

  template <typename T>
  struct __remove_array<T[]> {
      typedef T* type;
  };

  template <typename T>
  using __remove_array_t = typename __remove_array<T>::type;

  template <typename T>
  struct __normalise_type {
      typedef
      __remove_reference_wrapper_t<
              __remove_array_t<
                      __remove_reference_t<T>
                      >
              >
      type;
  };

  template <typename T>
  using __normalise_type_t = typename __normalise_type<T>::type;

}

template <typename T, typename ...Args>
struct Tuple {

    constexpr static bool one_length = false;
    typedef T first_type;
    typedef Tuple<Args...> tail_type;

    T __head;
    Tuple<Args...> __tail;

    /*
    template <typename U, typename TupleType>
    explicit Tuple(U&& t, TupleType&& other)
            : __head(std::forward<U>(t))
            , __tail(std::forward<TupleType>(other))
    {};

    template <typename U>
    explicit Tuple(U&& head, Tuple<Args...>&& tail)
            : __head(std::forward<U>(head))
            , __tail(std::move(tail))
    {}

    template <typename U>
    explicit Tuple(U&& head, const Tuple<Args...>& tail)
            : __head(std::forward<U>(head))
            , __tail(tail)
    {}
*/
    Tuple() = default;
    ~Tuple() = default;

    explicit Tuple(const T& t, const Args& ...args)
            : __head(t)
            , __tail(args...)
    {}

    template <typename ...UArgs>
    explicit Tuple(T&& t, UArgs&& ...args)
            : __head(std::move(t))
            , __tail(std::forward<UArgs>(args)...)
    {};


    template <typename ...UArgs>
    explicit Tuple(const T& t, UArgs&& ...args)
            : __head((t))
              , __tail(std::forward<UArgs>(args)...)
    {};

    Tuple(const Tuple& other)
            : __head(other.__head)
            , __tail(other.__tail)
    {}

    Tuple(Tuple&& other) noexcept
            : __head(std::move(other.__head))
            , __tail(std::move(other.__tail))
    {}

    Tuple& operator=(const Tuple& other) {
        __head = other.__head;
        __tail = other.__tail;
        return *this;
    }

    Tuple& operator=(Tuple&& other) noexcept {
        __head = std::move(other.__head);
        __tail = std::move(other.__tail);
        return *this;
    }

    void swap(Tuple& other) {
        std::swap(__head, other.__head);
        __tail.swap(other.__head);
    }
};

template <typename T>
struct Tuple<T> {

    constexpr static bool one_length = true;
    typedef T first_type;
    T __head;

    Tuple() = default;
    ~Tuple() = default;

    explicit Tuple(const T& t)
            : __head(t)
    {}

    /*template<typename U>
    explicit Tuple(U&& t) noexcept
            : __head(std::forward<U>(t))
    {}*/
    explicit Tuple(T&& t) noexcept
        : __head(std::move(t))
    {}

    Tuple(const Tuple& other)
            : __head(other.__head)
    {}

    Tuple(Tuple&& other) noexcept
            : __head(std::move(other.__head))
    {}

    Tuple& operator=(const Tuple& other) {
        __head = other.__head;
        return *this;
    }

    Tuple& operator=(Tuple&& other) noexcept {
        __head = std::move(other.__head);
        return *this;
    }

    void swap(Tuple& other) {
        std::swap(__head, other.__head);
    }
};
/*
template <typename T, typename ...Args>
Tuple<T, Args...> __buildTuple(T&& head, Tuple<Args...>&& tail) {
    Tuple<T, Args...> result;
    result.__tail = std::move(tail);
    result.__head = std::forward<T>(head);
    return result;
};

template <typename T, typename ...Args>
Tuple<T, Args...> __buildTuple(T&& head, const Tuple<Args...>& tail) {
    Tuple<T, Args...> result;
    result.__tail = std::move(tail);
    result.__head = std::forward<T>(head);
    return result;
};

template <typename T, typename ...Args>
Tuple<T, Args...> __buildTuple(T&& head, Tuple<Args...>& tail) {
    Tuple<T, Args...> result;
    result.__tail = std::move(tail);
    result.__head = std::forward<T>(head);
    return result;
};
*/

template <typename T>
inline bool operator<(const Tuple<T>& t1, const Tuple<T>& t2) {
    return t1.__head < t2.__head;
}

template <typename ...Args>
inline bool operator<(const Tuple<Args...>& tuple1, const Tuple<Args...>& tuple2) {
    /*if constexpr (sizeof...(Args) == 1) {
        return tuple1.__head < tuple2.__head;
    } else {*/
        if (tuple1.__head == tuple2.__head) {
            return tuple1.__tail < tuple2.__tail;
        } else {
            return tuple1.__head < tuple2.__head;
        }
    //}
}

template <typename ...Args>
inline bool operator>(const Tuple<Args...>& tuple1, const Tuple<Args...>& tuple2) {
    return tuple2 < tuple1;
}

template <typename ...Args>
inline bool operator>=(const Tuple<Args...>& tuple1, const Tuple<Args...>& tuple2) {
    return !(tuple1 < tuple2);
}

template <typename ...Args>
inline bool operator<=(const Tuple<Args...>& tuple1, const Tuple<Args...>& tuple2) {
    return !(tuple1 > tuple2);
}

template <typename T>
inline bool operator==(const Tuple<T>& t1, const Tuple<T>& t2) {
    return t1.__head == t2.__head;
}

template <typename ...Args>
inline bool operator==(const Tuple<Args...>& tuple1, const Tuple<Args...>& tuple2) {
    /*if constexpr (sizeof...(Args) == 1) {
        return tuple1.__head == tuple2.__head;
    } else {*/
        if (tuple1.__head == tuple2.__head) {
            return tuple1.__tail == tuple2.__tail;
        } else {
            return false;
        }
    //}
}

template <typename ...Args>
inline bool operator!=(const Tuple<Args...>& tuple1, const Tuple<Args...>& tuple2) {
    return !(tuple1 == tuple2);
}
/*
template <typename ...Args>
Tuple<TupleUtils::__normalise_type_t<Args>...> makeTuple(Args&& ...args) {
    return Tuple<TupleUtils::__normalise_type_t<Args>...>(std::forward<Args>(args)...);
}
*/
template <typename ...Args>
Tuple<std::decay_t<Args>...> makeTuple(Args&& ...args) {
    return Tuple<std::decay_t<Args>...>(std::forward<Args>(args)...);
}
template <typename TupleType>
using __first_tuple_t = typename TupleUtils::__remove_reference_t<TupleType>::first_type;

template <typename T, typename TupleType, typename = std::enable_if_t<std::is_same<__first_tuple_t<TupleType>, T>::value, char> >
decltype(auto) get(TupleType&& tuple) {
    return std::forward<decltype(tuple.__head)>(tuple.__head);
}

template <typename T, typename TupleType>
decltype(auto) get(TupleType&& tuple, std::enable_if_t<!std::is_same<__first_tuple_t<TupleType>, T>::value, char> c = ' ') {
    return std::forward<decltype(
    get<T>(std::forward<decltype(tuple.__tail)>(tuple.__tail)
    ))>(get<T>(std::forward<decltype(tuple.__tail)>(tuple.__tail)));
}

template <size_t i, typename TupleType, typename = std::enable_if_t<i == 0, char>>
decltype(auto) get(TupleType&& tuple) {
    return std::forward<decltype(tuple.__head)>(tuple.__head);
}

template <size_t i, typename TupleType>
decltype(auto) get(TupleType&& tuple, std::enable_if_t<i != 0, char> _c_ = ' ') {

    return std::forward<decltype(get<i - 1>(std::forward<decltype(tuple.__tail)>(tuple.__tail)))>
            (
                    get<i - 1>(std::forward<decltype(tuple.__tail)>(tuple.__tail))
            );
};

template <typename T1, typename pack2>
struct _concatType;

template <typename ...pack1, typename ...pack2>
struct _concatType<Tuple<pack1...>, Tuple<pack2...>> {
    typedef Tuple<pack1..., pack2...> type;
};

template <typename T1, typename T2>
using concatType = typename _concatType<T1, T2>::type;

template <typename TupleType, typename ...Tuples>
struct _r_concatType {
    typedef concatType<TupleType, typename _r_concatType<Tuples...>::type > type;
};

template <typename TupleType>
struct _r_concatType<TupleType> {
    typedef TupleType type;
};

template <typename TupleType, typename ...Tuples>
using r_concatType = typename _r_concatType<TupleType, Tuples...>::type;

template <typename T>
using __pure_type = std::remove_const_t<std::remove_reference_t<T>>;

template <bool one_element>
struct __tupleCat2 {
    template <typename Tuple1, typename Tuple2>
    constexpr static decltype(auto) f(Tuple1&& t1, Tuple2&& t2) {
        concatType<__pure_type<Tuple1>, __pure_type<Tuple2>> result;
        result.__head = std::forward<decltype(t1.__head)>(t1.__head);
        //if constexpr (__pure_type<Tuple1>::one_length) {
            result.__tail = std::forward<Tuple2>(t2);
        /*} else {
            result.__tail = tupleCat2(std::forward<decltype(t1.__tail)>(t1.__tail), std::forward<Tuple2>(t2));
        }*/
        return result;
    }
};

template <>
struct __tupleCat2<false> {
    template <typename Tuple1, typename Tuple2>
    constexpr static decltype(auto) f(Tuple1&& t1, Tuple2&& t2) {
        concatType<__pure_type<Tuple1>, __pure_type<Tuple2>> result;
        result.__head = std::forward<decltype(t1.__head)>(t1.__head);
        /*if constexpr (__pure_type<Tuple1>::one_length) {
            result.__tail = std::forward<Tuple2>(t2);
        } else {*/
            result.__tail = tupleCat2(std::forward<decltype(t1.__tail)>(t1.__tail), std::forward<Tuple2>(t2));
        //}
        return result;
    }
};

template <typename Tuple1, typename Tuple2>
decltype(auto) tupleCat2(Tuple1&& t1, Tuple2&& t2) {
    return __tupleCat2<__pure_type<Tuple1>::one_length>::f(std::forward<Tuple1>(t1), std::forward<Tuple2>(t2));
};

/*template <typename Tuple1, typename Tuple2>
decltype(auto) tupleCat2(Tuple1&& t1, Tuple2&& t2) {
    concatType<__pure_type<Tuple1>, __pure_type<Tuple2>> result;
    result.__head = std::forward<decltype(t1.__head)>(t1.__head);
    if constexpr (__pure_type<Tuple1>::one_length) {
        result.__tail = std::forward<Tuple2>(t2);
    } else {
        result.__tail = tupleCat2(std::forward<decltype(t1.__tail)>(t1.__tail), std::forward<Tuple2>(t2));
    }
    return result;
};*/

template <typename TupleType>
decltype(auto) tupleCat(TupleType&& tuple) {
    return tuple;
}

template <typename TupleType, typename ...Tuples>
decltype(auto) tupleCat(TupleType&& tuple, Tuples&& ...tuples) {
    /*if constexpr (sizeof...(tuples) == 0) {
        return tuple;
    } else {
        */return tupleCat2(std::forward<TupleType>(tuple), tupleCat(std::forward<Tuples>(tuples)...));
    //}
};

/*
template <typename TupleType, typename ...Tuples>
decltype(auto) tupleCat(TupleType&& tuple, Tuples&& ...tuples) {
    if constexpr (sizeof...(tuples) == 0) {
        return tuple;
    } else {
        return tupleCat2(std::forward<TupleType>(tuple), tupleCat(std::forward<Tuples>(tuples)...));
    }
};*/

/*
template <typename T1, typename T2>
struct __typeOfTupleConcat;

template <typename ...Args1, typename ...Args2>
struct __typeOfTupleConcat <Tuple<Args1...>, Tuple<Args2...>> {
    typedef Tuple<Args1..., Args2...> type;
};

template <typename T, typename ...Args>
struct __typeOfTupleConcat <Tuple<T>, Tuple<Args...>> {
    typedef Tuple<T, Args...> type;
};

template <typename Tuple1, typename Tuple2, bool isOne>
struct __concatTypes {
    typedef typename __concatTypes<typename Tuple1::first_type,
            typename __concatTypes<typename Tuple1::tail_type, Tuple2, Tuple1::tail_type::one_length>::type, true>::type type;
};

template <typename T, typename ...Args>
struct __concatTypes<Tuple<T>, Tuple<Args...>, true> {
    typedef Tuple<T, Args...> type;
};

template <typename Tuple1, typename Tuple2, bool isOne = Tuple1::one_length>
using __concatTypes_t = typename __concatTypes <Tuple1, Tuple2, isOne>::type;

template <typename TupleType1, typename TupleType2>
decltype(auto) tupleCat(TupleType1&& tuple1, TupleType2&& tuple2) {
    typedef std::remove_all_extents_t <TupleType1> TT1;
    typedef std::remove_all_extents <TupleType2> TT2;
    return std::forward<
            decltype(__concatTypes_t<TupleType1, TupleType2>::Tuple(std::forward<TupleType1>(tuple1), std::forward<TT2>(tuple2)))
                    >(
            __concatTypes_t<TupleType1, TupleType2>::Tuple(std::forward<TupleType1>(tuple1), std::forward<TT2>(tuple2))
    );
};
*/
/*
template <typename T1, typename T2>
struct __makeTupleType ;{
    typedef void type;
};*/
/*
template <typename T, typename ...Args>
struct __makeTupleType<T, Tuple<Args...>> {
    typedef Tuple<T, Args...> type;
};

template <typename T, typename ...Args>
struct __makeTupleType<Tuple<T>, Tuple<Args...>> {
    typedef Tuple<T, Args...> type;
};

template <typename ...Args1, typename ...Args2>
struct __makeTupleType<Tuple<Args1...>, Tuple<Args2...>> {
    typedef
        typename __makeTupleType<
            typename
                Tuple<
                    Args1...
                >::first_type,
            typename
                __makeTupleType<
                    typename Tuple<Args1...>::tail_type, Tuple<Args2...>
                >::type
    >::type type;
};

template <typename T>
using __pure_type = std::remove_reference_t <std::remove_const_t<T>>;

template <typename TupleType1, typename TupleType2>
decltype(auto) tupleCat2(TupleType1&& tuple1, TupleType2&& tuple2){
    typedef __pure_type<TupleType1> T1;
    typedef __pure_type<TupleType2> T2;
    if constexpr (T1::one_length) {
        return __buildTuple<typename __makeTupleType<typename __pure_type<TupleType1>::first_type, __pure_type <TupleType2>>::type>
                (std::forward<decltype(tuple1.__head)>(tuple1.__head), std::forward<TupleType2>(tuple2));
    } else {
        return __buildTuple<typename __makeTupleType<T1, T2>::type>(
                std::forward<typename __pure_type<TupleType1>::first_type>(tuple1.__head),
                std::forward<typename __makeTupleType<typename __pure_type<TupleType1>::tail_type,
                        __pure_type<TupleType2>>::type>(tupleCat2(std::forward<decltype(tuple1.__tail)>(tuple1.__tail),
                std::forward<TupleType2>(tuple2))));
    }
};

template <typename Tuple, typename ...Tuples>
decltype(auto) tupleCat(Tuple&& tuple, Tuples&& ...tuples) {
    if constexpr (sizeof...(tuples) == 0) {
        return tuple;
    } else {
        return tupleCat2(std::forward<Tuple>(tuple), tupleCat(std::forward<Tuples>(tuples)...));
    }
};*/