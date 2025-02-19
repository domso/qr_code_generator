#pragma once

#include "galois_field/context.h"
#include "util/version_table.h"
#include "util/area_view.h"

class qr_code {
public:
    bool configure(const std::string& text, const char ec_mode) {
        if (auto e = version_table::search(text.length() + 2, ec_mode)) {
            m_area.configure(17 + 4 * e->version, 17 + 4 * e->version);
            m_version = *e;

            m_content = text;

            return true;
        }

        return false;
    }
    const auto& area() const {
        return m_area;
    }
    auto& area() {
        return m_area;
    }
    const version_table::entry& info() const {
        return m_version;
    }
    const auto& context() const {
        return m_context;
    }
    const auto& content() const {
        return m_content;
    }
private:
    std::string m_content;

    version_table::entry m_version;
    galois_field::context m_context;
    area_view<uint8_t> m_area;
};
