#pragma once

#include <array>
#include <vector>
#include <optional>
#include <memory>
#include <stdint.h>

#include "galois_field/polynom.h"
#include "util/version_table.h"
#include "util/data_placer.h"

class data_generator {
public:
    void apply(qr_code& code);
private:
    auto compute_generator_polynom(const galois_field::context& context, const int degree);
    auto compute_edc(const galois_field::context& context, const galois_field::polynom& data, const int codewords);

    enum encoding_modes {
        numeric,
        alphanumeric,
        byte,
        kanji,
        eci
    };

    encoding_modes compute_encoding(const std::string& text);

    int compute_length_bit(const int version, const encoding_modes mode);

    auto compute_byte_data(const galois_field::context& context, const std::string& text, const size_t length_size, const size_t codewords);

    template<typename T>
    void data_reorder_caller(const version_table::entry e, const T& call) {
        int columns = std::max(e.size_of_grp_1_blocks, e.size_of_grp_2_blocks);
        int rows = e.blocks_in_grp_1 + e.blocks_in_grp_2;

        int n = 0;
        for (int c = 0; c < columns; c++) {
            for (int r = 0; r < rows; r++) {
                if (r < e.blocks_in_grp_1) {
                    if (c < e.size_of_grp_1_blocks) {
                        call(n, r * e.size_of_grp_1_blocks + c);
                        n++;
                    }
                } else {
                    if (c < e.size_of_grp_2_blocks) {
                        call(n, e.blocks_in_grp_1 * e.size_of_grp_1_blocks + (r - e.blocks_in_grp_1) * e.size_of_grp_2_blocks + c);
                        n++;
                    }
                }
            }
        }
    }
    template<typename T>
    void ec_reorder_caller(const version_table::entry e, const T& call) {
        int columns = e.ec_per_block;
        int rows = e.blocks_in_grp_1 + e.blocks_in_grp_2;

        int n = 0;
        for (int c = 0; c < columns; c++) {
            for (int r = 0; r < rows; r++) {
                call(n, r, c);
                n++;
            }
        }
    }

    std::pair<galois_field::polynom, galois_field::polynom> compute_msg(const qr_code& code);
};
