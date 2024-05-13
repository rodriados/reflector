# Reflector: A simple struct reflection framework for C++17.
# @file Makefile for compiling, installing and automatically testing.
# @author Rodrigo Siqueira <rodriados@gmail.com>
# @copyright 2024-present Rodrigo Siqueira
export
NAME = reflector

INCDIR = src
SRCDIR = src
EXPDIR = examples
TSTDIR = test

DSTDIR ?= dist
OBJDIR ?= obj
BINDIR ?= bin

CXX    ?= g++
STDCPP ?= c++17

MKTHIRD ?= makethird.mk

# Defining macros inside code at compile time. This can be used to enable or disable
# certain features on code or affect the projects compilation.
FLAGS 	  ?=
LINKFLAGS ?=
CXXFLAGS  ?= -std=$(STDCPP) -I$(INCDIR) $(FLAGS)

SRCFILES := $(shell find $(SRCDIR) -name '*.h')                                \
            $(shell find $(SRCDIR) -name '*.hpp')

# The operational system check. At least for now, we assume that we are always running
# on a Linux machine. Therefore, a disclaimer must be shown if this is not true.
SYSTEMOS := $(shell uname)
SYSTEMOS := $(patsubst MINGW%,Windows,$(SYSTEMOS))
SYSTEMOS := $(patsubst MSYS%,Msys,$(SYSTEMOS))
SYSTEMOS := $(patsubst CYGWIN%,Msys,$(SYSTEMOS))

ifneq ($(SYSTEMOS), Linux)
  $(info Warning: This makefile assumes OS to be Linux.)
endif

# If running an installation target, a prefix variable is used to determine where
# the files must be copied to. In this context, a default value must be provided.
ifeq ($(PREFIX),)
	PREFIX := /usr/local
endif

all: distribute

prepare-distribute:
	@mkdir -p $(DSTDIR)

REFLECTOR_DIST_CONFIG ?= .packconfig
REFLECTOR_DIST_TARGET ?= $(DSTDIR)/$(NAME).hpp

distribute: prepare-distribute thirdparty-distribute $(REFLECTOR_DIST_TARGET)
no-thirdparty-distribute: prepare-distribute $(REFLECTOR_DIST_TARGET)

thirdparty-distribute:
	@$(MAKE) --no-print-directory -f $(MKTHIRD) distribute

clean-distribute: thirdparty-clean
	@rm -f $(REFLECTOR_DIST_TARGET)
	@rm -rf $(DSTDIR)

INSTALL_DESTINATION ?= $(DESTDIR)$(PREFIX)/include
INSTALL_TARGETS = $(SRCFILES:$(SRCDIR)/%=$(INSTALL_DESTINATION)/%)

install: thirdparty-install $(INSTALL_TARGETS)

thirdparty-install:
	@$(MAKE) --no-print-directory -f $(MKTHIRD) install

$(INSTALL_DESTINATION)/%: $(SRCDIR)/%
	install -m 644 -D -T $< $@

uninstall: thirdparty-uninstall
	@rm -f $(INSTALL_TARGETS)

thirdparty-uninstall:
	@$(MAKE) --no-print-directory -f $(MKTHIRD) uninstall

thirdparty-clean:
	@$(MAKE) --no-print-directory -f $(MKTHIRD) clean

clean: clean-distribute
	@rm -rf $(OBJDIR)
	@rm -rf $(BINDIR)

.PHONY: all install uninstall clean
.PHONY: prepare-distribute distribute thirdparty-distribute no-thirdparty-distribute clean-distribute
.PHONY: thirdparty-install thirdparty-uninstall thirdparty-clean

$(REFLECTOR_DIST_TARGET): $(SRCFILES)
	python pack.py -c $(REFLECTOR_DIST_CONFIG) -o $@
