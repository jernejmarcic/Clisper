#include "textMIMErefinement.h"

#include <cctype>
#include <string>
#include <string_view>

namespace {

char toLowerASCII(char ch) {
    if (ch >= 'A' && ch <= 'Z') {
        return static_cast<char>(ch - 'A' + 'a');
    }
    return ch;
}

std::string toLowerASCII(std::string_view text) {
    std::string lowered;
    lowered.reserve(text.size());
    for (char ch : text) {
        lowered.push_back(toLowerASCII(ch));
    }
    return lowered;
}

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
           text.starts_with("magnet:") ||
           text.starts_with("mailto:") ||
           text.starts_with("ssh://") ||
           text.starts_with("smb://");
}

bool isHTMLTagNameChar(char ch) {
    return std::isalnum(static_cast<unsigned char>(ch)) || ch == '-' || ch == ':';
}

bool isKnownHTMLTag(std::string_view tagName) {
    static constexpr std::string_view knownTags[] = {
        "a",      "article", "body",   "br",    "div",   "em",    "footer", "form",
        "h1",     "h2",      "h3",     "h4",    "h5",    "h6",    "head",   "header",
        "html",   "img",     "input",  "label", "li",    "link",  "main",   "meta",
        "ol",     "p",       "script", "section", "span", "strong", "style",  "title",
        "table",  "tbody",   "td",     "th",    "thead", "tr",    "ul"
    };

    for (std::string_view knownTag : knownTags) {
        if (tagName == knownTag) {
            return true;
        }
    }
    return false;
}

bool looksLikeHTMLAtStart(std::string_view text) {
    text = trimLeadingWhitespace(text);
    if (text.empty()) {
        return false;
    }

    size_t prefixSize = text.size() < 32 ? text.size() : 32;
    std::string loweredPrefix = toLowerASCII(text.substr(0, prefixSize));
    if (loweredPrefix.starts_with("<!doctype html") || loweredPrefix.starts_with("<?xml")) {
        return true;
    }

    if (text[0] != '<') {
        return false;
    }

    size_t pos = 1;
    if (pos < text.size() && text[pos] == '/') {
        ++pos;
    }
    if (pos >= text.size() || !std::isalpha(static_cast<unsigned char>(text[pos]))) {
        return false;
    }

    size_t tagStart = pos;
    while (pos < text.size() && isHTMLTagNameChar(text[pos])) {
        ++pos;
    }

    std::string tagName = toLowerASCII(text.substr(tagStart, pos - tagStart));
    if (!isKnownHTMLTag(tagName)) {
        return false;
    }

    size_t tagEnd = text.find('>', pos);
    if (tagEnd == std::string_view::npos) {
        return false;
    }

    std::string_view firstTag = text.substr(0, tagEnd + 1);
    return firstTag.find("=\"") != std::string_view::npos ||
           firstTag.find("='") != std::string_view::npos ||
           firstTag.find(' ') != std::string_view::npos ||
           text.find('<', tagEnd + 1) != std::string_view::npos;
}

}

std::string refineTextMIME(std::string_view rawBuff, std::string_view detectedMIME) {
    if (detectedMIME == "text/plain") {
        if (hasURISchemeAtStart(rawBuff)) {
            return "text/uri-list";
        }
        if (looksLikeHTMLAtStart(rawBuff)) {
            return "text/html";
        }
    }
    return std::string(detectedMIME);
}
