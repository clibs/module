#include <stdio.h>
#include "logger.h"

// Default Module Exports
exports(logger) {
  .mode = LOGGER_NONE,
  .init = logger_init,
  .deinit = logger_deinit,
};

static inline void
logger_info(char *message) {
  if (require(logger)->mode >= LOGGER_INFO) {
    fprintf(stdout, " info: %s\n", message);
  }
}

static inline void
logger_error(char *message) {
  if (require(logger)->mode >= LOGGER_ERROR) {
    fprintf(stderr, "error: %s\n", message);
  }
}

static inline void
logger_debug(char *message) {
  if (require(logger)->mode >= LOGGER_DEBUG) {
    fprintf(stderr, "debug: %s\n", message);
  }
}

static inline int
logger_init(module(logger) *exports) {
  clib_module_init(logger, exports);
  exports->mode = LOGGER_NONE;
  exports->info = logger_info;
  exports->error = logger_error;
  exports->debug = logger_debug;
  return 0;
}

static void
logger_deinit(module(logger) *exports) {
  clib_module_deinit(logger);
}

int
main(void) {
  module(logger) *logger = require(logger);
  logger->mode = LOGGER_DEBUG;
  logger->info("hello");
  logger->error("oops");
  logger->debug("help");
  clib_module_free(logger);
  return 0;
}
