#include "Editor.hpp"

#include <fstream>
#include <ostream>
#include <stdexcept>

const std::string& Editor::currentText() const {
    if (!cursor_) {
        throw std::runtime_error("no current line");
    }
    return cursor_->data;
}

void Editor::clear() {
    lines_.clear();
    cursor_ = nullptr;
    cursorIndex_ = 0;
}


bool Editor::loadFile(const std::string& path) {
    std::ifstream in(path);
    if (!in) {
        return false;
    }
    clear();
    std::string line;
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        lines_.pushBack(line);
    }
    cursor_ = lines_.head();
    cursorIndex_ = 0;
    return true;
}

void Editor::moveUp() {
    if (cursor_ && cursor_->prev) {
        cursor_ = cursor_->prev;
        --cursorIndex_;
    }
}

void Editor::moveDown() {
    if (cursor_ && cursor_->next) {
        cursor_ = cursor_->next;
        ++cursorIndex_;
    }
}

bool Editor::gotoLine(std::size_t line1Based) {
    if (line1Based == 0 || line1Based > lines_.size()) {
        return false;
    }
    const std::size_t target = line1Based - 1;
    if (!cursor_) {
        return false;
    }
    while (cursorIndex_ < target) {
        cursor_ = cursor_->next;
        ++cursorIndex_;
    }
    while (cursorIndex_ > target) {
        cursor_ = cursor_->prev;
        --cursorIndex_;
    }
    return true;
}

void Editor::insertLine(const std::string& text) {
    if (!cursor_) {
        insertLineAt(1, text);
        return;
    }
    insertLineAt(cursorIndex_ + 2, text);
}

void Editor::deleteLine() {
    if (!cursor_) {
        return;
    }
    deleteLineAt(cursorIndex_ + 1);
}

void Editor::replaceLine(const std::string& text) {
    if (!cursor_) {
        insertLineAt(1, text);
        return;
    }
    replaceLineAt(cursorIndex_ + 1, text);
}

void Editor::insertLineAt(std::size_t line1Based, const std::string& text) {
    LineNode* node = nullptr;
    if (lines_.empty()) {
        node = lines_.pushBack(text);
        cursor_ = node;
        cursorIndex_ = 0;
        onLineInserted(1, node);
        return;
    }
    if (line1Based <= 1) {
        node = lines_.pushFront(text);
        if (cursor_) {
            ++cursorIndex_;
        }
        cursor_ = node;
        cursorIndex_ = 0;
        onLineInserted(1, node);
        return;
    }
    if (line1Based > lines_.size() + 1) {
        line1Based = lines_.size() + 1;
    }
    gotoLine(line1Based - 1);
    node = lines_.insertAfter(cursor_, text);
    cursor_ = node;
    cursorIndex_ = line1Based - 1;
    onLineInserted(line1Based, node);
}

void Editor::deleteLineAt(std::size_t line1Based) {
    if (lines_.empty() || !gotoLine(line1Based)) {
        return;
    }
    LineNode* doomed = cursor_;
    LineNode* next = doomed->next;
    LineNode* prev = doomed->prev;
    const std::string text = doomed->data;
    const std::size_t idx = cursorIndex_;
    onLineDeleted(line1Based, doomed, text);
    lines_.erase(doomed);
    if (next) {
        cursor_ = next;
        cursorIndex_ = idx;
    } else if (prev) {
        cursor_ = prev;
        cursorIndex_ = idx - 1;
    } else {
        cursor_ = nullptr;
        cursorIndex_ = 0;
    }
}

void Editor::replaceLineAt(std::size_t line1Based, const std::string& text) {
    if (lines_.empty()) {
        insertLineAt(1, text);
        return;
    }
    if (!gotoLine(line1Based)) {
        return;
    }
    const std::string oldText = cursor_->data;
    cursor_->data = text;
    onLineReplaced(line1Based, oldText, text);
}

void Editor::printAround(std::ostream& os, int radius) const {
    if (!cursor_) {
        os << "(empty document)\n";
        return;
    }
    LineNode* start = cursor_;
    std::size_t startIndex = cursorIndex_;
    for (int i = 0; i < radius && start->prev; ++i) {
        start = start->prev;
        --startIndex;
    }
    std::size_t i = startIndex;
    for (LineNode* n = start; n; n = n->next, ++i) {
        os << (n == cursor_ ? ">" : " ") << " " << (i + 1) << " | " << n->data << "\n";
        if (i >= cursorIndex_ + static_cast<std::size_t>(radius)) {
            break;
        }
    }
}

void Editor::onLineInserted(std::size_t, LineNode*) {}
void Editor::onLineDeleted(std::size_t, LineNode*, const std::string&) {}
void Editor::onLineReplaced(std::size_t, const std::string&, const std::string&) {}
