# Clipboard History Coursework

## Business Rule
Every clipboard paste becomes one Entry (auto PK) storing MIME type, created-at timestamp, and the raw payload (raw text for text, raw blob for everything else); MIME is captured for every row (PC total, MC 1). Payload participation is total (exactly one of text content or blob content is present, MC 1). Title is auto-generated when possible, else NULL (PC partial, MC 0..1). Text payloads always store the detected language code (e.g., EN); confidence is not stored. Image MIME values trigger best-effort metadata extraction; missing fields stay NULL (PC partial, MC 0..1). Optional fields stay NULL when unavailable. No other entities are modeled; PC/MC outside Entry are not applicable.

## Tasks
- [ ] Define schema for table `entry`
- [x] Implement image/text detection and format tagging
- [x] Extract EXIF/metadata for images where available
- [ ] Track created/updated/accessed timestamps on each entry
- [ ] Record source application (when known) -- Too hard
- [ ] Generate charts for usage by MIME, language, recency
- [ ] Implement cleanup policy with deletion log
- [ ] Package deliverables per coursework brief (CW.py, CW.pdf, CSVs, CW.zip)
