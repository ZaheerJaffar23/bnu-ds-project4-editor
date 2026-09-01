#ifndef EDITOR_EDITOR_HPP
#define EDITOR_EDITOR_HPP

#include "DoublyLinkedList.hpp"

#include <cstddef>
#include <iosfwd>
#include <string>

class Editor {
public:
    using LineNode = DoublyLinkedList<std::string>::Node;

    bool loadFile(const std::string& path);
    void clear();

    bool empty() const { return lines_.empty(); }
    std::size_t lineCount() const { return lines_.size(); }
    std::size_t cursorLine() const { return lines_.empty() ? 0 : cursorIndex_ + 1; }
    LineNode* cursorNode() const { return cursor_; }
    const std::string& currentText() const;

    void moveUp();
    void moveDown();
    bool gotoLine(std::size_t line1Based);

    // Insert after the current line (or as the first line if empty).
    void insertLine(const std::string& text);
    void deleteLine();
    void replaceLine(const std::string& text);

    void printAround(std::ostream& os, int radius = 2) const;

protected:
    // Used by undo/redo so history can mutate the list without recursion.
    void insertLineAt(std::size_t line1Based, const std::string& text);
    void deleteLineAt(std::size_t line1Based);
    void replaceLineAt(std::size_t line1Based, const std::string& text);

    DoublyLinkedList<std::string> lines_;
    LineNode* cursor_ = nullptr;
    std::size_t cursorIndex_ = 0;  // 0-based

    virtual void onLineInserted(std::size_t line1Based, LineNode* node);
    virtual void onLineDeleted(std::size_t line1Based, LineNode* node, const std::string& text);
    virtual void onLineReplaced(std::size_t line1Based, const std::string& oldText, const std::string& newText);
};

#endif
