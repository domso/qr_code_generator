#include "print_generator.h"

#include <iostream>

void print_generator::apply(const qr_code& code) const {
    //auto white = "⬜";
    //auto black = "⬛";
    //auto reset = "";

    auto white = "\033[47m  ";
    auto black = "\033[40m  ";
    auto reset = "\033[0m";

    for (int x = 0; x < code.area().width() + 2; x++) {
        std::cout << white;
    }
    std::cout << reset << std::endl;
    for (int y = 0; y < code.area().height(); y++) {
        std::cout << white;
        for (int x = 0; x < code.area().width(); x++) {
            if (code.area().is_set(x, y)) {
                std::cout << black;
            } else if (code.area().is_blocked(x, y)) {
                std::cout << white;
            } else {
                std::cout << white;
            }
        }
        std::cout << white;
        std::cout << reset << std::endl;
    }
    for (int x = 0; x < code.area().width() + 2; x++) {
        std::cout << white;
    }
    std::cout << reset << std::endl;
}
