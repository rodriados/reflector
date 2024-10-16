# Reflector: A simple struct reflection framework for C++17.
# @file Makefile for compiling, installing and automatically testing.
# @author Rodrigo Siqueira <rodriados@gmail.com>
# @copyright 2024-present Rodrigo Siqueira
NAME = reflector

INCDIR = src
SRCDIR = src
EXPDIR = examples
TSTDIR = test

DSTDIR ?= dist
OBJDIR ?= obj
BINDIR ?= bin
PT3DIR ?= thirdparty

CXX    ?= g++
STDCPP ?= c++17

# Defining macros inside code at compile time. This can be used to enable or disable
# certain features on code or affect the projects compilation.
FLAGS 	  ?=
CXXFLAGS  ?= -std=$(STDCPP) -I$(DSTDIR) -I$(TSTDIR) -I$(INCDIR) $(FLAGS)
LINKFLAGS ?= $(FLAGS)

SRCFILES := $(shell find $(SRCDIR) -name '*.h')                                \
            $(shell find $(SRCDIR) -name '*.hpp')

TSTFILES := $(shell find $(TSTDIR) -name '*.cpp')
TESTOBJS := $(TSTFILES:$(TSTDIR)/%.cpp=$(OBJDIR)/$(TSTDIR)/%.o)

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

all: build-tests

prepare-tests:
	@mkdir -p $(BINDIR)/$(TSTDIR)
	@mkdir -p $(sort $(dir $(TESTOBJS)))

build-tests: override FLAGS = -DTESTING -g -O0
build-tests: thirdparty-distribute prepare-tests $(BINDIR)/$(TSTDIR)/runtest

run-tests: build-tests
	$(BINDIR)/$(TSTDIR)/runtest

prepare-distribute:
	@mkdir -p $(DSTDIR)

REFLECTOR_DIST_CONFIG ?= .packconfig
REFLECTOR_DIST_TARGET ?= $(DSTDIR)/$(NAME).h

distribute: prepare-distribute thirdparty-distribute $(REFLECTOR_DIST_TARGET)
no-thirdparty-distribute: prepare-distribute $(REFLECTOR_DIST_TARGET)

clean-distribute: thirdparty-clean
	@rm -f $(REFLECTOR_DIST_TARGET)
	@rm -rf $(DSTDIR)

INSTALL_DESTINATION ?= $(DESTDIR)$(PREFIX)/include
INSTALL_TARGETS = $(SRCFILES:$(SRCDIR)/%=$(INSTALL_DESTINATION)/%)

install: thirdparty-install $(INSTALL_TARGETS)

$(INSTALL_DESTINATION)/%: $(SRCDIR)/%
	install -m 644 -D -T $< $@

uninstall: thirdparty-uninstall
	@rm -f $(INSTALL_TARGETS)

clean: clean-distribute
	@rm -rf $(OBJDIR)
	@rm -rf $(BINDIR)

.PHONY: all clean tests install uninstall
.PHONY: prepare-distribute distribute no-thirdparty-distribute clean-distribute
.PHONY: prepare-tests build-tests run-tests

# Creates dependency on header files. This is valuable so that whenever a header
# file is changed, all objects depending on it will be forced to recompile.
ifneq ($(wildcard $(OBJDIR)/.),)
-include $(shell find $(OBJDIR) -name '*.d')
endif

$(BINDIR)/$(TSTDIR)/runtest: $(TESTOBJS)
	$(CXX) $(LINKFLAGS) $^ -o $@

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

$(REFLECTOR_DIST_TARGET): $(SRCFILES)
	python3 pack.py -c $(REFLECTOR_DIST_CONFIG) -o $@

# The target path for third party dependencies' distribution files. As each dependency
# may allow different settings, a variable for each one is needed.
THIRDPARTY_IGNORE ?=
THIRDPARTY_DEPENDENCIES = supertuple

THIRDPARTY_TARGETS := $(filter-out $(THIRDPARTY_IGNORE),$(THIRDPARTY_DEPENDENCIES))
THIRDPARTY_TARGETS := $(THIRDPARTY_TARGETS:%=$(DSTDIR)/%.h)

thirdparty-distribute: prepare-distribute $(THIRDPARTY_TARGETS)
thirdparty-install:    $(THIRDPARTY_DEPENDENCIES:%=thirdparty-install-%)
thirdparty-uninstall:  $(THIRDPARTY_DEPENDENCIES:%=thirdparty-uninstall-%)
thirdparty-clean:      $(THIRDPARTY_DEPENDENCIES:%=thirdparty-clean-%)

ifndef REFLECTOR_DIST_STANDALONE

export SUPERTUPLE_DIST_STANDALONE = 1

$(DSTDIR)/%.h: % thirdparty-distribute-%
	@cp $(PT3DIR)/$</$@ $@

thirdparty-distribute-%: %
	@$(MAKE) --no-print-directory -C $(PT3DIR)/$< distribute

thirdparty-install-%: %
	@$(MAKE) --no-print-directory -C $(PT3DIR)/$< install

thirdparty-uninstall-%: %
	@$(MAKE) --no-print-directory -C $(PT3DIR)/$< uninstall

thirdparty-clean-%: %
	@$(MAKE) --no-print-directory -C $(PT3DIR)/$< clean

else
.PHONY: $(THIRDPARTY_TARGETS)
.PHONY: $(THIRDPARTY_DEPENDENCIES:%=thirdparty-distribute-%)
.PHONY: $(THIRDPARTY_DEPENDENCIES:%=thirdparty-install-%)
.PHONY: $(THIRDPARTY_DEPENDENCIES:%=thirdparty-uninstall-%)
.PHONY: $(THIRDPARTY_DEPENDENCIES:%=thirdparty-clean-%)
endif

.PHONY: thirdparty-distribute thirdparty-install thirdparty-uninstall thirdparty-clean
.PHONY: $(THIRDPARTY_DEPENDENCIES)
