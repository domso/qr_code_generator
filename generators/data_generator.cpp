#include "data_generator.h"

void data_generator::apply(qr_code& code) {
    auto [msg, ec] = compute_msg(code);
    data_placer dp;
    dp.apply(code, msg, ec);
}

auto data_generator::compute_generator_polynom(const galois_field::context& context, const int degree) {
    galois_field::polynom prev(context, 1);

    for (int i = 0; i < degree; i++) {
        prev = prev * galois_field::polynom(context, {
            context.template make<galois_field::element>(1),
            context.template make<galois_field::element>(context.decode(i)),
        });
    }

    return prev;
}

auto data_generator::compute_edc(const galois_field::context& context, const galois_field::polynom& data, const int codewords) {
    auto degree = codewords - data.degree() - 1;
    auto msg = data;
    msg.set_degree(codewords - 1);
    return msg.div_rest(compute_generator_polynom(context, degree));
}

enum encoding_modes {
    numeric,
    alphanumeric,
    byte,
    kanji,
    eci
};

data_generator::encoding_modes data_generator::compute_encoding(const std::string& text) {
    bool is_numeric = true;
    bool is_alphanumeric = true;
    bool is_byte = true;

    for (auto c : text) {
        is_numeric = is_numeric && ('0' <= c && c <= '9');
        is_alphanumeric = is_alphanumeric && (
            ('0' <= c && c <= '9') ||
            ('A' <= c && c <= 'Z') ||
            (c == ' ') ||
            (c == '$') ||
            (c == '%') ||
            (c == '*') ||
            (c == '+') ||
            (c == '-') ||
            (c == '.') ||
            (c == '/') ||
            (c == ':') 
        );
    }

    if (is_numeric) {
        return numeric;
    }
    if (is_alphanumeric) {
        return alphanumeric;
    }
    return byte;
}

int data_generator::compute_length_bit(const int version, const encoding_modes mode) {
    int column = 0;
    if (10 <= version && version <= 26) {
        column = 1;
    }
    if (27 <= version && version <= 40) {
        column = 2;
    }
    switch (mode) {
        case numeric: return std::array<int, 3>{10, 12, 14}[column];
        case alphanumeric: return std::array<int, 3>{9, 11, 13}[column];
        case byte: return std::array<int, 3>{8, 16, 16}[column];
        case kanji: return std::array<int, 3>{8, 10, 12}[column];
        case eci: return std::array<int, 3>{8, 10, 12}[column]; // not sure
    }

    return 0;
}

auto data_generator::compute_byte_data(const galois_field::context& context, const std::string& text, const size_t length_size, const size_t codewords) {
    galois_field::polynom_generator gen(context);

    gen.add(4, 4);
    gen.add(text.length(), length_size);
    for (auto c : text) {
        gen.add(c, 8);
    }
    gen.clear();
    
    int n = 0;
    while (gen.data.degree() != codewords - 1) {
        if ((n & 1) == 0) {
            gen.add(236, 8);
        } else {
            gen.add(17, 8);
        }

        n++;
    }

    gen.clear();

    return gen.data;
}

std::pair<galois_field::polynom, galois_field::polynom> data_generator::compute_msg(const qr_code& code) {
    encoding_modes encoding = compute_encoding(code.content());
    auto length_size = compute_length_bit(code.info().version, encoding);
    auto data = compute_byte_data(code.context(), code.content(), length_size, code.info().total_data);

    std::vector<galois_field::polynom> ecs;
    int offset = 0;
    for (int grp1 = 0; grp1 < code.info().blocks_in_grp_1; grp1++) {
        galois_field::polynom block(code.context(), data.factors().begin() + offset, data.factors().begin() + offset + code.info().size_of_grp_1_blocks);
        auto ec_of_block = compute_edc(code.context(), block, code.info().size_of_grp_1_blocks + code.info().ec_per_block);
        ecs.push_back(ec_of_block);
        offset += code.info().size_of_grp_1_blocks;
    }
    for (int grp2 = 0; grp2 < code.info().blocks_in_grp_2; grp2++) {
        galois_field::polynom block(code.context(), data.factors().begin() + offset, data.factors().begin() + offset + code.info().size_of_grp_2_blocks);
        auto ec_of_block = compute_edc(code.context(), block, code.info().size_of_grp_2_blocks + code.info().ec_per_block);
        ecs.push_back(ec_of_block);
        offset += code.info().size_of_grp_2_blocks;
    }
    
    galois_field::polynom reordered_data(code.context());
    reordered_data.set_degree(data.degree());
    data_reorder_caller(code.info(), [&](auto dest, auto src) {
        reordered_data.factor(dest) = data.factor(src);
    });

    galois_field::polynom reordered_ecc(code.context());
    reordered_ecc.set_degree((code.info().blocks_in_grp_1 + code.info().blocks_in_grp_2) * code.info().ec_per_block - 1);
    ec_reorder_caller(code.info(), [&](auto dest, auto index, auto i) {
        reordered_ecc.factor(dest) = ecs[index].factor(i);
    });

    return {reordered_data, reordered_ecc};
}
