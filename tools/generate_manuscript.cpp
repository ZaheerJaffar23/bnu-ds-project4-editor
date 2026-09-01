#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// Builds a manuscript-scale document: ~20,000 lines, ~120,000 words,
// and ~8,000 distinct tokens so the word-index hash table is well loaded.

int main(int argc, char* argv[]) {
    const char* path = argc > 1 ? argv[1] : "data/manuscript.txt";
    const int lines = argc > 2 ? std::atoi(argv[2]) : 20000;
    const int wordsPerLine = 6;
    const int distinct = 8000;

    std::vector<std::string> vocab;
    vocab.reserve(distinct);
    static const char* stems[] = {
        "river", "stone", "cloud", "ember", "grove", "harbor", "lantern", "meadow",
        "needle", "orbit", "petal", "quartz", "ridge", "sail", "timber", "umber",
        "vale", "willow", "xenon", "yarrow", "zephyr", "anchor", "bramble", "cinder",
        "delta", "echo", "flint", "glacier", "hollow", "ivory", "jasper", "kelp"};
    const int stemCount = sizeof(stems) / sizeof(stems[0]);
    int n = 0;
    while (static_cast<int>(vocab.size()) < distinct) {
        vocab.push_back(std::string(stems[n % stemCount]) + std::to_string(n / stemCount));
        ++n;
    }

    std::ofstream out(path);
    if (!out) {
        std::cerr << "cannot write " << path << "\n";
        return 1;
    }

    unsigned seed = 20240541u;
    auto rnd = [&]() {
        seed = seed * 1664525u + 1013904223u;
        return seed;
    };

    for (int i = 0; i < lines; ++i) {
        if (i % 250 == 0) {
            out << "chapter marker { section[" << (i / 250) << "] }\n";
            continue;
        }
        for (int w = 0; w < wordsPerLine; ++w) {
            if (w) {
                out << ' ';
            }
            out << vocab[rnd() % distinct];
        }
        out << '\n';
    }

    std::cout << "wrote " << path << " (" << lines << " lines, vocab " << distinct << ")\n";
    return 0;
}
