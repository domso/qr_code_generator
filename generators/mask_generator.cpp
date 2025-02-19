#include "mask_generator.h"

void mask_generator::apply(qr_code& code) {
    int min_penalty_mask = -1;
    int min_penalty = -1;

    for (int i = 0; i < 8; i++) {
        apply_mask(code, i);
        apply_errorlevel_and_mask(code, i);
        auto penalty = 
            mask_rule_1_penalty(code) +
            mask_rule_2_penalty(code) +
            mask_rule_3_penalty(code) +
            mask_rule_4_penalty(code);

        if (penalty < min_penalty || min_penalty == -1) {
            min_penalty = penalty;
            min_penalty_mask = i;
        }

        apply_mask(code, i);
    }

    apply_mask(code, min_penalty_mask);
    apply_errorlevel_and_mask(code, min_penalty_mask);
}

uint8_t mask_generator::compute_mask_dark_module_test(const int x, const int y, const int mask) {
    switch(mask) {
        case 0: return ((x + y) % 2) == 0;
        case 1: return (y % 2) == 0;
        case 2: return (x % 3) == 0;
        case 3: return ((x + y) % 3) == 0;
        case 4: return ((x / 3 + y / 2) % 2) == 0;
        case 5: return (y * x % 2 + y * x % 3) == 0;
        case 6: return (((x * y) % 2 + y * x % 3) % 2) == 0;
        case 7: return (((x + y) % 2 + y * x % 3) % 2) == 0;
    };

    return 0;
}

void mask_generator::apply_mask(qr_code& code, const int mask) {
    code.area().for_each([&](const int x, const int y, auto& p) {
        if (!code.area().is_blocked(x, y)) {
            p = p ^ compute_mask_dark_module_test(x, y, mask);
        }
    });
}

int mask_generator::ec_level_to_index(const char m) {
    // switcher order tue to little endian style
    switch (m) {
        case 'L': return 1;
        case 'M': return 0;
        case 'Q': return 3;
        case 'H': return 2;
    }
    return 0;
}

void mask_generator::apply_errorlevel_and_mask(qr_code& code, const int mask) {
    galois_field::polynom gen_format(code.context(),
        ec_level_to_index(code.info().ec_level) >> 1,
        ec_level_to_index(code.info().ec_level) & 1,
        mask >> 2,
        (mask >> 1) & 1,
        mask & 1,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    );

    galois_field::polynom gen_poly(code.context(), 1, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1);
    auto ec = gen_format.div_rest(gen_poly);
    auto result = gen_format;
    int i = 0;
    for (const auto& f : ec.factors()) {
        if (5 + i <= result.degree()) {
            result.factor(5 + i) = f.value;
        }
        i++;
    }

    std::vector<uint8_t> result_mask = {1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0};
    for (int i = 0; i <= result.degree(); i++) {
        assert(i < result_mask.size());
        result.factor(i) = result.factor(i) ^ result_mask[i];
    }

    code.area().subregion(8, code.area().height() - 7, 1, 7)->for_each([&](auto x, auto y, auto& p) {
        if (result.factor((6 - y)) > 0) {
            p = p | 1;
        } else {
            p = p & 0xFE;
        }
    });
    code.area().subregion(0, 8, 6, 1)->for_each([&](auto x, auto y, auto& p) {
        if (result.factor(x) > 0) {
            p = p | 1;
        } else {
            p = p & 0xFE;
        }
    });
    code.area().subregion(7, 8, 2, 1)->for_each([&](auto x, auto y, auto& p) {
        if (result.factor(6 + x) > 0) {
            p = p | 1;
        } else {
            p = p & 0xFE;
        }
    });
    code.area().subregion(code.area().width() - 8, 8, 8, 1)->for_each([&](auto x, auto y, auto& p) {
        if (result.factor(7 + x) > 0) {
            p = p | 1;
        } else {
            p = p & 0xFE;
        }
    });
    code.area().subregion(8, 0, 1, 6)->for_each([&](auto x, auto y, auto& p) {
        if (result.factor(14 - y) > 0) {
            p = p | 1;
        } else {
            p = p & 0xFE;
        }
    });
    code.area().subregion(8, 7, 1, 1)->for_each([&](auto x, auto y, auto& p) {
        if (result.factor(8) > 0) {
            p = p | 1;
        } else {
            p = p & 0xFE;
        }
    });
}


