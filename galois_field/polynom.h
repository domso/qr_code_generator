#pragma once

#include "context.h"
#include "element.h"

#include <vector>
#include <string>

namespace galois_field {

template<uint8_t primitive = 2, uint16_t primitive_poly = 0b100011101>
struct polynom_base {
    polynom_base(const context_base<primitive, primitive_poly>& context) : m_context(context) {}
    polynom_base(const context_base<primitive, primitive_poly>& context, const std::vector<element_base<primitive, primitive_poly>>& f) : m_factors(f), m_context(context) {}
    
    template<typename... Ts>
    polynom_base(const context_base<primitive, primitive_poly>& context, const Ts&... fs) : m_factors({context.template make<element_base<primitive, primitive_poly>>(fs)...}), m_context(context) {}
    polynom_base(const context_base<primitive, primitive_poly>& context, const auto begin, const auto end) : m_factors(begin, end), m_context(context) {}

    void operator=(const polynom_base& other) {
        m_factors = other.m_factors;
    }

    int degree() const {
        return m_factors.size() - 1;
    }

    void set_degree(const int d) {
        m_factors.resize(d + 1, m_context.template make<element_base<primitive, primitive_poly>>(0));
    }

    bool valid() const {
        return !m_factors.empty();
    }

    const auto& factor(const int i) const {
        return m_factors[i].value;
    }
    auto& factor(const int i) {
        return m_factors[i].value;
    }
    auto& factors() {
        return m_factors;
    }
    const auto& factors() const {
        return m_factors;
    }

    polynom_base<primitive, primitive_poly> operator*(const polynom_base<primitive, primitive_poly>& other) const {
        polynom_base<primitive, primitive_poly> result(m_context);
        if (!other.valid()) {
            return result;
        }
        result.set_degree(degree() + other.degree());

        int index = 0;
        for (auto& f : result.m_factors) {
            for (int i = 0; i <= index; i++) {
                int j = index - i;

                if (0 <= i && i <= degree() && 0 <= j && j <= other.degree()) {
                    f += m_factors[i] * other.m_factors[j];
                }
            }

            index++;
        }

        return result;
    }
    polynom_base<primitive, primitive_poly> div_rest(const polynom_base<primitive, primitive_poly>& other) const {
        auto quotient_size = degree() - other.degree() + 1;
        auto rest = *this;

        for (int i = 0; i < quotient_size; i++) {
            if (rest.factor(0) != 0) {
                auto factor = rest.m_factors[0] / other.m_factors[0];
                auto intermediate = other * polynom_base(m_context, {factor});
                intermediate.set_degree(rest.degree());

                for (int j = 0; j <= rest.degree(); j++) {
                    rest.m_factors[j] -= intermediate.m_factors[j];
                }

                rest.m_factors.erase(rest.m_factors.begin());
            } else {
                rest.m_factors.erase(rest.m_factors.begin());
            }
        }

        return rest;
    }

    std::string to_string() const {
        std::string result;
        int n = m_factors.size() - 1;
        for (auto f : m_factors) {
            result += std::to_string(f.value) + "x^" + std::to_string(n);
            if (n > 0)  {
                result += " + ";
            }
            n--;
        }

        return result;
    }
private:
    const context_base<primitive, primitive_poly>& m_context;
    std::vector<element_base<primitive, primitive_poly>> m_factors;
};

using polynom = polynom_base<>;

}
