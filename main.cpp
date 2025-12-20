#include <chrono>
#include <exiv2/exiv2.hpp>
#include <iostream>
#include <iterator>
#include <libexttextcat/textcat.h>
#include <magic.h>
#include <sqlite3.h>
#include <optional>
#include <string>

#ifndef APP_VERSION
#define APP_VERSION "dev"
#endif

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

struct ImageMetadata {
    std::optional<std::string> description;
    std::optional<std::string> make;
    std::optional<std::string> model;
    std::optional<std::string> resolutionWidth;
    std::optional<std::string> resolutionHeight;
    std::optional<std::string> dateTaken;
    std::optional<std::string> gpsLatRef;
    std::optional<std::string> gpsLat;
    std::optional<std::string> gpsLonRef;
    std::optional<std::string> gpsLon;
    std::optional<std::string> gpsAlt;
};


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

ImageMetadata extractImageMetadata(const std::string& mime, const std::string& rawBuff) {
    ImageMetadata meta;
    if (!isImageMime(mime)) {
        return meta;
    }
    Exiv2::Image::UniquePtr image = Exiv2::ImageFactory::open(reinterpret_cast<const Exiv2::byte*>(rawBuff.data()),
                                                             rawBuff.size()); // open from in-memory buffer
    if (!image) {
        return meta;
    }
    image->readMetadata(); // Loads image var into readMetadata
    Exiv2::ExifData& exifData = image->exifData();

    auto tagValue = [&exifData](const char* key) -> std::string {
        Exiv2::ExifData::const_iterator it = exifData.findKey(Exiv2::ExifKey(key));
        if (it == exifData.end()) return "";
        return it->value().toString();
    };

    std::string description = tagValue("Exif.Image.ImageDescription");
    if (!description.empty()) meta.description = description;

    std::string make = tagValue("Exif.Image.Make");
    if (!make.empty()) meta.make = make;

    std::string model = tagValue("Exif.Image.Model");
    if (!model.empty()) meta.model = model;

    // Resolution: prefer PixelX/YDimension if present, fallback to ImageWidth/ImageLength
    std::string width = tagValue("Exif.Photo.PixelXDimension");
    std::string height = tagValue("Exif.Photo.PixelYDimension");
    if (width.empty() || height.empty()) {
        width = tagValue("Exif.Image.ImageWidth");
        height = tagValue("Exif.Image.ImageLength");
    }
    if (!width.empty()) meta.resolutionWidth = width;
    if (!height.empty()) meta.resolutionHeight = height;

    std::string dateTaken = tagValue("Exif.Photo.DateTimeOriginal");
    if (dateTaken.empty()) dateTaken = tagValue("Exif.Image.DateTime");
    if (!dateTaken.empty()) meta.dateTaken = dateTaken;

    std::string latRef = tagValue("Exif.GPSInfo.GPSLatitudeRef");
    std::string lat = tagValue("Exif.GPSInfo.GPSLatitude");
    std::string lonRef = tagValue("Exif.GPSInfo.GPSLongitudeRef");
    std::string lon = tagValue("Exif.GPSInfo.GPSLongitude");
    std::string alt = tagValue("Exif.GPSInfo.GPSAltitude");
    if (!latRef.empty()) meta.gpsLatRef = latRef;
    if (!lat.empty()) meta.gpsLat = lat;
    if (!lonRef.empty()) meta.gpsLonRef = lonRef;
    if (!lon.empty()) meta.gpsLon = lon;
    if (!alt.empty()) meta.gpsAlt = alt;

    return meta;
}

std::string stripEncodingSuffix(const std::string& rawLangResult) {
    std::string cleaned = rawLangResult;
    size_t pos = 0;
    while ((pos = cleaned.find("--", pos)) != std::string::npos) {
        size_t end = cleaned.find(']', pos);
        if (end == std::string::npos) break;
        cleaned.erase(pos, end - pos); // drop encoding marker up to closing bracket
    }
    return cleaned;
}


