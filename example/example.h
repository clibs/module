#ifndef CLIB_MODULE_EXAMPLE_H
#define CLIB_MODULE_EXAMPLE_H

#include "../module.h"

/**
 * An example module definition with the `CLIB_MODULE`
 * module prototype.
 */
clib_module(example) {
  clib_module_define(example, CLIB_MODULE);
  void (*set)(void *);
  void *(*get)();
};

/**
 * An example module initializer.
 */
int
example_module_init(clib_module(example) *exports);

/**
 * An example module symbol exports initial state.
 */
clib_module_exports(example) {
  .init = example_module_init,
  .deinit = 0,
};

#endif
