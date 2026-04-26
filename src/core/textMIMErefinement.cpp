#include "textMIMErefinement.h"

#include <string>
#include <string_view>

std::string refineTextMIME(std::string_view rawBuff, std::string_view detectedMIME) {
    (void)rawBuff;
    return std::string(detectedMIME);
}