int main(int argc, char** argv) {
    std::ios::sync_with_stdio(false); // speed up iostreams by decoupling from stdio
    std::cin.tie(nullptr);            // avoid flushing stdout on each input operation

    if (argc > 1) { // lightweight version flag
        std::string arg1 = argv[1];
        if (arg1 == "--version" || arg1 == "-V" || arg1 == "-v") {
            std::cout << "Clisper " << APP_VERSION << std::endl;
            return 0;
        }
    }
    sqlite3* DB;
    int exit = 0;
    char* messaggeError;
    exit = sqlite3_open("clipser.db", &DB);

    std::string rawBuff((std::istreambuf_iterator<char>(std::cin)),
                        std::istreambuf_iterator<char>()); // grow to fit all stdin
    const auto p1 = std::chrono::system_clock::now();
    int unixTime = std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count();

    std::string insertIntoDB;
    std::string detectedLanguage;
    std::string title;


    std::string mimeType = getMIME(rawBuff); // detect MIME type of captured input


    if (!isImageMime(mimeType)) { // non-image: echo and detect language
        std::cout << rawBuff << std::endl;    // stream captured input to stdout

        static const char* textcatConfig = "/usr/share/libexttextcat/fpdb.conf";
        static const char* textcatPrefix = "/usr/share/libexttextcat/";
        void* detector = special_textcat_Init(textcatConfig, textcatPrefix);
        if (detector) {
            char* lang = textcat_Classify(detector, rawBuff.c_str(), rawBuff.size());
            std::string langOut = lang ? std::string(lang) : "UNKNOWN"; // copy before cleanup
            std::cout << "Language: " << stripEncodingSuffix(langOut) << std::endl;
            detectedLanguage.append(langOut);

            textcat_Done(detector);
        } else {
            std::cout << "Language: UNKNOWN (detector init failed)" << std::endl;
            detectedLanguage.append("UNKNOWN");
        }
    } else { // image: dump selected EXIF data
        ImageMetadata meta = extractImageMetadata(mimeType, rawBuff);
        if (meta.description) {
            std::cout << "Description: " << *meta.description << "\n";
        }
        if (meta.make) {
            std::cout << "Make: " << *meta.make << "\n";
        }
        if (meta.model) {
            std::cout << "Model: " << *meta.model << "\n";
        }
        if (meta.resolutionWidth && meta.resolutionHeight) {
            std::cout << "Resolution: " << *meta.resolutionWidth << "x" << *meta.resolutionHeight << "\n";
        }
        if (meta.dateTaken) {
            std::cout << "DateTaken: " << *meta.dateTaken << "\n";
        }
        if (meta.gpsLat && meta.gpsLon) {
            std::cout << "GPS: ";
            if (meta.gpsLatRef) {
                std::cout << *meta.gpsLatRef << " ";
            }
            std::cout << *meta.gpsLat << ", ";
            if (meta.gpsLonRef) {
                std::cout << *meta.gpsLonRef << " ";
            }
            std::cout << *meta.gpsLon;
            if (meta.gpsAlt) {
                std::cout << ", Altitude: " << *meta.gpsAlt;
            }
            std::cout << "\n";
        }
    }
    std::cout << "MIME type: " << mimeType << std::endl;
    std::cout << "Length: " << rawBuff.size() << std::endl;
    std::cout << "Unix Time: " << unixTime << std::endl;
    // std::cout << "TEXT: " << rawBuff << std::endl;

    // insertIntoDB.append("INSERT INTO clisper (title,language,mimeType,entry) VALUES('");
    // insertIntoDB.append("title");
    // insertIntoDB.append("','");
    // insertIntoDB.append(detectedLanguage);
    // insertIntoDB.append("','");
    // insertIntoDB.append(mimeType);
    // insertIntoDB.append("','");
    // insertIntoDB.append(rawBuff);


    // insertIntoDB.append("');");

    // std::cout << insertIntoDB << std::endl;
    // exit = sqlite3_exec(DB, insertIntoDB.c_str(), NULL, 0, &messaggeError);



    const bool isImage = isImageMime(mimeType);
    const char* insertSqlImg = R"sql(
        INSERT INTO clisper (
            title, language, mimeType, entry, imageMetadata, accessedAt
        ) VALUES (?1, ?2, ?3, ?4, ?5, ?6);
    )sql";
    sqlite3_stmt* stmtImg = nullptr;
    if (sqlite3_prepare_v2(DB, insertSqlImg, -1, &stmtImg, nullptr) == SQLITE_OK) {
        auto bindTextOrNull = [&](int idx, const std::string& val) {
            if (val.empty()) sqlite3_bind_null(stmtImg, idx);
            else sqlite3_bind_text(stmtImg, idx, val.c_str(), -1, SQLITE_TRANSIENT);
        };
        bindTextOrNull(1, title);
        bindTextOrNull(2, isImage ? "" : detectedLanguage);
        bindTextOrNull(3, mimeType);
        if (!rawBuff.empty()) sqlite3_bind_blob(stmtImg, 4, rawBuff.data(), static_cast<int>(rawBuff.size()), SQLITE_TRANSIENT);
        else sqlite3_bind_null(stmtImg, 4);
        std::string imageMetadataJson; // fill if you serialize EXIF; leave empty for NULL
        bindTextOrNull(5, imageMetadataJson);
        sqlite3_bind_int64(stmtImg, 6, unixTime); // or bind_null to keep accessedAt NULL
        if (sqlite3_step(stmtImg) != SQLITE_DONE) {
            std::cerr << "Image insert failed: " << sqlite3_errmsg(DB) << std::endl;
        }
        sqlite3_finalize(stmtImg);
    } else {
        std::cerr << "Image prepare failed: " << sqlite3_errmsg(DB) << std::endl;
    }


    std::string sql;
    // std::string query = "SELECT * FROM clisper;";

    if (exit != SQLITE_OK) {
           std::cerr << "Error Insert" << std::endl;
           sqlite3_free(messaggeError);
       } else
           std::cout << "Records created Successfully!" << std::endl;




    sqlite3_close(DB);

    return 0;
}
