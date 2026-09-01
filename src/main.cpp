#include "Engine.hpp"
#include <iostream>
int main() {
    Engine editor;
    editor.insertLine("alpha");
    editor.insertLine("bravo");
    editor.undo();
    std::cout << "lines after undo: " << editor.lineCount() << "\n";
    return 0;
}
