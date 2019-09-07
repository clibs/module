#ifndef CLIB_MODULE_REQUIRE_H
#define CLIB_MODULE_REQUIRE_H

#include <string.h>
#include <stdlib.h>

#include "module.h"

/**
 * Returns an allocated pointer a module that should be
 * free'd with `clib_module_free()`.
 *
 * ```
 *   clib_module(module) *mod = clib_module_require(module);
 * ```
 */
#define clib_module_require(name) ({                                           \
  if (0 == __##name##_clib_module_init || 0 == __##name##_clib_module) {       \
    __##name##_clib_module = malloc(sizeof(*__##name##_clib_module));          \
    clib_module_init(name, __##name##_clib_module);                            \
                                                                               \
    memset(                                                                    \
      __##name##_clib_module,                                                  \
      0,                                                                       \
      sizeof(__##name##_clib_module_exports));                                 \
                                                                               \
    memcpy(                                                                    \
      __##name##_clib_module,                                                  \
      &__##name##_clib_module_exports,                                         \
      sizeof(__##name##_clib_module_exports));                                 \
    __##name##_clib_module_init = 1;                                           \
                                                                               \
    if (0 != (__##name##_clib_module)->init) {                                 \
      if (0 != (__##name##_clib_module)->init((__##name##_clib_module))) {     \
        free(__##name##_clib_module);                                          \
        (__##name##_clib_module) = 0;                                          \
      }                                                                        \
    }                                                                          \
  }                                                                            \
                                                                               \
  (__##name##_clib_module);                                                    \
})

#endif
