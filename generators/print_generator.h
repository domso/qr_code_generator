#pragma once

#include "qr_code.h"

class print_generator {
public:
    void apply(const qr_code& code) const;
};
