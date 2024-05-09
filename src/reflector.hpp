/**
 * Reflector: A simple struct reflection framework for C++17.
 * @file The project's main header file.
 * @author Rodrigo Siqueira <rodriados@gmail.com>
 * @copyright 2024-present Rodrigo Siqueira
 */
#pragma once

#include <reflector/version.h>
#include <reflector/environment.h>

#ifndef REFLECTOR_AVOID_LOOPHOLE
  #if REFLECTOR_CPP_DIALECT < 2014
    #define REFLECTOR_AVOID_LOOPHOLE
  #endif
#endif

#include <reflector/descriptor.hpp>
#include <reflector/reflector.hpp>
