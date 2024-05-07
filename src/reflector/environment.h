/**
 * Reflector: A simple struct reflection framework for C++17.
 * @file Environment configuration and macro values
 * @author Rodrigo Siqueira <rodriados@gmail.com>
 * @copyright 2024-present Rodrigo Siqueira
 */
#pragma once

#include <reflector/version.h>

/*
 * Enumerates all possible target environment modes to which the code might be compiled
 * to. The environment mode may affect some features' availability and performace.
 */
#define REFLECTOR_BUILD_DEV        0
#define REFLECTOR_BUILD_DEBUG      1
#define REFLECTOR_BUILD_TESTING    2
#define REFLECTOR_BUILD_PRODUCTION 3

/*
 * Discovers and explicits the target environment mode to which the code must be
 * currently compiled to. The mode may affect some features' availability and performance.
 */
#if defined(DEBUG) || defined(_DEBUG)
  #define REFLECTOR_BUILD REFLECTOR_BUILD_DEBUG
  #define REFLECTOR_ENVIRONMENT "Debug"
#elif defined(TESTING)
  #define REFLECTOR_BUILD REFLECTOR_BUILD_TESTING
  #define REFLECTOR_ENVIRONMENT "Testing"
#elif defined(DEV) || defined(DEVELOPMENT)
  #define REFLECTOR_BUILD REFLECTOR_BUILD_DEV
  #define REFLECTOR_ENVIRONMENT "Development"
#else
  #define REFLECTOR_BUILD REFLECTOR_BUILD_PRODUCTION
  #define REFLECTOR_ENVIRONMENT "Production"
#endif

/*
 * Enumerating known host compilers. These compilers are not all necessarily officially
 * supported. Nevertheless, some special adaptation or fixes might be implemented
 * to each one of these if so needed.
 */
#define REFLECTOR_HOST_COMPILER_UNKNOWN 0
#define REFLECTOR_HOST_COMPILER_GCC     1
#define REFLECTOR_HOST_COMPILER_CLANG   2

/*
 * Enumerating known device compilers. These compilers are not all necessarily officially
 * supported. Nevertheless, some special adaptation or fixes might be implemented
 * to each one of these if so needed.
 */
#define REFLECTOR_DEVICE_COMPILER_UNKNOWN 0
#define REFLECTOR_DEVICE_COMPILER_GCC     1
#define REFLECTOR_DEVICE_COMPILER_CLANG   2
#define REFLECTOR_DEVICE_COMPILER_NVCC    3

/*
 * Finds the version of the host compiler being used. Some features might change
 * or be unavailable depending on the current compiler configuration.
 */
#if defined(__clang__)
  #define REFLECTOR_HOST_COMPILER REFLECTOR_HOST_COMPILER_CLANG
  #define REFLECTOR_CLANG_VERSION (__clang_major__ * 100 + __clang_minor__)
#elif defined(__GNUC__) || defined(__GNUG__)
  #define REFLECTOR_HOST_COMPILER REFLECTOR_HOST_COMPILER_GCC
  #define REFLECTOR_GCC_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)
#else
  #define REFLECTOR_HOST_COMPILER REFLECTOR_HOST_COMPILER_UNKNOWN
#endif

/*
 * Finds the version of the device compiler being used. Some features might change
 * or be unavailable depending on the current compiler configuration.
 */
#if defined(__clang__)
  #define REFLECTOR_DEVICE_COMPILER REFLECTOR_DEVICE_COMPILER_CLANG
#elif defined(__CUDACC__) || defined(__NVCOMPILER_CUDA__)
  #define REFLECTOR_DEVICE_COMPILER REFLECTOR_DEVICE_COMPILER_NVCC
#elif REFLECTOR_HOST_COMPILER == REFLECTOR_HOST_COMPILER_GCC
  #define REFLECTOR_DEVICE_COMPILER REFLECTOR_DEVICE_COMPILER_GCC
#else
  #define REFLECTOR_DEVICE_COMPILER REFLECTOR_DEVICE_COMPILER_UNKNOWN
#endif

/*
 * Macro for programmatically emitting a pragma call, independently on which compiler
 * is currently in use.
 */
#if !defined(REFLECTOR_EMIT_PRAGMA_CALL)
  #define REFLECTOR_EMIT_PRAGMA_CALL(x) _Pragma(#x)
