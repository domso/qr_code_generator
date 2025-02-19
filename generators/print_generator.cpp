#include "print_generator.h"

#include <iostream>

void print_generator::apply(const qr_code& code) const {
    for (int x = 0; x < code.area().width() + 2; x++) {
        std::cout << "\033[47m  ";
    }
        std::cout << "\033[0m" << std::endl;
    for (int y = 0; y < code.area().height(); y++) {
        std::cout << "\033[47m  ";
        for (int x = 0; x < code.area().width(); x++) {
            if (code.area().is_set(x, y)) {
                std::cout << "\033[40m  ";
            } else if (code.area().is_blocked(x, y)) {
                std::cout << "\033[47m  ";
            } else {
                std::cout << "\033[47m  ";
            }
        }
        std::cout << "\033[47m  ";
        std::cout << "\033[0m" << std::endl;
    }
    for (int x = 0; x < code.area().width() + 2; x++) {
        std::cout << "\033[47m  ";
    }
        std::cout << "\033[0m" << std::endl;
}
