#include <iostream>
#include <magic.h>
#include <string>
#include <exiv2/exiv2.hpp>

std::string getMIME(std::string &rawBuff) {
    magic_t magicCookie = magic_open(MAGIC_MIME_TYPE | MAGIC_ERROR); // init libmagic for MIME output with error reporting
    if (magicCookie == NULL) {
        std::cerr << "Failed to initialize magic cookie" << std::endl; // bail if libmagic unavailable
        return "";
    }
    if (magic_load(magicCookie, nullptr) != 0) { // load default magic database
            magic_close(magicCookie);
            return "application/octet-stream"; // safe fallback on load failure
        }
    std::string mime = magic_buffer(magicCookie, rawBuff.c_str(), rawBuff.size()); // derive MIME from buffer contents
    magic_close(magicCookie); // release libmagic resources
    return mime;
}


int main() {
    std::ios::sync_with_stdio(false); // speed up iostreams by decoupling from stdio
    std::cin.tie(nullptr);            // avoid flushing stdout on each input operation

    std::string rawBuff((std::istreambuf_iterator<char>(std::cin)),
                        std::istreambuf_iterator<char>()); // grow to fit all stdin

    std::string mimeType = getMIME(rawBuff); // detect MIME type of captured input
    std::cout << rawBuff << std::endl;    // stream captured input to stdout
    std::cout << "MIME type: " << mimeType << std::endl;
    std::cout << "Length: " << rawBuff.size() << std::endl;
    return 0;
}
