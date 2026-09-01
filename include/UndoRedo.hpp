#ifndef EDITOR_UNDO_REDO_HPP
#define EDITOR_UNDO_REDO_HPP

#include "Stack.hpp"

#include <cstddef>
#include <string>

enum class EditType { Insert, Delete, Replace };

struct EditRecord {
    EditType type = EditType::Insert;
    std::size_t line = 0;  // 1-based line the edit affected
    std::string before;
    std::string after;
};

// Two LIFO stacks: a new user edit clears redo. Undo pops undo and
// pushes the inverse onto redo; redo does the opposite.
class UndoRedo {
public:
    void record(const EditRecord& rec) {
        undo_.push(rec);
        redo_.clear();
    }

    bool canUndo() const { return !undo_.empty(); }
    bool canRedo() const { return !redo_.empty(); }
    std::size_t undoSize() const { return undo_.size(); }
    std::size_t redoSize() const { return redo_.size(); }

    EditRecord popUndo() {
        EditRecord rec = undo_.pop();
        redo_.push(rec);
        return rec;
    }

    EditRecord popRedo() {
        EditRecord rec = redo_.pop();
        undo_.push(rec);
        return rec;
    }

    void clear() {
        undo_.clear();
        redo_.clear();
    }

private:
    Stack<EditRecord> undo_;
    Stack<EditRecord> redo_;
};

#endif
