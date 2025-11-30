#include <iostream>
#include <magic.h>
#include <sstream>
#include <string>
#include <exiv2/exiv2.hpp>

enum class imageMime {
    png,
    jpeg,
    jpg,
    gif,
    bmp,
    tiff,
    webp,
    svg,
    ico,
    unknown
};

inline imageMime imageMimeFromString(const std::string& mime) {
    if (mime == "image/png") return imageMime::png;
    if (mime == "image/jpeg") return imageMime::jpeg;
    if (mime == "image/jpg") return imageMime::jpg;
    if (mime == "image/gif") return imageMime::gif;
    if (mime == "image/bmp") return imageMime::bmp;
    if (mime == "image/tiff") return imageMime::tiff;
    if (mime == "image/webp") return imageMime::webp;
    if (mime == "image/svg+xml") return imageMime::svg;
    if (mime == "image/x-icon" || mime == "image/vnd.microsoft.icon") return imageMime::ico;
    if (mime.rfind("image/", 0) == 0) return imageMime::unknown; // treat other image/* as image
    return imageMime::unknown;
}

bool isImageMime(const std::string& mime) { // broad check for any image/*
    return mime.rfind("image/", 0) == 0;
}


std::string getMIME(const std::string &rawBuff) {
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

std::string extractImageMetadata(const std::string& mime, const std::string& rawBuff) {
    if (!isImageMime(mime)) {
        return "";
    }
    Exiv2::Image::UniquePtr image = Exiv2::ImageFactory::open(reinterpret_cast<const Exiv2::byte*>(rawBuff.data()),
                                                             rawBuff.size()); // open from in-memory buffer
    if (!image) {
        return "";
    }
    image->readMetadata(); // Loads image var into readMetadata
    Exiv2::ExifData& exifData = image->exifData();

    auto tagValue = [&exifData](const char* key) -> std::string {
        Exiv2::ExifData::const_iterator it = exifData.findKey(Exiv2::ExifKey(key));
        if (it == exifData.end()) return "";
        return it->value().toString();
    };

    std::ostringstream out;

    std::string description = tagValue("Exif.Image.ImageDescription");
    if (!description.empty()) out << "Description: " << description << "\n";

    std::string make = tagValue("Exif.Image.Make");
    if (!make.empty()) out << "Make: " << make << "\n";

    std::string model = tagValue("Exif.Image.Model");
    if (!model.empty()) out << "Model: " << model << "\n";

    // Resolution: prefer PixelX/YDimension if present, fallback to ImageWidth/ImageLength
    std::string width = tagValue("Exif.Photo.PixelXDimension");
    std::string height = tagValue("Exif.Photo.PixelYDimension");
    if (width.empty() || height.empty()) {
        width = tagValue("Exif.Image.ImageWidth");
        height = tagValue("Exif.Image.ImageLength");
    }
    if (!width.empty() && !height.empty()) out << "Resolution: " << width << "x" << height << "\n";

    std::string dateTaken = tagValue("Exif.Photo.DateTimeOriginal");
    if (dateTaken.empty()) dateTaken = tagValue("Exif.Image.DateTime");
    if (!dateTaken.empty()) out << "DateTaken: " << dateTaken << "\n";

    std::string latRef = tagValue("Exif.GPSInfo.GPSLatitudeRef");
    std::string lat = tagValue("Exif.GPSInfo.GPSLatitude");
    std::string lonRef = tagValue("Exif.GPSInfo.GPSLongitudeRef");
    std::string lon = tagValue("Exif.GPSInfo.GPSLongitude");
    std::string alt = tagValue("Exif.GPSInfo.GPSAltitude");
    if (!lat.empty() && !lon.empty()) {
        out << "GPS: " << latRef << " " << lat << ", " << lonRef << " " << lon;
        if (!alt.empty()) out << ", Altitude: " << alt;
        out << "\n";
    }

    return out.str();
}

int main() {
    std::ios::sync_with_stdio(false); // speed up iostreams by decoupling from stdio
    std::cin.tie(nullptr);            // avoid flushing stdout on each input operation

    std::string rawBuff((std::istreambuf_iterator<char>(std::cin)),
                        std::istreambuf_iterator<char>()); // grow to fit all stdin

    std::string mimeType = getMIME(rawBuff); // detect MIME type of captured input
    if (!isImageMime(mimeType)) { // broad check for any image/*
        std::cout << rawBuff << std::endl;    // stream captured input to stdout
    } else if(isImageMime(mimeType)) { // broad check for any image/* will po
        std::cout << extractImageMetadata(mimeType, rawBuff) << std::endl;
    }
    std::cout << "MIME type: " << mimeType << std::endl;
    std::cout << "Length: " << rawBuff.size() << std::endl;
    return 0;
}
