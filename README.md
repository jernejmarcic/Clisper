# Clipboard History Coursework

## Business Rule
The clipboard history service receives each paste event, inspects its MIME type, and processes it accordingly. Text is decoded using the declared format, assigned titles manually or by getting the highest heading available, and stored with timestamps. If no title exists the field is left NULL, the programm also gets and stores language. Image payloads automatically undergo metadata extraction, including EXIF GPS, camera, and software tags, and a full blobs is stored. OCR may be run on the images for searching for images containing text.

## Tasks
- [ ] Define schema for table `entry`
- [x] Implement image/text detection and format tagging
- [x] Extract EXIF/metadata for images where available
- [ ] Persist OCR text when images contain text
- [ ] Track created/updated/accessed timestamps on each entry
- [ ] Record source application (when known) -- Too hard
- [ ] Generate charts for usage by kind, language, source, recency
- [ ] Implement cleanup policy with deletion log
- [ ] Package deliverables per coursework brief (CW.py, CW.pdf, CSVs, CW.zip)