#endif

/*
 * Macro for programmatically emitting a compiler warning. This will be shown during
 * compile-time and might stop compilation if warnings are treated as errors.
 */
#if !defined(REFLECTOR_EMIT_COMPILER_WARNING)
  #if (REFLECTOR_HOST_COMPILER == REFLECTOR_HOST_COMPILER_GCC)
    #define REFLECTOR_EMIT_COMPILER_WARNING(msg)           \
      REFLECTOR_EMIT_PRAGMA_CALL(GCC warning msg)
  #elif (REFLECTOR_HOST_COMPILER == REFLECTOR_HOST_COMPILER_CLANG)
    #define REFLECTOR_EMIT_COMPILER_WARNING(msg)           \
      REFLECTOR_EMIT_PRAGMA_CALL(clang warning msg)
  #else
    #define REFLECTOR_EMIT_COMPILER_WARNING(msg)
  #endif
#endif

/*
 * Macro for disabling or manually emitting warnings when compiling host code with
 * GCC. This is useful for hiding buggy compiler warnings or compiler exploits that
 * have intentionally been taken advantage of.
 */
#if (REFLECTOR_HOST_COMPILER == REFLECTOR_HOST_COMPILER_GCC) && !defined(__CUDA_ARCH__)
  #define REFLECTOR_EMIT_GCC_WARNING(x) REFLECTOR_EMIT_COMPILER_WARNING(x)
  #define REFLECTOR_DISABLE_GCC_WARNING_BEGIN(x)           \
    REFLECTOR_EMIT_PRAGMA_CALL(GCC diagnostic push)        \
    REFLECTOR_EMIT_PRAGMA_CALL(GCC diagnostic ignored x)
  #define REFLECTOR_DISABLE_GCC_WARNING_END(x)             \
    REFLECTOR_EMIT_PRAGMA_CALL(GCC diagnostic pop)
#else
  #define REFLECTOR_DISABLE_GCC_WARNING_BEGIN(x)
  #define REFLECTOR_DISABLE_GCC_WARNING_END(x)
  #define REFLECTOR_EMIT_GCC_WARNING(x)
#endif

#if (REFLECTOR_HOST_COMPILER == REFLECTOR_HOST_COMPILER_CLANG)
  #define REFLECTOR_EMIT_CLANG_WARNING(x) REFLECTOR_EMIT_COMPILER_WARNING(x)
  #define REFLECTOR_DISABLE_CLANG_WARNING_BEGIN(x)           \
    REFLECTOR_EMIT_PRAGMA_CALL(clang diagnostic push)        \
    REFLECTOR_EMIT_PRAGMA_CALL(clang diagnostic ignored x)
  #define REFLECTOR_DISABLE_CLANG_WARNING_END(x)             \
    REFLECTOR_EMIT_PRAGMA_CALL(clang diagnostic pop)
#else
  #define REFLECTOR_DISABLE_CLANG_WARNING_BEGIN
  #define REFLECTOR_DISABLE_CLANG_WARNING_END
  #define REFLECTOR_EMIT_CLANG_WARNING
#endif

/*
 * Macro for pushing compiling options for GCC. These allow certain compiler behaviors
 * to be modified for the code within the block of changed settings.
 */
#if (REFLECTOR_HOST_COMPILER == REFLECTOR_HOST_COMPILER_GCC) && !defined(__CUDA_ARCH__)
  #define REFLECTOR_PUSH_GCC_OPTION_BEGIN(x)               \
    REFLECTOR_EMIT_PRAGMA_CALL(GCC push_options)           \
    REFLECTOR_EMIT_PRAGMA_CALL(GCC x)
  #define REFLECTOR_PUSH_GCC_OPTION_END(x)                 \
    REFLECTOR_EMIT_PRAGMA_CALL(GCC pop_options)
#else
  #define REFLECTOR_PUSH_GCC_OPTION_BEGIN(x)
  #define REFLECTOR_PUSH_GCC_OPTION_END(x)
#endif

/*
 * Macro for disabling warnings when compiling device code with NVCC. This is useful
 * for hiding buggy compiler warnings or intentional compiler exploits.
 */
