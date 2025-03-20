/**
 * Reflector: A simple struct reflection framework for C++17.
 * @file Configuration and inclusion of the supertuple third party library.
 * @author Rodrigo Siqueira <rodriados@gmail.com>
 * @copyright 2025-present Rodrigo Siqueira
 */
#pragma once

#ifdef REFLECTOR_OVERRIDE_SUPERTUPLE
  #include REFLECTOR_OVERRIDE_SUPERTUPLE
#elif __has_include(<rodriados/supertuple.h>)
  #include <rodriados/supertuple.h>
#elif __has_include(<supertuple/api.h>)
  #include <supertuple/api.h>
#else
  #include <supertuple.h>
#endif

/*
 * Check whether the dependency has been successfully included. If not, we must
 * bail out and inform that it is a required dependency and must be included.
 */
#ifndef SUPERTUPLE_HEADER_INCLUDED
  #error "SuperTuple is a required dependency and must be included before Reflector"
#endif
