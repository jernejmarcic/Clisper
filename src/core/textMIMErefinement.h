#ifndef TEXT_MIME_REFINEMENT_H
#define TEXT_MIME_REFINEMENT_H

#include <string>
#include <string_view>

std::string refineTextMIME(std::string_view rawBuff, std::string_view detectedMIME);

#endif
