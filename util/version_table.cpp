#include "version_table.h"

namespace version_table {
    std::optional<entry> search(const int msg_words, const char level) {
        switch (level) {
            case 'L': {
                for (auto& e : low_table) {
                    if (e.total_data >= msg_words) {
                        return e;
                    }
                }
            }
            case 'M': {
                for (auto& e : medium_table) {
                    if (e.total_data >= msg_words) {
                        return e;
                    }
                }
            }
            case 'Q': {
                for (auto& e : quartile_table) {
                    if (e.total_data >= msg_words) {
                        return e;
                    }
                }
            }
            case 'H': {
                for (auto& e : high_table) {
                    if (e.total_data >= msg_words) {
                        return e;
                    }
                }
            }
        }

        return std::nullopt;
    }
}
