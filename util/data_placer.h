#pragma once

#include <array>
#include <vector>
#include <optional>
#include <memory>
#include <stdint.h>

#include "galois_field/polynom.h"
#include "galois_field/polynom_generator.h"

#include "qr_code.h"

class data_placer {
public:
    void apply(qr_code& code, const galois_field::polynom& data, const galois_field::polynom& ec);
private:
    enum placement_direction {
        upwards_left,
        upwards_right,
        downwards_left,
        downwards_right
    };

    struct placement_position {
        int x;
        int y;
        placement_direction direction;
        bool valid;
    };

    placement_position compute_next_position(const qr_code& code, const placement_position& pos);
};
