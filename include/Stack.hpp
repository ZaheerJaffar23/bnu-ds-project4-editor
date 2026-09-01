#ifndef EDITOR_STACK_HPP
#define EDITOR_STACK_HPP

#include <cstddef>
#include <stdexcept>
#include <utility>

// Linked-list stack (LIFO). Used for undo/redo history and for the
// bracket-matching scan. Push/pop/top are O(1).
template <typename T>
class Stack {
public:
    Stack() = default;
    Stack(const Stack&) = delete;
    Stack& operator=(const Stack&) = delete;

    ~Stack() { clear(); }

    bool empty() const { return size_ == 0; }
    std::size_t size() const { return size_; }

    void push(T value) {
        Node* node = new Node(std::move(value), top_);
        top_ = node;
        ++size_;
    }

    T pop() {
        if (!top_) {
            throw std::runtime_error("pop from empty stack");
        }
        Node* node = top_;
        T value = std::move(node->data);
        top_ = node->next;
        delete node;
        --size_;
        return value;
    }

    T& top() {
        if (!top_) {
            throw std::runtime_error("top of empty stack");
        }
        return top_->data;
    }

    const T& top() const {
        if (!top_) {
            throw std::runtime_error("top of empty stack");
        }
        return top_->data;
    }

    void clear() {
        while (top_) {
            Node* next = top_->next;
            delete top_;
            top_ = next;
        }
        size_ = 0;
    }

private:
    struct Node {
        T data;
        Node* next;
        Node(T value, Node* n) : data(std::move(value)), next(n) {}
    };

    Node* top_ = nullptr;
    std::size_t size_ = 0;
};

#endif
