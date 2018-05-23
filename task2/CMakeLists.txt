cmake_minimum_required(VERSION 3.9)
project(task2)

set(CMAKE_CXX_STANDARD 14)

add_executable(task2 main.cpp MemoryBlock.cpp MemoryBlock.h BasicMemoryTools.h MemoryStack.h
        MemoryStack.cpp StackAllocator.h SmartPointer.h
        Allocators.h Allocators.cpp
        GlobalAllocatorSwitcher.h CAllocatedOn.h f_.cpp GlobalAllocatorSwitcher.cpp)