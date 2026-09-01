#include "Engine.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {

void printHelp() {
    std::cout
        << "Commands:\n"
        << "  load <path>              Load a document\n"
        << "  status                   Line count, cursor, undo/redo, index size\n"
        << "  print [radius]            Show lines around the cursor\n"
        << "  up / down / goto <n>     Move the cursor\n"
        << "  insert <text>            Insert a line after the cursor\n"
        << "  delete                   Delete the current line\n"
        << "  replace <text>           Replace the current line\n"
        << "  undo / redo              Reverse or replay edits\n"
        << "  brackets                 Stack-check (), [], {}\n"
        << "  enqueue <user> insert|delete|replace <line> [text]\n"
        << "  process                  Apply the next queued multi-user edit\n"
        << "  bookmark add|jump|del <name>\n"
        << "  bookmarks                List named bookmarks\n"
        << "  find <word>              Line numbers for a word\n"
        << "  help / quit\n";
}

void printStatus(const Engine& editor) {
    std::cout << "lines=" << editor.lineCount()
              << " cursor=" << editor.cursorLine()
              << " undo=" << editor.undoSize()
              << " redo=" << editor.redoSize()
              << " queued=" << editor.pendingEdits()
              << " bookmarks=" << editor.bookmarkCount()
              << " words=" << editor.distinctWords() << "\n";
}

void printBrackets(const Engine& editor) {
    const BracketReport report = editor.checkDocumentBrackets();
    if (report.balanced) {
        std::cout << "brackets: balanced\n";
        return;
    }
    std::cout << "brackets: NOT balanced\n";
    for (const BracketIssue& issue : report.issues) {
        std::cout << "  line " << issue.line << " col " << issue.column << ": "
                  << issue.message;
        if (issue.expected) {
            std::cout << " (expected '" << issue.expected << "')";
        }
        std::cout << "\n";
    }
}

void printBookmarks(const Engine& editor) {
    if (editor.bookmarkCount() == 0) {
        std::cout << "(no bookmarks)\n";
        return;
    }
    editor.bookmarks().forEach([](const std::string& name, const Bookmark& bm) {
        std::cout << "  " << name << " -> line " << bm.line << "\n";
    });
}

bool handleCommand(Engine& editor, const std::string& line) {
    std::istringstream in(line);
    std::string cmd;
    in >> cmd;
    if (cmd.empty()) {
        return true;
    }
    if (cmd == "quit" || cmd == "exit") {
        return false;
    }
    if (cmd == "help") {
        printHelp();
    } else if (cmd == "load") {
        std::string path;
        in >> path;
        if (!editor.loadFile(path)) {
            std::cout << "failed to load " << path << "\n";
        } else {
            std::cout << "loaded " << editor.lineCount() << " lines\n";
        }
    } else if (cmd == "status") {
        printStatus(editor);
    } else if (cmd == "print") {
        int radius = 3;
        in >> radius;
        editor.printAround(std::cout, radius);
    } else if (cmd == "up") {
        editor.moveUp();
    } else if (cmd == "down") {
        editor.moveDown();
    } else if (cmd == "goto") {
        std::size_t n = 0;
        in >> n;
        if (!editor.gotoLine(n)) {
            std::cout << "invalid line\n";
        }
    } else if (cmd == "insert") {
        std::string text;
        std::getline(in, text);
        if (!text.empty() && text[0] == ' ') {
            text.erase(0, 1);
        }
        editor.insertLine(text);
    } else if (cmd == "delete") {
        editor.deleteLine();
    } else if (cmd == "replace") {
        std::string text;
        std::getline(in, text);
        if (!text.empty() && text[0] == ' ') {
            text.erase(0, 1);
        }
        editor.replaceLine(text);
    } else if (cmd == "undo") {
        if (!editor.canUndo()) {
            std::cout << "nothing to undo\n";
        } else {
            editor.undo();
        }
    } else if (cmd == "redo") {
        if (!editor.canRedo()) {
            std::cout << "nothing to redo\n";
        } else {
            editor.redo();
        }
    } else if (cmd == "brackets") {
        printBrackets(editor);
    } else if (cmd == "enqueue") {
        EditRequest req;
        std::string type;
        in >> req.user >> type >> req.line;
        std::getline(in, req.text);
        if (!req.text.empty() && req.text[0] == ' ') {
            req.text.erase(0, 1);
        }
        if (type == "insert") {
            req.type = EditType::Insert;
        } else if (type == "delete") {
            req.type = EditType::Delete;
        } else if (type == "replace") {
            req.type = EditType::Replace;
        } else {
            std::cout << "unknown edit type\n";
            return true;
        }
        editor.enqueueEdit(std::move(req));
        std::cout << "queued (" << editor.pendingEdits() << " pending)\n";
    } else if (cmd == "process") {
        if (!editor.processNextEdit()) {
            std::cout << "queue empty\n";
        }
    } else if (cmd == "bookmark") {
        std::string sub, name;
        in >> sub >> name;
        if (sub == "add") {
            std::cout << (editor.addBookmark(name) ? "ok\n" : "failed\n");
        } else if (sub == "jump") {
            std::cout << (editor.jumpToBookmark(name) ? "ok\n" : "not found\n");
        } else if (sub == "del") {
            std::cout << (editor.deleteBookmark(name) ? "ok\n" : "not found\n");
        } else {
            std::cout << "usage: bookmark add|jump|del <name>\n";
        }
    } else if (cmd == "bookmarks") {
        printBookmarks(editor);
    } else if (cmd == "find") {
        std::string word;
        in >> word;
        const std::vector<std::size_t> hits = editor.findWord(word);
        if (hits.empty()) {
            std::cout << "no occurrences of '" << word << "'\n";
        } else {
            std::cout << word << " occurs on " << hits.size() << " line(s):";
            const std::size_t shown = hits.size() < 20 ? hits.size() : 20;
            for (std::size_t i = 0; i < shown; ++i) {
                std::cout << " " << hits[i];
            }
            if (hits.size() > shown) {
                std::cout << " ...";
            }
            std::cout << "\n";
        }
    } else {
        std::cout << "unknown command; type help\n";
    }
    return true;
}

