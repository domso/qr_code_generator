#include "qr_code.h"
#include "generators/generator_pipeline.h"
#include "generators/mask_generator.h"
#include "generators/marker_generator.h"
#include "generators/data_generator.h"
#include "generators/version_generator.h"
#include "generators/print_generator.h"

int main(const int argc, const char** args) {
    if (argc == 2) {
        auto command = args[1];

        qr_code code;
        code.configure(command, 'Q');

        generator_pipeline<qr_code, marker_generator, data_generator, mask_generator, version_generator, print_generator> pipeline;
        pipeline.apply(code);

        return 0;
    }

    return -1;
}
