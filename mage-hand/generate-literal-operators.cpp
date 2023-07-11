#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <string>


void generate_operator(std::ostream& out, int n, std::string_view typeSuffix) {
    const std::map<char, std::string> mapping{
            {'0', "⁰"},
            {'1', "¹"},
            {'2', "²"},
            {'3', "³"},
            {'4', "⁴"},
            {'5', "⁵"},
            {'6', "⁶"},
            {'7', "⁷"},
            {'8', "⁸"},
            {'9', "⁹"},
    };
    const std::map<std::string_view, std::string_view> typeMapping{
            {"", "int"},
            {"u", "unsigned"},
            {"l", "long"},
            {"ul", "unsigned long"},
            {"ll", "long long"},
            {"ull", "unsigned long long"},
    };
    auto string = std::to_string(n);
    auto mapped = std::accumulate(string.begin(),
            string.end(),
            std::string{},
            [&](std::string s, char c) { return s.append(mapping.at(c)); });
    auto type = typeMapping.at(typeSuffix);

    out << "constexpr " << type << " "                                      //
        << "operator\"\"" << mapped << typeSuffix                           //
        << "(unsigned long long n) { "                                      //
        << "return pow(static_cast<" << type << ">(n), " << string << "); " //
        << "}\n";
}

void generate_header(std::ostream& out) {
    out << "#pragma once\n"      //
           "\n"                  //
           "#include \"math.h\"\n" //
           "\n"                  //
           "namespace mage_hand::literals {\n";
    for (const char* suffix : {"", "u", "l", "ul", "ll", "ull"}) {
        for (int i = 0; i < 64; i++) {
            generate_operator(out, i, suffix);
            if (i % 10 == 9)
                out << "\n";
        }
        out << "\n\n";
    }
    out << "} // namespace mage_hand::literals\n";
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        std::fstream out(argv[1], std::ios::out | std::ios::trunc);
        generate_header(out);
    } else {
        generate_header(std::cout);
    }
    return 0;
}
