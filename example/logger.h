#include "../require.h"
#include "../module.h"
#include "../def.h"

enum logger_mode {
  LOGGER_NONE,
  LOGGER_INFO,
  LOGGER_ERROR,
  LOGGER_DEBUG,
};

// Module Type Interface
module(logger) {
  define(logger, CLIB_MODULE);

  enum logger_mode mode;
  void (*info)(char *);
  void (*debug)(char *);
  void (*error)(char *);
};

int
logger_init(module(logger) *exports);

void
logger_deinit(module(logger) *exports);

// Default Module Exports
exports(logger) {
  .mode = LOGGER_NONE,
  .init = logger_init,
  .deinit = logger_deinit,
};
