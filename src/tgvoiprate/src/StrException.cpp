//
// Created by Andrey Syvrachev on 18.10.2019.
//

#include "StrException.h"
#include <iostream>
#include <cstdarg>

StrException::StrException(const char *format, ...) {
    va_list ptr;
    va_start(ptr, format);
    vsnprintf(_buffer, sizeof(_buffer), format, ptr);
    va_end(ptr);
    printf("\n");
}

const char *StrException::what() const noexcept {
    return _buffer;
}
