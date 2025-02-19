#include "marker_generator.h"

void marker_generator::apply(qr_code& code) {
    apply_finder_pattern(code);
    apply_alignment_pattern(code);
    apply_timing_pattern(code);
    apply_dark_module(code);
    block_format_region(code);
    block_version_information_region(code);
}

area_view<uint8_t> marker_generator::qr_code_marker(qr_code& code) const {
    area_view<uint8_t> area;

    area.configure(7, 7);
    for (int i = 0; i < 7; i++) {
        area.set(i, 0);
        area.set(i, 6);
        area.set(0, i);
        area.set(6, i);

        area.block(i, 0);
        area.block(i, 6);
        area.block(0, i);
        area.block(6, i);
    }

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            area.set(2 + i, 2 + j);
            area.block(2 + i, 2 + j);
        }
    }

    return area;
}
area_view<uint8_t> marker_generator::qr_code_alignment_pattern(qr_code& code) const {
    area_view<uint8_t> area;

    area.configure(5, 5);
    for (int i = 0; i < 5; i++) {
        area.set(i, 0);
        area.set(i, 4);
        area.set(0, i);
        area.set(4, i);
    }

    area.set(2, 2);

    area.block();

    return area;
}
void marker_generator::apply_finder_pattern(qr_code& code) {
    if (auto subarea = code.area().subregion(0, 0, 7, 7)) {
        subarea->apply(qr_code_marker(code));
    }
    if (auto subarea = code.area().subregion(code.area().width() - 7, 0, 7, 7)) {
        subarea->apply(qr_code_marker(code));
    }
    if (auto subarea = code.area().subregion(0, code.area().height() - 7, 7, 7)) {
        subarea->apply(qr_code_marker(code));
    }
    if (auto subarea = code.area().subregion(0, 0, 8, 8)) {
        subarea->block();
    }
    if (auto subarea = code.area().subregion(code.area().width() - 8, 0, 8, 8)) {
        subarea->block();
    }
    if (auto subarea = code.area().subregion(0, code.area().height() - 8, 8, 8)) {
        subarea->block();
    }
}

void marker_generator::apply_timing_pattern(qr_code& code) {
    for (int x = 8; x < code.area().width() - 8; x += 2) {
        code.area().force_set(x, 6);
    }
    for (int y = 8; y < code.area().height() - 8; y += 2) {
        code.area().force_set(6, y);
    }
    for (int x = 8; x < code.area().width() - 8; x += 1) {
        code.area().block(x, 6);
    }
    for (int y = 8; y < code.area().height() - 8; y += 1) {
        code.area().block(6, y);
    }
}

std::vector<int> marker_generator::alignment_pattern_locations(const int version) const {
    switch (version) {
        case 2: return {6, 18};
        case 3: return {6, 22};
        case 4: return {6, 26};
        case 5: return {6, 30};
        case 6: return {6, 34};
        case 7: return {6, 22, 38};
        case 8: return {6, 24, 42};
        case 9: return {6, 26, 46};
        case 10: return {6, 28, 50};
        case 11: return {6, 30, 54};
        case 12: return {6, 32, 58};
        case 13: return {6, 34, 62};
        case 14: return {6, 26, 46, 66};
        case 15: return {6, 26, 48, 70};
        case 16: return {6, 26, 50, 74};
        case 17: return {6, 30, 54, 78};
        case 18: return {6, 30, 56, 82};
        case 19: return {6, 30, 58, 86};
        case 20: return {6, 34, 62, 90};
        case 21: return {6, 28, 50, 72, 94};
        case 22: return {6, 26, 50, 74, 98};
        case 23: return {6, 30, 54, 78, 102};
        case 24: return {6, 28, 54, 80, 106};
        case 25: return {6, 32, 58, 84, 110};
        case 26: return {6, 30, 58, 86, 114};
        case 27: return {6, 34, 62, 90, 118};
        case 28: return {6, 26, 50, 74, 98, 122};
        case 29: return {6, 30, 54, 78, 102, 126};
        case 30: return {6, 26, 52, 78, 104, 130};
        case 31: return {6, 30, 56, 82, 108, 134};
        case 32: return {6, 34, 60, 86, 112, 138};
        case 33: return {6, 30, 58, 86, 114, 142};
        case 34: return {6, 34, 62, 90, 118, 146};
        case 35: return {6, 30, 54, 78, 102, 126, 150};
        case 36: return {6, 24, 50, 76, 102, 128, 154};
        case 37: return {6, 28, 54, 80, 106, 132, 158};
        case 38: return {6, 32, 58, 84, 110, 136, 162};
        case 39: return {6, 26, 54, 82, 110, 138, 166};
        case 40: return {6, 30, 58, 86, 114, 142, 170};
        default: return {};
    }
}
std::vector<std::pair<int, int>> marker_generator::alignment_pattern_positions(const int version) {
    auto locations = alignment_pattern_locations(version);
    std::vector<std::pair<int, int>> result;
    for (int i = 0; i < locations.size(); i++) {
        for (int j = 0; j < locations.size(); j++) {
            result.push_back({locations[i], locations[j]});
        }
    }

    return result;
}

void marker_generator::apply_alignment_pattern(qr_code& code) {
    auto positions = alignment_pattern_positions(code.info().version);
    for (auto [x, y] : positions) {
        if (auto subarea = code.area().subregion(x - 2, y - 2, 5, 5)) {
            subarea->apply(qr_code_alignment_pattern(code));
        }
    }
}

void marker_generator::apply_dark_module(qr_code& code) {
    code.area().force_set(8, 4 * code.info().version + 9);
}

void marker_generator::block_version_information_region(qr_code& code) {
    if (code.info().version >= 7) {
        code.area().subregion(0, code.area().height() - 11, 6, 3)->block();
        code.area().subregion(code.area().height() - 11, 0, 3, 6)->block();
    }
}
void marker_generator::block_format_region(qr_code& code) {
    code.area().subregion(8, 4 * code.info().version + 9, 1, 8)->block();

    code.area().subregion(8, 0, 1, 9)->block();
    code.area().subregion(0, 8, 9, 1)->block();
    code.area().subregion(code.area().width() - 8, 8, 8, 1)->block();
}
