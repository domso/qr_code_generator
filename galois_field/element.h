#pragma once

#include <stdint.h>
#include <assert.h>

#include "context.h"

namespace galois_field {

template<uint8_t primitive = 2, uint16_t primitive_poly = 0b100011101>
class element_base {
public:
    uint8_t value;

    element_base(const context_base<primitive, primitive_poly>& converter) : m_context(converter) {}
    element_base(const context_base<primitive, primitive_poly>& converter, const uint8_t v) : value(v), m_context(converter) {}
    element_base(const element_base<primitive, primitive_poly>& other) : value(other.value), m_context(other.m_context) {}
    
    void operator=(const element_base other) {
        value = other.value;
    }

    element_base operator+(const element_base other) const {
        return element_base(
            m_context,
            static_cast<uint8_t>(value ^ other.value)
        );
    }
    element_base operator+=(const element_base other) {
        *this = (*this + other);
        return *this;
    }
    element_base operator-(const element_base other) const {
        return element_base(
            m_context,
            static_cast<uint8_t>(value ^ other.value)
        );
    }
    element_base operator-=(const element_base other) {
        *this = (*this - other);
        return *this;
    }
    element_base operator*(const element_base other) const {
        if (value == 0 || other.value == 0) {
            return element_base(
                m_context,
                0
            );
        }

        return element_base(
            m_context,
            m_context.decode(static_cast<uint8_t>((m_context.encode(value) + m_context.encode(other.value)) % 255))
        );
    }
    element_base operator*=(const element_base other) {
        *this = (*this * other);
        return *this;
    }
    element_base operator/(const element_base other) const {
        assert(other.value != 0);
        return element_base(
            m_context,
            m_context.decode(static_cast<uint8_t>((m_context.encode(value) + m_context.encode(other.value) * 254) % 255))
        );
    }
    element_base operator/=(const element_base other) {
        *this = (*this / other);
        return *this;
    }
private:
    const context_base<primitive, primitive_poly>& m_context;
};

using element = element_base<>;

}
