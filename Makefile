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

# Sources and Objects
SOURCES=$(shell find src/ -type f -name "*.c")
SOURCEDIRS=$(shell find src/ -type d)

OBJECTS=$(patsubst src/%.c,obj/$(CFG)/%.o,$(SOURCES))

# Main targets
.PHONY: all clean clean_all

all: Makefile.deps bin/$(CFG)/main GTAGS

Makefile.deps: $(SOURCES)
	$(CC) $(CFLAGS) -MM $(SOURCES) > Makefile.deps
	make

GTAGS: $(SOURCES)
	gtags

clean:
	rm -rf bin/$(CFG)/* obj/$(CFG)/* src/*.gch GTAGS GPATH GRTAGS Makefile.deps

clean_all:
	rm -rf bin/* obj/*

# --- Compile Rule
obj/$(CFG)/%.o: src/%.c
	$(DIR_GUARD)
	$(CC) $(CFLAGS) $< -o $@

# --- Link Rule
bin/$(CFG)/main: $(OBJECTS)
	$(DIR_GUARD)
	$(CC) $(LFLAGS) $(OBJECTS) -o bin/$(CFG)/main

-include Makefile.deps
