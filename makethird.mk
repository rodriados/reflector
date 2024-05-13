# Reflector: A simple struct reflection framework for C++17.
# @file Makefile for installing and distributing thirdparty dependencies.
# @author Rodrigo Siqueira <rodriados@gmail.com>
# @copyright 2024-present Rodrigo Siqueira
export
NAME ?= reflector

DSTDIR ?= dist
TSTDIR ?= test
OBJDIR ?= obj
BINDIR ?= bin
PT3DIR ?= thirdparty

CXX    ?= g++
STDCPP ?= c++17

# The target path for third party dependencies' distribution files. As each dependency
# may allow different settings, a variable for each one is needed.
SUPERTUPLE_DIST_TARGET ?= $(DSTDIR)/supertuple.hpp

THIRDPARTY_DEPENDENCIES ?= supertuple
THIRDPARTY_TARGETS = $(SUPERTUPLE_DIST_TARGET)

distribute: prepare-distribute $(THIRDPARTY_TARGETS)
install:    $(THIRDPARTY_DEPENDENCIES:%=install-%)
uninstall:  $(THIRDPARTY_DEPENDENCIES:%=uninstall-%)
clean:      $(THIRDPARTY_DEPENDENCIES:%=clean-%)

.PHONY: prepare-distribute distribute install uninstall clean
.PHONY: $(THIRDPARTY_DEPENDENCIES)

prepare-distribute:
	@mkdir -p $(DSTDIR)

$(SUPERTUPLE_DIST_TARGET): supertuple
	@$(MAKE) --no-print-directory -C $(PT3DIR)/$< distribute
	cp $(PT3DIR)/$</$@ $@

install-%: %
	@$(MAKE) --no-print-directory -C $(PT3DIR)/$< install

uninstall-%: %
	@$(MAKE) --no-print-directory -C $(PT3DIR)/$< uninstall

clean-%: %
	@$(MAKE) --no-print-directory -C $(PT3DIR)/$< clean
