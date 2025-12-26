CXX ?= g++
CXXFLAGS ?= -std=c++23 -O2 -Wall -Wextra -Wpedantic -pipe
PKG_CONFIG ?= pkg-config
VERSION ?= 0.5.1

MAGIC_CFLAGS := $(shell $(PKG_CONFIG) --cflags libmagic 2>/dev/null)
MAGIC_LIBS   := $(shell $(PKG_CONFIG) --libs libmagic 2>/dev/null)
EXIV2_CFLAGS := $(shell $(PKG_CONFIG) --cflags exiv2 2>/dev/null)
EXIV2_LIBS   := $(shell $(PKG_CONFIG) --libs exiv2 2>/dev/null)
TEXTCAT_CFLAGS := $(shell $(PKG_CONFIG) --cflags libexttextcat 2>/dev/null)
TEXTCAT_LIBS   := $(shell $(PKG_CONFIG) --libs libexttextcat 2>/dev/null)
SQLITE3_LIBS   := $(shell $(PKG_CONFIG) --libs sqlite3 2>/dev/null)
SQLITE3_CFLAGS := $(shell $(PKG_CONFIG) --cflags sqlite3 2>/dev/null)
ifeq ($(strip $(SQLITE3_LIBS)),)
SQLITE3_LIBS := -lsqlite3
endif

CPPFLAGS += $(MAGIC_CFLAGS) $(EXIV2_CFLAGS) $(TEXTCAT_CFLAGS) $(SQLITE3_CFLAGS) -DAPP_VERSION=\"$(VERSION)\"
LDLIBS += $(MAGIC_LIBS) $(EXIV2_LIBS) $(TEXTCAT_LIBS) $(SQLITE3_LIBS) -pthread

BUILD_DIR := build
BINARY := main
DMENU_BIN := dmenuCompatibility
DBINIT_BIN := dbInit

MAIN_OBJ := $(BUILD_DIR)/main.o

.PHONY: all clean database

all: $(BINARY) $(DMENU_BIN)

$(BINARY): $(MAIN_OBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(BUILD_DIR)/main.o: main.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

$(DMENU_BIN): dmenuCompatibility.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) dmenuCompatibility.cpp -o $@ $(LDLIBS)

$(DBINIT_BIN): dbInit.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) dbInit.cpp -o $@ $(LDLIBS)



test: $(BINARY)
	make all
	@echo "Where lies the strangling fruit that came from the hand of the sinner I shall bring forth the seeds of the dead to share with the worms that gather in the darkness and surround the world with the power of their lives while from the dimlit halls of other places forms that never were and never could be writhe for the impatience of the few who never saw what could have been. In the black water with the sun shining at midnight, those fruit shall come ripe and in the darkness of that which is golden shall split open to reveal the revelation of the fatal softness in the earth. The shadows of the abyss are like the petals of a monstrous flower that shall blossom within the skull and expand the mind beyond what any man can bear, but whether it decays under the earth or above on green fields, or out to sea or in the very air, all shall come to revelation, and to revel, in the knowledge of the strangling fruit—and the hand of the sinner shall rejoice, for there is no sin in shadow or in light that the seeds of the dead cannot forgive. And there shall be in the planting in the shadows a grace and a mercy from which shall blossom dark flowers, and their teeth shall devour and sustain and herald the passing of an age. That which dies shall still know life in death for all that decays is not forgotten and reanimated it shall walk the world in the bliss of not-knowing. And then there shall be a fire that knows the naming of you, and in the presence of the strangling fruit, its dark flame shall acquire every part of you that remains." | ./main

clean:
	$(RM) -r $(BUILD_DIR) $(BINARY) $(DMENU_BIN) $(DBINIT_BIN) clipser.db
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) dbInit.cpp -o $(DBINIT_BIN) $(LDLIBS)
	./$(DBINIT_BIN)

database: $(DBINIT_BIN)
	./$(DBINIT_BIN)
