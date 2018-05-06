cmake_minimum_required(VERSION 3.9)
project(task1)

set(CMAKE_CXX_STANDARD 14)

add_executable(task1 main.cpp BasicMemoryTools.h MemoryBlock.cpp MemoryBlock.h MemoryStack.cpp MemoryStack.h SmartPointer.h StackAllocator.h XorList.h)