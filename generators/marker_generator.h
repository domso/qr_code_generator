#include <iostream>
#include <array>
#include <vector>
#include <optional>
#include <memory>
#include <stdint.h>

#include "qr_code.h"

class marker_generator {
public:
    void apply(qr_code& code);
private:
    area_view<uint8_t> qr_code_marker(qr_code& code) const;
    area_view<uint8_t> qr_code_alignment_pattern(qr_code& code) const;

    void apply_finder_pattern(qr_code& code);
    void apply_timing_pattern(qr_code& code);

    std::vector<int> alignment_pattern_locations(const int version) const;
    std::vector<std::pair<int, int>> alignment_pattern_positions(const int version);

    void apply_alignment_pattern(qr_code& code);
    void apply_dark_module(qr_code& code);

    void block_version_information_region(qr_code& code);
    void block_format_region(qr_code& code);
};

