#pragma once

#include <array>
#include <stdint.h>

namespace galois_field {
    template<uint8_t primitive = 2, uint16_t primitive_poly = 0b100011101>
    class context_base {
    public:
        context_base() {
            int x = 1;
            for (int i = 0; i < 256; i++) {
                m_log[x] = i % 255;
                m_exp[i] = x;

                x *= primitive;

                if (x >= 256) {
                    x = (x ^ primitive_poly);
                }
            }

            m_exp[255] = m_exp[0];
        }

        int encode(const uint8_t v) const {
            return m_log[v];
        }
        uint8_t decode(const int v) const {
            return m_exp[v];
        }

        template<typename T, typename... T_args>
        auto make(const T_args&... args) const {
            return T(*this, args...);
        }
    private:
        std::array<uint8_t, 256> m_log;
        std::array<uint8_t, 256> m_exp;
    };
    using context = context_base<>;
}
