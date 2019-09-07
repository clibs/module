#include <stdio.h>
#include "example.h"
#include "../require.h"

#define require(...) clib_module_require(__VA_ARGS__)
#define exports(...) clib_module_exports(__VA_ARGS__)
#define module(...) clib_module(__VA_ARGS__)
#define define(...) clib_module_define(__VA_ARGS__)

module(test) {
  define(test, CLIB_MODULE);
  example_t *example;
};

static int
init(module(test) *exports);

static void
deinit(module(test) *exports);

exports(test) {
  .init = init,
  .deinit = deinit,
};

static int
init(module(test) *exports) {
  clib_module_init(test, exports);
  exports->example = require(example);
  printf("init module(test)\n");
  return 0;
}

static void
deinit(module(test) *exports) {
  clib_module_deinit(test);
  clib_module_free(exports->example);
  printf("deinit module(test)\n");
}

int
main(void) {
  example_t *example = require(example);
  test_t *test = require(test);

  example->set("hello");
  printf("%s\n", (char *) example->get());

  clib_module_free(test);
  return 0;
}
