//
// Created by Andrey Syvrachev on 18.10.2019.
//

#ifndef TGVOIPRATE_STREXCEPTION_H
#define TGVOIPRATE_STREXCEPTION_H

#include <exception>
#include <iostream>
#include <stdarg.h>

class StrException : public std::exception {
public:
    StrException(const char *format, ...) {
        va_list ptr;
        va_start(ptr, format);
        vsnprintf(_buffer, sizeof(_buffer), format, ptr);
        va_end(ptr);
        printf("\n");
    }

    virtual const char *what() const noexcept {
        return _buffer;
    }

private:
    char _buffer[512];
};

#endif //TGVOIPRATE_STREXCEPTION_H
