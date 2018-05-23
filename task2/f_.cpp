//
// Created by kvk1920 on 14.05.18.
//

#include "GlobalAllocatorSwitcher.h"

int* f_() {
  return new(std::nothrow) int;
}