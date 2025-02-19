#pragma once

#include <array>
#include <vector>
#include <optional>
#include <memory>
#include <stdint.h>

#include "galois_field/polynom.h"
#include "galois_field/polynom_generator.h"
#include "util/version_table.h"
#include "qr_code.h"

class mask_generator {
public:
    void apply(qr_code& code);
private:
    uint8_t compute_mask_dark_module_test(const int x, const int y, const int mask);

    void apply_mask(qr_code& code, const int mask);
    int ec_level_to_index(const char m);
    void apply_errorlevel_and_mask(qr_code& code, const int mask);

    int mask_rule_1_penalty(const qr_code& code);
    int mask_rule_2_penalty(const qr_code& code);
    int mask_rule_3_penalty(const qr_code& code);
    int mask_rule_4_penalty(const qr_code& code);
};
