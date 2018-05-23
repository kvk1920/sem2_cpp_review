#include "ahalaimahalai.h"
#include <iostream>

constexpr int N_1 = 2;
constexpr int N_2 = 2;
constexpr int N_3 = 0;

template <int ...N>
struct Test {
    Test() {
        std::cout << AhalaiMahalai<N...>::who << '\n';
        std::cout << AhalaiMahalai<N...>::whence << '\n';
        std::cout << AhalaiMahalai<N...>::how;
    }
};

int main() {
  /*constexpr int who = AhalaiMahalai<N_1, N_2, N_3>::who;
  constexpr int whence = AhalaiMahalai<N_1, N_2, N_3>::whence;
  constexpr int how = AhalaiMahalai<N_1, N_2, N_3>::how;*/
  //std::cout << who << ' ' << whence << ' ' << how;
    Test<4, 2, 1>();
}
