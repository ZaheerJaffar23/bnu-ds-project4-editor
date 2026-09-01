#ifndef EDITOR_EDIT_QUEUE_HPP
#define EDITOR_EDIT_QUEUE_HPP

#include "Queue.hpp"
#include "UndoRedo.hpp"

#include <string>

struct EditRequest {
    std::string user;
    EditType type = EditType::Insert;
    std::size_t line = 1;
    std::string text;
};

// Arrival-order buffer for simulated concurrent users. The editor never
// applies a request until it is dequeued from the front.
using EditQueue = Queue<EditRequest>;

#endif
