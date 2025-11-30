#include <iostream>
#include <magic.h>
#include <string>
#include <exiv2/exiv2.hpp>

std::string getMIME(std::string &rawBuff) {
    magic_t magicCokie = magic_open(MAGIC_MIME_TYPE | MAGIC_ERROR); // Needs it I dont understand why
    if (magicCokie == NULL) {
        std::cerr << "Failed to initialize magic cookie" << std::endl;
        return NULL;
    }
    std::string mime = magic_buffer(magicCokie, rawBuff.c_str(), rawBuff.size());
    magic_close(magicCokie);
    return mime;
}


int main() {
    std::ios::sync_with_stdio(false); // speed up iostreams by decoupling from stdio
    std::cin.tie(nullptr);            // avoid flushing stdout on each input operation

    std::string rawBuff((std::istreambuf_iterator<char>(std::cin)),
                        std::istreambuf_iterator<char>()); // grow to fit all stdin

    std::string mimeType = getMIME(rawBuff);
    std::cout << rawBuff << std::endl;    // stream captured input to stdout
    std::cout << "MIME type: " << mimeType << std::endl;
    std::cout << "Length: " << rawBuff.size() << std::endl;
    return 0;
}
