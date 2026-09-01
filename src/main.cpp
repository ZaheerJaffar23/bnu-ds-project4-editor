#include "Editor.hpp"
#include <iostream>
int main() {
    Editor editor;
    editor.insertLine("alpha");
    editor.insertLine("bravo");
    std::cout << "lines=" << editor.lineCount() << "\n";
    return 0;
}
