#include "version_generator.h"

void version_generator::apply(qr_code& code) {
    if (code.info().version < 7) {
        // not needed for small codes
        return;
    }
    auto version = code.info().version;
    galois_field::polynom version_divisor(code.context(), 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1);
    galois_field::polynom version_poly(code.context(), 
        (version >> 5) & 1, (version >> 4) & 1, (version >> 3) & 1, (version >> 2) & 1, (version >> 1) & 1, (version) & 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    );

    auto version_dividend = version_poly;

    auto ec = version_dividend.div_rest(version_divisor);
    for (int i = 0; i < 12; i++) {
        version_poly.factor(6 + i) = ec.factor(i);
    }
    code.area().subregion(0, code.area().height() - 11, 6, 3)->for_each([&](auto x, auto y, auto& p) {
        if (version_poly.factor(x * 3 + y) > 0) {
            p = p | 1;
        } else {
            p = p & 0xFE;
        }
    });

    code.area().subregion(code.area().width() - 11, 0, 3, 6)->for_each([&](auto x, auto y, auto& p) {
        if (version_poly.factor((2 - x) + (5 - y) * 3) > 0) {
            p = p | 1;
        } else {
            p = p & 0xFE;
        }
    });
}

