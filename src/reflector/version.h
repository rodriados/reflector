/**
 * Reflector: A simple struct reflection framework for C++17.
 * @file Compiler-time macros encoding Reflector release version.
 * @author Rodrigo Siqueira <rodriados@gmail.com>
 * @copyright 2024-present Rodrigo Siqueira
 */
#pragma once

/**
 * The preprocessor macros encoding the current Reflector library release version.
 * This is guaranteed to change with every Reflector release.
 */
#define REFLECTOR_VERSION 10000

/**
 * The preprocessor macros encoding the release policy's values to the current Reflector
 * library release version.
 */
#define REFLECTOR_VERSION_MAJOR (REFLECTOR_VERSION / 10000)
#define REFLECTOR_VERSION_MINOR (REFLECTOR_VERSION / 100 % 100)
#define REFLECTOR_VERSION_PATCH (REFLECTOR_VERSION % 100)
/**#@-*/
