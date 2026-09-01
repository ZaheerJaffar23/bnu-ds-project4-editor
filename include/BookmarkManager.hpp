#ifndef EDITOR_BOOKMARK_MANAGER_HPP
#define EDITOR_BOOKMARK_MANAGER_HPP

#include "DoublyLinkedList.hpp"
#include "HashTable.hpp"

#include <string>
#include <vector>

struct Bookmark {
    DoublyLinkedList<std::string>::Node* node = nullptr;
    std::size_t line = 0;  // 1-based, kept in sync so jumps stay O(1)
};

class BookmarkManager {
public:
    bool add(const std::string& name, DoublyLinkedList<std::string>::Node* node, std::size_t line) {
        if (name.empty() || !node) {
            return false;
        }
        table_.insert(name, Bookmark{node, line});
        return true;
    }

    Bookmark* find(const std::string& name) { return table_.find(name); }
    const Bookmark* find(const std::string& name) const { return table_.find(name); }

    bool erase(const std::string& name) { return table_.erase(name); }
    std::size_t size() const { return table_.size(); }

    void onInserted(std::size_t line1Based) {
        table_.forEach([&](const std::string&, Bookmark& bm) {
            if (bm.line >= line1Based) {
                ++bm.line;
            }
        });
    }

    void onDeleted(std::size_t line1Based, DoublyLinkedList<std::string>::Node* node) {
        std::vector<std::string> doomed;
        table_.forEach([&](const std::string& name, Bookmark& bm) {
            if (bm.node == node) {
                doomed.push_back(name);
            } else if (bm.line > line1Based) {
                --bm.line;
            }
        });
        for (const std::string& name : doomed) {
            table_.erase(name);
        }
    }

    void clear() { table_.clear(); }

    template <typename Fn>
    void forEach(Fn fn) const {
        table_.forEach(fn);
    }

private:
    HashTable<std::string, Bookmark> table_{256};
};

#endif