#if (REFLECTOR_DEVICE_COMPILER == REFLECTOR_DEVICE_COMPILER_NVCC)
  #define REFLECTOR_DISABLE_NVCC_WARNING_BEGIN(x)          \
    REFLECTOR_EMIT_PRAGMA_CALL(push)                       \
    REFLECTOR_EMIT_PRAGMA_CALL(nv_diag_suppress = x)
  #define REFLECTOR_DISABLE_NVCC_WARNING_END(x)            \
    REFLECTOR_EMIT_PRAGMA_CALL(pop)
#else
  #define REFLECTOR_DISABLE_NVCC_WARNING_BEGIN(x)
  #define REFLECTOR_DISABLE_NVCC_WARNING_END(x)
#endif

/*
 * Discovers the C++ language dialect in use for the current compilation. A specific
 * dialect might not be supported or might be required for certain functionalities
 * to work properly.
 */
#if defined(__cplusplus)
  #if __cplusplus < 201103L
    #define REFLECTOR_CPP_DIALECT 2003
  #elif __cplusplus < 201402L
    #define REFLECTOR_CPP_DIALECT 2011
  #elif __cplusplus < 201703L
    #define REFLECTOR_CPP_DIALECT 2014
  #elif __cplusplus == 201703L
    #define REFLECTOR_CPP_DIALECT 2017
  #elif __cplusplus > 201703L
    #define REFLECTOR_CPP_DIALECT 2020
  #endif
#endif

/*
 * Checks the current compiler's C++ language level. As the majority of this software's
 * codebase is written in C++, we must check whether its available or not.
 */
#if !defined(REFLECTOR_IGNORE_CPP_DIALECT)
  #if !defined(REFLECTOR_CPP_DIALECT) || REFLECTOR_CPP_DIALECT < 2017
    #error This software requires at least a C++17 enabled compiler.
  #endif
#endif

/*
 * Checks whether the current compiler is compatible with the software's prerequisites.
 * Should it not be compatible, then we emit a warning but try compiling anyway.
 */
#if !defined(REFLECTOR_IGNORE_COMPILER_CHECK) && REFLECTOR_BUILD == REFLECTOR_BUILD_PRODUCTION
  #if REFLECTOR_HOST_COMPILER == REFLECTOR_HOST_COMPILER_UNKNOWN
    #warning This software has not been tested with the current compiler.
  #endif
#endif

/*
 * Since only NVCC knows how to deal with `__host__` and `__device__` annotations,
 * we define them to empty strings when another compiler is in use. This allows
 * the use of these annotations without needing to care whether they'll be known
 * by the compiler or not.
 */
#if REFLECTOR_DEVICE_COMPILER == REFLECTOR_DEVICE_COMPILER_NVCC
  #define REFLECTOR_CUDA_ENABLED __host__ __device__
#else
  #define REFLECTOR_CUDA_ENABLED
#endif

/*
 * Macros for applying annotations and qualifiers to functions and methods. As the
 * minimum required language version is C++17, we assume it is guaranteed that all
 * compilers will have `inline` and `constexpr` implemented.
 */
#define REFLECTOR_INLINE REFLECTOR_CUDA_ENABLED inline
#define REFLECTOR_CONSTEXPR REFLECTOR_INLINE constexpr

/**
 * Defines the namespace in which the library lives. This might be overriden if
 * the default namespace value is already in use.
 * @since 1.0
 */
#if defined(REFLECTOR_OVERRIDE_NAMESPACE)
  #define REFLECTOR_NAMESPACE REFLECTOR_OVERRIDE_NAMESPACE
#else
  #define REFLECTOR_NAMESPACE reflector
#endif

/**
 * This macro is used to open the `reflector::` namespace block and must not be in
 * any way overriden. This namespace must not be prefixed by any other namespaces
 * to avoid problems when allowing the use some of the library's facilities to with
 * STL's algorithms, structures or constructions.
 * @since 1.0
 */
#define REFLECTOR_BEGIN_NAMESPACE       \
    namespace REFLECTOR_NAMESPACE {     \
        inline namespace v1 {           \
        namespace reflector = ::REFLECTOR_NAMESPACE;

/**
 * This macro is used to close the `reflector::` namespace block and must not be
 * in any way overriden.
 * @since 1.0
 */
#define REFLECTOR_END_NAMESPACE     }}
