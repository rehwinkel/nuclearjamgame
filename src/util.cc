#include "util.h"

#include <iostream>

void panic(const char* message) {
    std::cerr << "[PANIC] " << message << std::endl;
    abort();
}