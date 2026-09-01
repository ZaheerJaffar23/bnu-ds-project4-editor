#ifndef EDITOR_BRACKET_CHECKER_HPP
#define EDITOR_BRACKET_CHECKER_HPP

#include "DoublyLinkedList.hpp"
#include "Stack.hpp"

#include <string>
#include <vector>

struct BracketIssue {
    std::size_t line = 0;    // 1-based
    std::size_t column = 0;  // 1-based
    char found = 0;
    char expected = 0;
    std::string message;
};

struct BracketReport {
    bool balanced = true;
    std::vector<BracketIssue> issues;
};

struct BracketOpen {
    char ch = 0;
    std::size_t line = 0;
    std::size_t column = 0;
};

inline char matchingOpen(char close) {
    switch (close) {
        case ')': return '(';
        case ']': return '[';
        case '}': return '{';
        default: return 0;
    }
}

inline char matchingClose(char open) {
    switch (open) {
        case '(': return ')';
        case '[': return ']';
        case '{': return '}';
        default: return 0;
    }
}

inline bool isOpen(char c) { return c == '(' || c == '[' || c == '{'; }
inline bool isClose(char c) { return c == ')' || c == ']' || c == '}'; }

// Single-pass scan. On a type mismatch, the opener still on the stack lost
// its matching closer — report that opener's line/column, not the later closer.
inline BracketReport checkBrackets(const DoublyLinkedList<std::string>& lines) {
    BracketReport report;
    Stack<BracketOpen> open;
    std::size_t lineNo = 1;
    for (auto* node = lines.head(); node; node = node->next, ++lineNo) {
        const std::string& text = node->data;
        for (std::size_t col = 0; col < text.size(); ++col) {
            const char c = text[col];
            if (isOpen(c)) {
                open.push({c, lineNo, col + 1});
            } else if (isClose(c)) {
                if (open.empty()) {
                    report.balanced = false;
                    report.issues.push_back(
                        {lineNo, col + 1, c, matchingOpen(c), "closing bracket with no opener"});
                    return report;
                }
                const BracketOpen& top = open.top();
                if (top.ch != matchingOpen(c)) {
                    report.balanced = false;
                    report.issues.push_back({top.line, top.column, top.ch, matchingClose(top.ch),
                                             "unclosed opening bracket"});
                    return report;
                }
                open.pop();
            }
        }
    }
    if (!open.empty()) {
        BracketOpen leftover = open.top();
        report.balanced = false;
        report.issues.push_back({leftover.line, leftover.column, leftover.ch, matchingClose(leftover.ch),
                                 "unclosed opening bracket"});
    }
    return report;
}

#endif