int runDemo(Engine& editor, const std::string& path) {
    if (!path.empty() && !editor.loadFile(path)) {
        std::cerr << "could not load " << path << "\n";
        return 1;
    }
    if (editor.empty()) {
        editor.insertLine("int main() {");
        editor.insertLine("    if (ready) { work(); }");
        editor.insertLine("    return 0;");
        editor.insertLine("}");
    }

    std::cout << "=== demo: document + cursor ===\n";
    editor.gotoLine(1);
    editor.printAround(std::cout, 2);
    printStatus(editor);

    std::cout << "\n=== demo: brackets (official file, before edits) ===\n";
    printBrackets(editor);

    std::cout << "\n=== demo: insert / undo / redo ===\n";
    editor.insertLine("// cursor demo line");
    editor.printAround(std::cout, 1);
    editor.undo();
    std::cout << "after undo, cursor line " << editor.cursorLine() << "\n";
    editor.redo();
    std::cout << "after redo, current: " << editor.currentText() << "\n";

    std::cout << "\n=== demo: multi-user queue ===\n";
    editor.enqueueEdit({"alice", EditType::Insert, 2, "// edit from alice"});
    editor.enqueueEdit({"bob", EditType::Replace, 1, "// edit from bob"});
    while (editor.processNextEdit()) {
    }
    editor.gotoLine(1);
    editor.printAround(std::cout, 2);

    std::cout << "\n=== demo: bookmarks ===\n";
    editor.gotoLine(1);
    editor.addBookmark("start");
    if (editor.lineCount() >= 4) {
        editor.gotoLine(4);
        editor.addBookmark("end");
    }
    editor.jumpToBookmark("start");
    std::cout << "jumped to start, line " << editor.cursorLine() << "\n";
    printBookmarks(editor);

    std::cout << "\n=== demo: word index ===\n";
    const std::vector<std::size_t> hits = editor.findWord("edit");
    std::cout << "word 'edit' on " << hits.size() << " line(s)";
    if (!hits.empty()) {
        std::cout << " (first " << hits.front() << ")";
    }
    std::cout << ", distinct words=" << editor.distinctWords() << "\n";
    return 0;
}

}  // namespace

int main(int argc, char* argv[]) {
    Engine editor;
    bool demo = false;
    std::string path;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--demo") {
            demo = true;
        } else if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        } else {
            path = arg;
        }
    }

    if (!path.empty() && !demo) {
        if (!editor.loadFile(path)) {
            std::cerr << "could not load " << path << "\n";
            return 1;
        }
        std::cout << "loaded " << editor.lineCount() << " lines from " << path << "\n";
    }

    if (demo) {
        return runDemo(editor, path);
    }

    std::cout << "Text and Code Editor Engine — Zaheer Jafar F2024-0541\n";
    std::cout << "type help for commands\n";
    std::string line;
    while (std::cout << "> " && std::getline(std::cin, line)) {
        if (!handleCommand(editor, line)) {
            break;
        }
    }
    return 0;
}
