#include "Engine.hpp"

#include <vector>

bool Engine::loadFile(const std::string& path) {
    bookmarks_.clear();
    history_.clear();
    edits_.clear();
    words_.clear();
    if (!Editor::loadFile(path)) {
        return false;
    }
    words_.rebuild(lines_.head());
    return true;
}

void Engine::onLineInserted(std::size_t line1Based, LineNode*) {
    bookmarks_.onInserted(line1Based);
    words_.addLine(line1Based, currentText());
    if (record_) {
        history_.record({EditType::Insert, line1Based, "", currentText()});
    }
}

void Engine::onLineDeleted(std::size_t line1Based, LineNode* node, const std::string& text) {
    bookmarks_.onDeleted(line1Based, node);
    words_.removeLine(line1Based, text);
    if (record_) {
        history_.record({EditType::Delete, line1Based, text, ""});
    }
}

void Engine::onLineReplaced(std::size_t line1Based, const std::string& oldText,
                           const std::string& newText) {
    words_.replaceLine(line1Based, oldText, newText);
    if (record_) {
        history_.record({EditType::Replace, line1Based, oldText, newText});
    }
}

void Engine::applyInverse(const EditRecord& rec) {
    record_ = false;
    switch (rec.type) {
        case EditType::Insert:
            deleteLineAt(rec.line);
            break;
        case EditType::Delete:
            insertLineAt(rec.line, rec.before);
            break;
        case EditType::Replace:
            replaceLineAt(rec.line, rec.before);
            break;
    }
    record_ = true;
}

void Engine::applyForward(const EditRecord& rec) {
    record_ = false;
    switch (rec.type) {
        case EditType::Insert:
            insertLineAt(rec.line, rec.after);
            break;
        case EditType::Delete:
            deleteLineAt(rec.line);
            break;
        case EditType::Replace:
            replaceLineAt(rec.line, rec.after);
            break;
    }
    record_ = true;
}

void Engine::undo() {
    if (!history_.canUndo()) {
        return;
    }
    applyInverse(history_.popUndo());
}

void Engine::redo() {
    if (!history_.canRedo()) {
        return;
    }
    applyForward(history_.popRedo());
}

bool Engine::processNextEdit() {
    if (edits_.empty()) {
        return false;
    }
    const EditRequest req = edits_.dequeue();
    switch (req.type) {
        case EditType::Insert:
            insertLineAt(req.line, req.text);
            break;
        case EditType::Delete:
            deleteLineAt(req.line);
            break;
        case EditType::Replace:
            replaceLineAt(req.line, req.text);
            break;
    }
    return true;
}

bool Engine::addBookmark(const std::string& name) {
    if (!cursor_) {
        return false;
    }
    return bookmarks_.add(name, cursor_, cursorIndex_ + 1);
}

bool Engine::jumpToBookmark(const std::string& name) {
    Bookmark* bm = bookmarks_.find(name);
    if (!bm || !bm->node) {
        return false;
    }
    cursor_ = bm->node;
    cursorIndex_ = bm->line - 1;
    return true;
}

bool Engine::deleteBookmark(const std::string& name) {
    return bookmarks_.erase(name);
}

std::vector<std::size_t> Engine::findWord(const std::string& word) const {
    const std::vector<std::size_t>* lines = words_.find(word);
    if (!lines) {
        return {};
    }
    return *lines;
}
