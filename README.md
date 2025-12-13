# Clipboard History Coursework (C++)

Command-line utility that ingests clipboard payloads from stdin, tags them by MIME type, and emits lightweight metadata. Text payloads get a best-effort language guess; images get selected EXIF/metadata fields when present. This is the ingestion probe for the coursework’s `entry` table.

## What It Does
- Reads raw stdin into memory.
- Detects MIME using `libmagic`.
- If text: echoes the payload, detects language via `libexttextcat`, prints MIME, payload length, and a Unix timestamp.
- If image: extracts EXIF fields (description, make/model, resolution, date taken, GPS) via `exiv2`, then prints MIME, payload length, and timestamp.

## Build (Arch)
Prereqs: `g++`, `make`, `pkgconf`, `libmagic`, `exiv2`, `libexttextcat`.
- Install deps: `sudo pacman -S gcc make pkgconf file exiv2 libexttextcat`
- Build: `make` (or `make clean && make` to force a rebuild)
- Output binary: `./main`

The Makefile uses `pkg-config` to pull the include/lib flags and only builds `main.cpp`.


Wayland clipboard watcher (requires `wl-clipboard`):
```
wl-paste --watch ./main
```
