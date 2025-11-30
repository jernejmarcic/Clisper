#include <iostream>

int main() {
    std::ios::sync_with_stdio(false); // speed up iostreams by decoupling from stdio
    std::cin.tie(nullptr);            // avoid flushing stdout on each input operation

    std::cout << std::cin.rdbuf();    // stream stdin directly to stdout
    return 0;
}
