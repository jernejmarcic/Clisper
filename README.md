# Clipser (C++ clipboard history utility)

Clipboard history keeper/scratchpad with language detection for text, image metadata extraction, and OCR.
This is meant to be a clipboard history keeper and scratchpad.

## Table of Contents
- [Why I built it](#why-i-built-it)
- [Dependencies](#dependencies)
  - [Installing Dependencies](#installing-dependencies)
    - [Ubuntu/Debian](#ubuntudebian)
    - [Arch Linux](#arch-linux)
    - [Fedora](#fedora)
- [Build Instructions](#build-instructions)
- [Running](#running)
- [Auto-start (Hyprland)](#auto-start-hyprland)
- [Auto-start (Sway)](#auto-start-sway)
- [Roadmap](#roadmap)
  - [1.0.0](#100)
  - [2.0.0](#200)
- [Versions](#versions)

## Why I built it
I noticed I was using my clipboard history for more than just keeping clipboard history. I found myself searching through it for some information I frequently had to copy and paste such as email addresses, phone numbers, addresses, and more.
So I built Clipser to make it easier to access this information.

## Dependencies

Required (core CLI + database):
- g++ (C++23), make, and pkg-config
- sqlite3 (core database storage)
- libmagic (file type detection)
- exiv2 (image metadata extraction)
- libexttextcat (language detection)
- tesseract + leptonica (OCR headers used by current core build)

Optional (build only if you need these targets):
- gtkmm-4.0 (GTK UI in `src/gui`)

### Installing Dependencies

#### Ubuntu/Debian

```shell
sudo apt-get update
sudo apt-get install build-essential pkg-config libmagic-dev libexiv2-dev libexttextcat-dev libsqlite3-dev libtesseract-dev libleptonica-dev
sudo apt-get install sqlite3
```

Optional (GTK UI):

```shell
sudo apt-get install libgtkmm-4.0-dev
```

#### Arch Linux

```shell
sudo pacman -S --needed base-devel pkgconf file exiv2 libexttextcat sqlite tesseract leptonica
```

Optional (GTK UI):

```shell
sudo pacman -S --needed gtkmm-4.0
```

#### Fedora

```shell
sudo dnf install gcc-c++ make pkgconfig file-devel exiv2-devel libexttextcat-devel sqlite-devel tesseract-devel leptonica-devel
```

Optional (GTK UI):

```shell
sudo dnf install gtkmm4.0-devel
```

## Build Instructions
Open a terminal and navigate to the project directory. Then, run the `make all` command.
Or copy paste the following that will do just that:

```shell
git clone https://github.com/jernejmarcic/Clisper
cd Clisper
make all
```

Maybe I will add precompiled binaries so you don't have to compile it yourself and install all the dependancies.

## Running
You need two separate instances so both text and images are captured.

```shell
wl-paste --type image --watch /path/to/main
wl-paste --type text --watch /path/to/main
```

## Auto-start (Hyprland)
Add the two commands to your Hyprland config

```ini
exec-once = wl-paste --type image --watch /path/to/main
exec-once = wl-paste --type text --watch /path/to/main
```

## Auto-start (Sway)
Add the two commands to your Sway config

```ini
exec wl-paste --type image --watch /path/to/main
exec wl-paste --type text --watch /path/to/main
```

## Roadmap
- [ ] Actually finish making it

### 1.0.0
**FULL RELEASE**
- [ ] Configurable database
- [ ] Configurable metadata extraction 
- [ ] Configurable post-processing
- [ ] Image OCR via tesseract


- [ ] Custom GTK app

### 2.0.0

- [ ] Custom cross platform copy/paste utility 


## Versions
<!--### 0.7.x
- [x] DB works
- [x] Integration for dmenu compatible interfaces-->
