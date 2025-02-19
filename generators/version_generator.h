#pragma once

#include "util/area_view.h"
#include "galois_field/polynom.h"
#include "galois_field/polynom_generator.h"
#include "qr_code.h"

class version_generator {
public:
    void apply(qr_code& code);
};
