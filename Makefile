CXX ?= g++
CXXFLAGS ?= -std=c++23 -O2 -Wall -Wextra -Wpedantic -pipe
PKG_CONFIG ?= pkg-config
VERSION ?= 0.7.1
GTK_PKG ?= gtkmm-4.0

MAGIC_CFLAGS := $(shell $(PKG_CONFIG) --cflags libmagic 2>/dev/null)
MAGIC_LIBS   := $(shell $(PKG_CONFIG) --libs libmagic 2>/dev/null)
EXIV2_CFLAGS := $(shell $(PKG_CONFIG) --cflags exiv2 2>/dev/null)
EXIV2_LIBS   := $(shell $(PKG_CONFIG) --libs exiv2 2>/dev/null)
TEXTCAT_CFLAGS := $(shell $(PKG_CONFIG) --cflags libexttextcat 2>/dev/null)
TEXTCAT_LIBS   := $(shell $(PKG_CONFIG) --libs libexttextcat 2>/dev/null)
SQLITE3_LIBS   := $(shell $(PKG_CONFIG) --libs sqlite3 2>/dev/null)
SQLITE3_CFLAGS := $(shell $(PKG_CONFIG) --cflags sqlite3 2>/dev/null)
GTK_CFLAGS := $(shell $(PKG_CONFIG) --cflags $(GTK_PKG) 2>/dev/null)
GTK_LIBS := $(shell $(PKG_CONFIG) --libs $(GTK_PKG) 2>/dev/null)
ifeq ($(strip $(SQLITE3_LIBS)),)
SQLITE3_LIBS := -lsqlite3
endif

CPPFLAGS += $(MAGIC_CFLAGS) $(EXIV2_CFLAGS) $(TEXTCAT_CFLAGS) $(SQLITE3_CFLAGS) -DAPP_VERSION=\"$(VERSION)\"
LDLIBS += $(MAGIC_LIBS) $(EXIV2_LIBS) $(TEXTCAT_LIBS) $(SQLITE3_LIBS) -pthread

BUILD_DIR := build
BINARY := $(BUILD_DIR)/main
DMENU_BIN := $(BUILD_DIR)/dmenuCompatibility
DBINIT_BIN := $(BUILD_DIR)/dbInit
GTK_BIN := $(BUILD_DIR)/clisperGTK

MAIN_SRC := src/core/main.cpp
MAIN_OBJ := $(BUILD_DIR)/core/main.o

.PHONY: all clean database

all: $(BINARY) $(DMENU_BIN)

$(BINARY): $(MAIN_OBJ)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(BUILD_DIR)/core/main.o: $(MAIN_SRC)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

$(DMENU_BIN): src/compat/dmenuCompatibility.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) src/compat/dmenuCompatibility.cpp -o $@ $(LDLIBS)

$(DBINIT_BIN): src/core/dbInit.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) src/core/dbInit.cpp -o $@ $(LDLIBS)

$(GTK_BIN): src/gui/main.cpp src/gui/clisperGTK.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(GTK_CFLAGS) $^ -o $@ $(GTK_LIBS)

gtk: $(GTK_BIN)



test: $(BINARY)
	make all
	@echo "Where lies the strangling fruit that came from the hand of the sinner I shall bring forth the seeds of the dead to share with the worms that gather in the darkness and surround the world with the power of their lives while from the dimlit halls of other places forms that never were and never could be writhe for the impatience of the few who never saw what could have been. In the black water with the sun shining at midnight, those fruit shall come ripe and in the darkness of that which is golden shall split open to reveal the revelation of the fatal softness in the earth. The shadows of the abyss are like the petals of a monstrous flower that shall blossom within the skull and expand the mind beyond what any man can bear, but whether it decays under the earth or above on green fields, or out to sea or in the very air, all shall come to revelation, and to revel, in the knowledge of the strangling fruit—and the hand of the sinner shall rejoice, for there is no sin in shadow or in light that the seeds of the dead cannot forgive. And there shall be in the planting in the shadows a grace and a mercy from which shall blossom dark flowers, and their teeth shall devour and sustain and herald the passing of an age. That which dies shall still know life in death for all that decays is not forgotten and reanimated it shall walk the world in the bliss of not-knowing. And then there shall be a fire that knows the naming of you, and in the presence of the strangling fruit, its dark flame shall acquire every part of you that remains." | $(BINARY)

clean:
	$(RM) -r $(BUILD_DIR) clipser.db
	@mkdir -p $(dir $(DBINIT_BIN))
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) src/core/dbInit.cpp -o $(DBINIT_BIN) $(LDLIBS)
	$(DBINIT_BIN)

database: $(DBINIT_BIN)
	$(DBINIT_BIN)
