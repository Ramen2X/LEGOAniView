// From libweaver: https://github.com/itsmattkc/libweaver

#ifndef TYPES_H
#define TYPES_H

#include <cstdio>
#include <cstring>
#include <map>
#include <streambuf>
#include <string>
#include <ostream>
#include <vector>

#if defined(_MSC_VER) && (_MSC_VER < 1600)
// Declare types for MSVC versions less than 2010 (1600) which lacked a stdint.h
typedef unsigned char       uint8_t;
typedef char                int8_t;
typedef unsigned short      uint16_t;
typedef short               int16_t;
typedef unsigned int        uint32_t;
typedef int                 int32_t;
typedef unsigned long long  uint64_t;
typedef long long           int64_t;
#else
#include <stdint.h>
#endif

namespace f {

    class bytearray : public std::vector<char>
    {
    public:
        bytearray() {}
        bytearray(size_t size)
        {
            resize(size);
        }
        bytearray(const char* data, size_t size)
        {
            resize(size);
            memcpy(this->data(), data, size);
        }

        template <typename T>
        T* cast() { return reinterpret_cast<T*>(data()); }

        template <typename T>
        const T* cast() const { return reinterpret_cast<const T*>(data()); }

        void append(const char* data, size_t size)
        {
            size_t current = this->size();
            this->resize(current + size);
            memcpy(this->data() + current, data, size);
        }

        void append(const bytearray& other)
        {
            size_t current = this->size();
            this->resize(current + other.size());
            memcpy(this->data() + current, other.data(), other.size());
        }

        void fill(char c)
        {
            memset(this->data(), c, this->size());
        }

        bytearray left(size_t sz) const
        {
            bytearray b(std::min(sz, this->size()));
            memcpy(b.data(), this->data(), b.size());
            return b;
        }

        bytearray mid(size_t i, size_t size = 0) const
        {
            if (i >= this->size()) {
                return bytearray();
            }

            size_t target = this->size() - i;
            if (size != 0) {
                target = std::min(target, size);
            }

            bytearray b(target);
            memcpy(b.data(), this->data() + i, b.size());
            return b;
        }

        bytearray right(size_t i) const
        {
            if (i >= size()) {
                return *this;
            }

            bytearray b(i);
            memcpy(b.data(), this->data() + this->size() - i, b.size());
            return b;
        }

    };

    class Vector3
    {
    public:
        Vector3() {}
        Vector3(double ix, double iy, double iz)
        {
            x = ix;
            y = iy;
            z = iz;
        }

        double x;
        double y;
        double z;
    };
}

#endif // TYPES_H
