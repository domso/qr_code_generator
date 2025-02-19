#include "data_placer.h"

#include "qr_code.h"

void data_placer::apply(qr_code& code, const galois_field::polynom& data, const galois_field::polynom& ec) {
    placement_position current_position = {
        .x = static_cast<int>(code.area().width()) - 1,
        .y = static_cast<int>(code.area().height()) - 1,
        .direction = upwards_right,
        .valid = true
    };

    for (auto f : data.factors()) {
        auto w = f.value;
        for (int i = 0; i < 8; i++) {
            auto c = (w >> (7 - i)) & 1;

            if (c != 0) {
                code.area().set(current_position.x, current_position.y);
            }

            do {
                current_position = compute_next_position(code, current_position);
                if (current_position.x < 0) {
                    return;
                }
            } while (!current_position.valid);

        }
    }
    for (auto f : ec.factors()) {
        auto w = f.value;
        for (int i = 0; i < 8; i++) {
            auto c = (w >> (7 - i)) & 1;

            if (c != 0) {
                code.area().set(current_position.x, current_position.y);
            }

            do {
                current_position = compute_next_position(code, current_position);
                if (current_position.x < 0) {
                    return;
                }
            } while (!current_position.valid);

        }
    }
}

data_placer::placement_position data_placer::compute_next_position(const qr_code& code, const placement_position& pos) {
    auto new_pos = pos;

    switch (new_pos.direction) {
        case upwards_right: {
            new_pos.direction = upwards_left;
            new_pos.x--;
            break;
        }
        case upwards_left: {
            if (new_pos.y > 0) {
                new_pos.direction = upwards_right;
                new_pos.x++;
                new_pos.y--;
            } else {
                new_pos.direction = downwards_right;
                new_pos.x--;
            }
            break;
        }
        case downwards_right: {
            new_pos.direction = downwards_left;
            new_pos.x--;
            break;
        }
        case downwards_left: {
            if (new_pos.y < code.area().height() - 1) {
                new_pos.direction = downwards_right;
                new_pos.x++;
                new_pos.y++;
            } else {
                new_pos.direction = upwards_right;
                new_pos.x--;
            }
            break;
        }
    }

    if (new_pos.x == 6) {
        new_pos.x--;
    }

    new_pos.valid = (new_pos.x >= 0) && (!code.area().is_blocked(new_pos.x, new_pos.y));

    return new_pos;
}