int mask_generator::mask_rule_1_penalty(const qr_code& code) {
    int penalty = 0;

    auto add_to_penalty = [&](const int a, const int b) {
        auto size = b - a;
        if (a >= 0 && size >= 5) {
            penalty += size - 2;
        }
    };

    for (int y = 0; y < code.area().height(); y++) {
        int found = -1;
        for (int x = 0; x < code.area().width(); x++) {
            if (found == -1) {
                found = x;
            } else {
                if (code.area().is_set(found, y) != code.area().is_set(x, y)) {
                    add_to_penalty(found, x);
                    found = x;
                }
            }
        }
        add_to_penalty(found, code.area().width());
    }
    for (int x = 0; x < code.area().width(); x++) {
        int found = -1;
        for (int y = 0; y < code.area().height(); y++) {
            if (found == -1) {
                found = y;
            } else {
                if (code.area().is_set(x, found) != code.area().is_set(x, y)) {
                    add_to_penalty(found, y);
                    found = y;
                }
            }
        }
        add_to_penalty(found, code.area().height());
    }

    return penalty;
}

int mask_generator::mask_rule_2_penalty(const qr_code& code) {
    int penalty = 0;

    for (int y = 0; y < code.area().height() - 1; y++) {
        for (int x = 0; x < code.area().width() - 1; x++) {
            if (
                (code.area().is_set(x, y) == code.area().is_set(x + 1, y)) &&
                (code.area().is_set(x, y) == code.area().is_set(x, y + 1)) &&
                (code.area().is_set(x, y) == code.area().is_set(x + 1, y + 1))
            ) {
                penalty += 3;
            }
        }
    }

    return penalty;
}

int mask_generator::mask_rule_3_penalty(const qr_code& code) {
    int penalty = 0;
    auto patterns = std::vector<std::vector<int>>{{1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0}, {0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1}};
    for (int y = 0; y < code.area().height(); y++) {
        for (int x = 0; x < code.area().width(); x++) {
            for (const auto& pattern : patterns) {
                if (x + pattern.size() <= code.area().width()) {
                    int dx = 0;
                    for (auto p : pattern) {
                        if ((p == 1) != code.area().is_set(x + dx, y)) {
                            break;
                        }
                        dx++;
                    }
                    if (dx == pattern.size()) {
                        penalty += 40;
                    }
                }
                if (y + pattern.size() <= code.area().height()) {
                    int dy = 0;
                    for (auto p : pattern) {
                        if ((p == 1) != code.area().is_set(x, y + dy)) {
                            break;
                        }
                        dy++;
                    }
                    if (dy == pattern.size()) {
                        penalty += 40;
                    }
                }
            }
        }
    }

    return penalty;
}

int mask_generator::mask_rule_4_penalty(const qr_code& code) {
    int dark = 0;
    int light = 0;

    for (int y = 0; y < code.area().height(); y++) {
        for (int x = 0; x < code.area().width(); x++) {
            if (code.area().is_set(x, y)) {
                dark++;
            } else {
                light++;
            }
        }
    }

    int percentage = (100 * dark) / (light + dark);
    int rest = (100 * dark) % (light + dark);
    int percentage_aligned_by_5 = (percentage / 5) * 5;

    if (percentage_aligned_by_5 != percentage) {
        if (percentage < 50) {
            percentage_aligned_by_5 += 5;            
        }
    } else {
        if (percentage < 50 && rest > 0) {
            percentage_aligned_by_5 += 5;            
        }
    }

    return std::abs(percentage_aligned_by_5 - 50) * 2;
}
