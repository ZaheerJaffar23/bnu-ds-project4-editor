#ifndef EDITOR_WORD_INDEX_HPP
#define EDITOR_WORD_INDEX_HPP

#include "HashTable.hpp"

#include <cctype>
#include <string>
#include <vector>

template <typename Fn>
inline void forEachWord(const std::string& text, Fn fn) {
    std::string word;
    for (unsigned char ch : text) {
        if (std::isalnum(ch)) {
            word.push_back(static_cast<char>(std::tolower(ch)));
        } else if (!word.empty()) {
            fn(word);
            word.clear();
        }
    }
    if (!word.empty()) {
        fn(word);
    }
}

// Maps each distinct word to the 1-based line numbers where it occurs.
// Same hash table as bookmarks, keyed by word rather than name.
class WordIndex {
public:
    void addLine(std::size_t line1Based, const std::string& text) {
        shiftFrom(line1Based, +1);
        insertWords(line1Based, text);
    }

    void removeLine(std::size_t line1Based, const std::string& text) {
        eraseWords(line1Based, text);
        shiftFrom(line1Based + 1, -1);
    }

    void replaceLine(std::size_t line1Based, const std::string& oldText, const std::string& newText) {
        eraseWords(line1Based, oldText);
        insertWords(line1Based, newText);
    }

    const std::vector<std::size_t>* find(const std::string& word) const {
        std::string key = word;
        for (char& c : key) {
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }
        return table_.find(key);
    }

    std::size_t distinctWords() const { return table_.size(); }

    void clear() { table_.clear(); }

    template <typename Node>
    void rebuild(Node* head) {
        clear();
        std::size_t line = 1;
        for (auto* n = head; n; n = n->next, ++line) {
            insertWords(line, n->data);
        }
    }

private:
    void insertWords(std::size_t line, const std::string& text) {
        forEachWord(text, [&](const std::string& word) {
            std::vector<std::size_t>* lines = table_.find(word);
            if (!lines) {
                table_.insert(word, std::vector<std::size_t>{line});
            } else if (lines->empty() || lines->back() != line) {
                lines->push_back(line);
            }
        });
    }

    void eraseWords(std::size_t line, const std::string& text) {
        forEachWord(text, [&](const std::string& word) {
            std::vector<std::size_t>* lines = table_.find(word);
            if (!lines) {
                return;
            }
            auto it = lines->begin();
            while (it != lines->end()) {
                if (*it == line) {
                    it = lines->erase(it);
                } else {
                    ++it;
                }
            }
            if (lines->empty()) {
                table_.erase(word);
            }
        });
    }

    void shiftFrom(std::size_t startLine, int delta) {
        table_.forEach([&](const std::string&, std::vector<std::size_t>& lines) {
            for (std::size_t& n : lines) {
                if (n >= startLine) {
                    n = static_cast<std::size_t>(static_cast<long>(n) + delta);
                }
            }
        });
    }

    HashTable<std::string, std::vector<std::size_t>> table_{4096};
};

#endif
