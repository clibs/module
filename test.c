#include <ok/ok.h>
#include <stdio.h>

#include "require.h"
#include "module.h"
#include "def.h"

// `test` module definition
module(test) {
  defaults(test, CLIB_MODULE);
  // a private module pointer
  void *private;
  int state;
  int (*function)();
};

// `test` module prototypes
static int
test_init(module(test) *exports);

static void
test_deinit(module(test) *exports);

// `test` module exports
exports(test) {
  .init = test_init,
  .deinit = test_deinit,
};

// `private` module definition
module(private) {
  define(private, CLIB_MODULE);
  int (*function)();
};

// private `private` module function symbol
static int
test_private_function() {
  ok("test_private_function()");
  return 0;
}

// `private` module exports
exports(private) {
  defaults(private, CLIB_MODULE_DEFAULT),
  .function = test_private_function
};

// private `test` module function symbol
static int
test_function() {
  ok("test_function()");
  require(test)->state = 1;
  require(private)->function();
  return 0;
}

// `test` module initializer
static int
test_init(module(test) *exports) {
  ok("test_init()");
  exports->function = test_function;
  exports->private = require(private);
  exports->state = -1;
  if (0 != exports->private) {
    ok("exports->private");
  }
  return 0;
}

// `test` module deinitializer
static void
test_deinit(module(test) *exports) {
  ok("test_deinit()");
  clib_module_free((module(private) *) exports->private);
}

int
main(void) {
  ok_expect(8);

  module(test) *test = require(test);

  if (0 != test) { ok("module(test) *test = require(test)"); }
  if (-1 == test->state) { ok("-1 == test->state // before test->function()"); }

  test->function();
  if (1 == test->state) { ok("1 == test->state // after test->function()"); }

  clib_module_free(test);

  ok_done();
  return ok_count() - ok_expected();
}
