# Clipser (C++ clipboard history utility)

Personal clipboard history manager with advanced features, such as language detection for text, image metadata extraction, and more.

## Dependencies

Required:
- g++ (C++17), make, and pkg-config
- libmagic (file type detection)
- exiv2 (image metadata extraction)
- libexttextcat (language detection)
- sqlite3 (core database storage)

### Installing Dependencies

#### Ubuntu/Debian

```shell
sudo apt-get update
sudo apt-get install build-essential pkg-config libmagic-dev libexiv2-dev libexttextcat-dev
sudo apt-get install sqlite3 libsqlite3-dev
```

#### Arch Linux

```shell
sudo pacman -S --needed base-devel pkgconf file exiv2 libexttextcat
sudo pacman -S --needed sqlite
```

#### Fedora

```shell
sudo dnf install gcc-c++ make pkgconfig file-devel exiv2-devel libexttextcat-devel
sudo dnf install sqlite-devel
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
