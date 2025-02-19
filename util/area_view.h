#pragma once

#include <cstddef>
#include <vector>
#include <memory>
#include <optional>

template<typename T>
class area_view {
public:
    void configure(const size_t width, const size_t height) {
        m_data = std::make_shared<std::vector<T>>(width * height);
        m_width = width;
        m_height = height;
        m_stride = height;
        m_offset = 0;
    }
    size_t width() const {
        return m_width;
    }
    size_t height() const {
        return m_height;
    }
    std::optional<const T*> at(const int x, const int y) const {
        auto index = y * m_stride + x + m_offset;
        if (m_data != nullptr && index < m_data->size()) {
            return &(m_data->at(index));
        }
        return std::nullopt;
    }
    std::optional<T*> at(const int x, const int y) {
        auto index = y * m_stride + x + m_offset;
        if (m_data != nullptr && index < m_data->size()) {
            return &(m_data->at(index));
        }
        return std::nullopt;
    }

    bool is_set(const int x, const int y) const {
        if (auto p = at(x, y)) {
            return (**p & 1) != 0;
        }
        return false;
    }

    bool is_blocked(const int x, const int y) const {
        if (auto p = at(x, y)) {
            return (**p & 2) != 0;
        }
        return false;
    }

    bool is_blocked() const {
        for (int y = 0; y < m_height; y++) {
            for (int x = 0; x < m_width; x++) {
                if (is_blocked(x, y)) {
                    return true;
                }
            }
        }
        return false;
    }
    void block() {
        for (int y = 0; y < m_height; y++) {
            for (int x = 0; x < m_width; x++) {
                block(x, y);
            }
        }
    }

    void force_set(const int x, const int y) {
        if (auto p = at(x, y)) {
            **p = (**p | 1);
        }
    }
    void force_unset(const int x, const int y) {
        if (auto p = at(x, y)) {
            **p = (**p & 2);
        }
    }
    void set(const int x, const int y) {
        if (!is_blocked(x, y)) {
            if (auto p = at(x, y)) {
                **p = (**p | 1);
            }
        }
    }
    void unset(const int x, const int y) {
        if (!is_blocked(x, y)) {
            if (auto p = at(x, y)) {
                **p = (**p & 2);
            }
        }
    }
    void block(const int x, const int y) {
        if (auto p = at(x, y)) {
            **p = (**p | 2);
        }
    }
    void unblock(const int x, const int y) {
        if (auto p = at(x, y)) {
            **p = (**p & 1);
        }
    }

    template<typename Tcall>
    void for_each(const Tcall& call) const {
        if (m_data) {
            for (int y = 0; y < m_height; y++) {
                for (int x = 0; x < m_width; x++) {
                    auto index = y * m_stride + x + m_offset;
                    call(x, y, m_data->at(index));
                }
            }
        }
    }
    template<typename Tcall>
    void for_each(const Tcall& call) {
        if (m_data) {
            for (int y = 0; y < m_height; y++) {
                for (int x = 0; x < m_width; x++) {
                    auto index = y * m_stride + x + m_offset;
                    call(x, y, m_data->at(index));
                }
            }
        }
    }

    bool contains(const int x, const int y, const size_t width, const size_t height) const {
        return (0 <= x && x + width <= m_width && 0 <= y && y + height <= m_height);
    }
    std::optional<area_view<T>> subregion(const int x, const int y, const size_t width, const size_t height) const {
        area_view<T> result;

        if (contains(x, y, width, height)) {
            result = *this;
            result.m_width = width;
            result.m_height = height;
            result.m_offset += (x + y * m_stride);

            return result;
        }

        return std::nullopt;
    }
    bool apply(const area_view<T>& area) {
        if (m_width != area.m_width || m_height != area.m_height) {
            return false;
        }

        if (is_blocked()) {
            return false;
        }

        area.for_each([&](const auto x, const auto y, const T s) {
            if (auto p = at(x, y)) {
                **p = s;
            }
        });

        return true;
    }
private:
    size_t m_width = 0;
    size_t m_height = 0;
    size_t m_stride = 0;
    size_t m_offset = 0;
    std::shared_ptr<std::vector<T>> m_data;
};

