#include <stdio.h>

#include "example.h"
#include "../require.h"

#define defaults(...) clib_module_defaults(__VA_ARGS__)
#define require(...) clib_module_require(__VA_ARGS__)
#define exports(...) clib_module_exports(__VA_ARGS__)
#define module(...) clib_module(__VA_ARGS__)
#define define(...) clib_module_define(__VA_ARGS__)

module(internal) {
  define(internal, CLIB_MODULE);
  char *value;
};

exports(internal) {
  defaults(internal, CLIB_MODULE_DEFAULT),
  .value = 0,
};

static inline void *
get() {
  return require(internal)->value;
}

static inline void
set(void *val) {
  require(internal)->value = val;
}

static inline void
deinit(module(example) *exports) {
  printf("deinit module(example)\n");
  clib_module_free(require(internal));
  clib_module_deinit(internal);
}

static int
init(module(example) *exports) {
  clib_module_init(example, exports);
  exports->deinit = deinit;
  exports->set = set;
  exports->get = get;
  return 0;
}

int
example_module_init(module(example) *exports) {
  return init(exports);
}
