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

# The operational system check. At least for now, we assume that we are always running
# on a Linux machine. Therefore, a disclaimer must be shown if this is not true.
SYSTEMOS := $(shell uname)
SYSTEMOS := $(patsubst MINGW%,Windows,$(SYSTEMOS))
SYSTEMOS := $(patsubst MSYS%,Msys,$(SYSTEMOS))
SYSTEMOS := $(patsubst CYGWIN%,Msys,$(SYSTEMOS))

ifneq ($(SYSTEMOS), Linux)
  $(info Warning: This makefile assumes OS to be Linux.)
endif

all: distribute

prepare-distribute:
	@mkdir -p $(DSTDIR)

distribute: prepare-distribute
	@$(MAKE) --no-print-directory -f makethird distribute

clean-distribute:
	@rm -rf $(DSTDIR)
	@$(MAKE) --no-print-directory -f makethird clean

clean: clean-distribute
	@rm -rf $(OBJDIR)
	@rm -rf $(BINDIR)

.PHONY: all clean
.PHONY: prepare-distribute distribute clean-distribute
