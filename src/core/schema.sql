CREATE TABLE clisper (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    title TEXT NULL,
    language TEXT NULL,
    mimeType TEXT NULL,
    entry BLOB NOT NULL UNIQUE,
    createdAt INTEGER NOT NULL DEFAULT (strftime('%s','now')),
    ocrText TEXT,
    imageMetadata TEXT NULL,
    isFavourite BOOLEAN NOT NULL DEFAULT 0,
    importance INTEGER NOT NULL DEFAULT 9,
    updatedAt INTEGER NOT NULL DEFAULT (strftime('%s','now')),
    accessedAt INTEGER
);
