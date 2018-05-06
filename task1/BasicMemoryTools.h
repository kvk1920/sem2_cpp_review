//
// Created by kvk1920 on 24.04.18.
//

#ifndef TASK1_STACKALLOCATORTOOLS_H
#define TASK1_STACKALLOCATORTOOLS_H

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
#endif //TASK1_STACKALLOCATORTOOLS_H
