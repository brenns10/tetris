#-------------------------------------------------------------------------------
#
# File:         Makefile
#
# Author:       Stephen Brennan
#
# Date Created: Tuesday, 10 June 2015
#
# Description:  Makefile for CKY parser program.
#
# Copyright (c) 2015, Stephen Brennan.  Released under the Revised BSD License.
# See the LICENSE.txt file for details.
#
#-------------------------------------------------------------------------------

# Compiler Variable Declarations
CC=gcc
FLAGS=-Wall -pedantic
INC=-Isrc/
CFLAGS=$(FLAGS) -c -g --std=c99 $(INC)
LFLAGS=$(FLAGS) -lncurses
DIR_GUARD=@mkdir -p $(@D)

# Build configurations.
CFG=release
ifeq ($(CFG),debug)
FLAGS += -g -DDEBUG -DSMB_DEBUG
endif
ifneq ($(CFG),debug)
ifneq ($(CFG),release)
	@echo "Invalid configuration "$(CFG)" specified."
	@echo "You must specify a configuration when running make, e.g."
	@echo "  make CFG=debug"
	@echo "Choices are 'release', 'debug'."
	@exit 1
endif
endif

SDL=yes
ifeq ($(SDL),yes)
CFLAGS += `sdl-config --cflags` -DWITH_SDL=1
LFLAGS += `sdl-config --libs` -lSDL_mixer
endif
ifneq ($(SDL),yes)
ifneq ($(SDL),no)
	@echo "Invalid SDL configuration "$(SDL)" specified."
	@echo "You must specify the SDL configuration when running make, e.g."
	@echo "  make SDL=yes"
	@echo "Choices are 'yes', 'no'."
	@exit 1
endif
endif

# Sources and Objects
SOURCES=$(shell find src/ -type f -name "*.c")
OBJECTS=$(patsubst src/%.c,obj/$(CFG)/%.o,$(SOURCES))
DEPS=$(patsubst src/%.c,deps/%.d,$(SOURCES))

# Main targets
.PHONY: all clean clean_all

all: bin/$(CFG)/main

GTAGS: $(SOURCES)
	gtags

clean:
	rm -rf obj/$(CFG)/* bin/$(CFG)/* src/*.gch GTAGS GPATH GRTAGS

clean_all:
	rm -rf bin/* obj/* deps/*

# --- Compile Rule
obj/$(CFG)/%.o: src/%.c
	$(DIR_GUARD)
	$(CC) $(CFLAGS) $< -o $@

# --- Link Rule
bin/$(CFG)/main: $(OBJECTS)
	$(DIR_GUARD)
	$(CC) $(OBJECTS) $(LFLAGS) -o bin/$(CFG)/main

# --- Dependency Rule
deps/%.d: src/%.c
	$(DIR_GUARD)
	$(CC) $(CFLAGS) -MM $< | sed -e 's/~\(.*\)\.o:/\1.d \1.o:/' > $@

ifneq "$(MAKECMDGOALS)" "clean_all"
-include $(DEPS)
endif
