#ifndef EDITOR_DOUBLY_LINKED_LIST_HPP
#define EDITOR_DOUBLY_LINKED_LIST_HPP

#include <cstddef>
#include <stdexcept>
#include <utility>

// Doubly linked list of document lines. Cursor movement is O(1) because
// each node stores both prev and next. Index-based access is O(n).
template <typename T>
class DoublyLinkedList {
public:
    struct Node {
        T data;
        Node* prev;
        Node* next;
        explicit Node(T value) : data(std::move(value)), prev(nullptr), next(nullptr) {}
    };

    DoublyLinkedList() = default;

    DoublyLinkedList(const DoublyLinkedList&) = delete;
    DoublyLinkedList& operator=(const DoublyLinkedList&) = delete;

    DoublyLinkedList(DoublyLinkedList&& other) noexcept {
        head_ = other.head_;
        tail_ = other.tail_;
        size_ = other.size_;
        other.head_ = other.tail_ = nullptr;
        other.size_ = 0;
    }

    DoublyLinkedList& operator=(DoublyLinkedList&& other) noexcept {
        if (this != &other) {
            clear();
            head_ = other.head_;
            tail_ = other.tail_;
            size_ = other.size_;
            other.head_ = other.tail_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }

    ~DoublyLinkedList() { clear(); }

    bool empty() const { return size_ == 0; }
    std::size_t size() const { return size_; }
    Node* head() const { return head_; }
    Node* tail() const { return tail_; }

    Node* pushBack(T value) {
        Node* node = new Node(std::move(value));
        if (!tail_) {
            head_ = tail_ = node;
        } else {
            tail_->next = node;
            node->prev = tail_;
            tail_ = node;
        }
        ++size_;
        return node;
    }

    Node* insertAfter(Node* pos, T value) {
        if (!pos) {
            return pushFront(std::move(value));
        }
        if (pos == tail_) {
            return pushBack(std::move(value));
        }
        Node* node = new Node(std::move(value));
        node->prev = pos;
        node->next = pos->next;
        pos->next->prev = node;
        pos->next = node;
        ++size_;
        return node;
    }

    Node* pushFront(T value) {
        Node* node = new Node(std::move(value));
        if (!head_) {
            head_ = tail_ = node;
        } else {
            node->next = head_;
            head_->prev = node;
            head_ = node;
        }
        ++size_;
        return node;
    }

    T erase(Node* node) {
        if (!node) {
            throw std::invalid_argument("erase: null node");
        }
        if (node->prev) {
            node->prev->next = node->next;
        } else {
            head_ = node->next;
        }
        if (node->next) {
            node->next->prev = node->prev;
        } else {
            tail_ = node->prev;
        }
        T value = std::move(node->data);
        delete node;
        --size_;
        return value;
    }

    Node* at(std::size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("line index out of range");
        }
        Node* cur;
        if (index < size_ / 2) {
            cur = head_;
            for (std::size_t i = 0; i < index; ++i) {
                cur = cur->next;
            }
        } else {
            cur = tail_;
            for (std::size_t i = size_ - 1; i > index; --i) {
                cur = cur->prev;
            }
        }
        return cur;
    }

    void clear() {
        Node* cur = head_;
        while (cur) {
            Node* next = cur->next;
            delete cur;
            cur = next;
        }
        head_ = tail_ = nullptr;
        size_ = 0;
    }

private:
    Node* head_ = nullptr;
    Node* tail_ = nullptr;
    std::size_t size_ = 0;
};

#endif
