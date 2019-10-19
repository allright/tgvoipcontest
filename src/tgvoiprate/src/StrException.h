//
// Created by Andrey Syvrachev on 18.10.2019.
//

#ifndef TGVOIPRATE_STREXCEPTION_H
#define TGVOIPRATE_STREXCEPTION_H

#include <exception>

class StrException : public std::exception {
public:
    StrException(const char *format, ...);

    virtual const char *what() const noexcept;

private:
    char _buffer[512];
};

#endif //TGVOIPRATE_STREXCEPTION_H
