#ifndef EDITOR_QUEUE_HPP
#define EDITOR_QUEUE_HPP

#include <cstddef>
#include <stdexcept>
#include <utility>

// Linked-list FIFO queue. Multi-user edit requests are stored in arrival
// order: enqueue at the tail, dequeue from the head, both O(1).
template <typename T>
class Queue {
public:
    Queue() = default;
    Queue(const Queue&) = delete;
    Queue& operator=(const Queue&) = delete;

    ~Queue() { clear(); }

    bool empty() const { return size_ == 0; }
    std::size_t size() const { return size_; }

    void enqueue(T value) {
        Node* node = new Node(std::move(value));
        if (!tail_) {
            head_ = tail_ = node;
        } else {
            tail_->next = node;
            tail_ = node;
        }
        ++size_;
    }

    T dequeue() {
        if (!head_) {
            throw std::runtime_error("dequeue from empty queue");
        }
        Node* node = head_;
        T value = std::move(node->data);
        head_ = node->next;
        if (!head_) {
            tail_ = nullptr;
        }
        delete node;
        --size_;
        return value;
    }

    T& front() {
        if (!head_) {
            throw std::runtime_error("front of empty queue");
        }
        return head_->data;
    }

    void clear() {
        while (head_) {
            Node* next = head_->next;
            delete head_;
            head_ = next;
        }
        tail_ = nullptr;
        size_ = 0;
    }

private:
    struct Node {
        T data;
        Node* next;
        explicit Node(T value) : data(std::move(value)), next(nullptr) {}
    };

    Node* head_ = nullptr;
    Node* tail_ = nullptr;
    std::size_t size_ = 0;
};

#endif
