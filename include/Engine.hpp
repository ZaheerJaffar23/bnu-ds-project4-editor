#ifndef EDITOR_ENGINE_HPP
#define EDITOR_ENGINE_HPP

#include "BookmarkManager.hpp"
#include "BracketChecker.hpp"
#include "EditQueue.hpp"
#include "Editor.hpp"
#include "UndoRedo.hpp"
#include "WordIndex.hpp"

#include <vector>

class Engine : public Editor {
public:
    bool loadFile(const std::string& path);
    void undo();
    void redo();
    bool canUndo() const { return history_.canUndo(); }
    bool canRedo() const { return history_.canRedo(); }
    std::size_t undoSize() const { return history_.undoSize(); }
    std::size_t redoSize() const { return history_.redoSize(); }
    BracketReport checkDocumentBrackets() const { return checkBrackets(lines_); }

    void enqueueEdit(EditRequest request) { edits_.enqueue(std::move(request)); }
    bool processNextEdit();
    std::size_t pendingEdits() const { return edits_.size(); }

    bool addBookmark(const std::string& name);
    bool jumpToBookmark(const std::string& name);
    bool deleteBookmark(const std::string& name);
    std::size_t bookmarkCount() const { return bookmarks_.size(); }
    const BookmarkManager& bookmarks() const { return bookmarks_; }

    std::vector<std::size_t> findWord(const std::string& word) const;
    std::size_t distinctWords() const { return words_.distinctWords(); }

protected:
    void onLineInserted(std::size_t line1Based, LineNode* node) override;
    void onLineDeleted(std::size_t line1Based, LineNode* node, const std::string& text) override;
    void onLineReplaced(std::size_t line1Based, const std::string& oldText,
                        const std::string& newText) override;

private:
    void applyInverse(const EditRecord& rec);
    void applyForward(const EditRecord& rec);

    UndoRedo history_;
    EditQueue edits_;
    BookmarkManager bookmarks_;
    WordIndex words_;
    bool record_ = true;
};

#endif
