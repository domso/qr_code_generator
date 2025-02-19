#pragma once

#include "context.h"
#include "element.h"
#include "polynom.h"

#include <stdint.h>

namespace galois_field {

template<uint8_t primitive = 2, uint16_t primitive_poly = 0b100011101>
class polynom_generator_base {
public:
    polynom_generator_base(const galois_field::context_base<primitive, primitive_poly>& context) : data(context), m_context(context) {}
    galois_field::polynom_base<primitive, primitive_poly> data;

    void add(const int word, const int size) {
        auto remaining = size;

        while (remaining > 0) {
            if (remaining > 8) {
                uint8_t c = (word >> (remaining - 8)) & 0xFF;
                add_char(c, 8);
                remaining -= 8;
            } else {
                add_char(word, remaining);
                remaining = 0;
            }
        }
    }
    void clear() {
        if (bits_in_current > 0) {
            add_char(0, 8 - bits_in_current);
        }
    }
private:
    uint8_t m_current = 0;
    int bits_in_current = 0;
    
    void add_char(const uint8_t c, const int size) {
        auto f = c & (~(0xFF << size));
        int current = m_current;
        current = (current << size) | f;
        bits_in_current += size;

        if (bits_in_current >= 8) {
            data.factors().push_back(galois_field::element_base<primitive, primitive_poly>(m_context, ((current >> (bits_in_current - 8)) & 0xFF)));
            bits_in_current -= 8;
        }
        m_current = current & 0xFF;
    }

    const galois_field::context_base<primitive, primitive_poly>& m_context;
};
using polynom_generator = polynom_generator_base<>;

}
