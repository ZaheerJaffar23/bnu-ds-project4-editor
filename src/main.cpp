#include "DoublyLinkedList.hpp"
#include <iostream>
#include <string>
int main() {
    DoublyLinkedList<std::string> lines;
    lines.pushBack("first line");
    lines.pushBack("second line");
    std::cout << "DLL size: " << lines.size() << "\n";
    return 0;
}
