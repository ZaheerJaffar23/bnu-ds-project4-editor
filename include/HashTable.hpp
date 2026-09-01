#ifndef EDITOR_HASH_TABLE_HPP
#define EDITOR_HASH_TABLE_HPP

#include <cstddef>
#include <functional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

// Separate-chaining hash table. Average O(1) insert / find / erase at a
// load factor below 0.75. Used for named bookmarks and the word index.
template <typename K, typename V, typename Hash = std::hash<K>>
class HashTable {
public:
    explicit HashTable(std::size_t buckets = 64) : buckets_(nextPow2(buckets)) {}

    HashTable(const HashTable&) = delete;
    HashTable& operator=(const HashTable&) = delete;

    ~HashTable() { clear(); }

    std::size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }
    std::size_t bucketCount() const { return buckets_.size(); }

    void insert(const K& key, V value) {
        if (V* existing = find(key)) {
            *existing = std::move(value);
            return;
        }
        if ((size_ + 1) * 4 >= buckets_.size() * 3) {
            rehash(buckets_.size() * 2);
        }
        const std::size_t i = indexOf(key);
        buckets_[i] = new Node(key, std::move(value), buckets_[i]);
        ++size_;
    }

    V* find(const K& key) {
        const std::size_t i = indexOf(key);
        for (Node* n = buckets_[i]; n; n = n->next) {
            if (n->key == key) {
                return &n->value;
            }
        }
        return nullptr;
    }

    const V* find(const K& key) const {
        const std::size_t i = indexOf(key);
        for (Node* n = buckets_[i]; n; n = n->next) {
            if (n->key == key) {
                return &n->value;
            }
        }
        return nullptr;
    }

    bool erase(const K& key) {
        const std::size_t i = indexOf(key);
        Node* prev = nullptr;
        Node* cur = buckets_[i];
        while (cur) {
            if (cur->key == key) {
                if (prev) {
                    prev->next = cur->next;
                } else {
                    buckets_[i] = cur->next;
                }
                delete cur;
                --size_;
                return true;
            }
            prev = cur;
            cur = cur->next;
        }
        return false;
    }

    bool contains(const K& key) const { return find(key) != nullptr; }

    template <typename Fn>
    void forEach(Fn fn) {
        for (Node* head : buckets_) {
            for (Node* n = head; n; n = n->next) {
                fn(n->key, n->value);
            }
        }
    }

    template <typename Fn>
    void forEach(Fn fn) const {
        for (Node* head : buckets_) {
            for (Node* n = head; n; n = n->next) {
                fn(n->key, n->value);
            }
        }
    }

    void clear() {
        for (Node*& head : buckets_) {
            while (head) {
                Node* next = head->next;
                delete head;
                head = next;
            }
        }
        size_ = 0;
    }

private:
    struct Node {
        K key;
        V value;
        Node* next;
        Node(K k, V v, Node* n) : key(std::move(k)), value(std::move(v)), next(n) {}
    };

    std::size_t indexOf(const K& key) const {
        return Hash{}(key) & (buckets_.size() - 1);
    }

    static std::size_t nextPow2(std::size_t n) {
        std::size_t p = 1;
        while (p < n) {
            p <<= 1;
        }
        return p < 8 ? 8 : p;
    }

    void rehash(std::size_t newCount) {
        std::vector<Node*> old = std::move(buckets_);
        buckets_.assign(newCount, nullptr);
        size_ = 0;
        for (Node* head : old) {
            while (head) {
                Node* next = head->next;
                const std::size_t i = indexOf(head->key);
                head->next = buckets_[i];
                buckets_[i] = head;
                ++size_;
                head = next;
            }
        }
    }

    std::vector<Node*> buckets_;
    std::size_t size_ = 0;
};

#endif
