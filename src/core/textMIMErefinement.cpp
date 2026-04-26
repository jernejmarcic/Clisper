#include "textMIMErefinement.h"

#include <cctype>
#include <string>
#include <string_view>

namespace {

std::string_view trimLeadingWhitespace(std::string_view text) {
    size_t pos = 0;
    while (pos < text.size() && std::isspace(static_cast<unsigned char>(text[pos]))) {
        ++pos;
    }
    return text.substr(pos);
}

bool hasURISchemeAtStart(std::string_view text) {
    text = trimLeadingWhitespace(text);
    return text.starts_with("http://") ||
           text.starts_with("https://") ||
           text.starts_with("file://") ||
           text.starts_with("ftp://") ||
           text.starts_with("mailto:") ||
           text.starts_with("ssh://") ||
           text.starts_with("smb://");
}

}

std::string refineTextMIME(std::string_view rawBuff, std::string_view detectedMIME) {
    if (detectedMIME == "text/plain" && hasURISchemeAtStart(rawBuff)) {
        return "text/uri-list";
    }
    return std::string(detectedMIME);
}
