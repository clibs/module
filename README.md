clibs/module
============

> Macros to help define static module structure around C APIs.

## Installation

```sh
$ clib install clibs/module --save
```

## Usage

```c
#include <clibs/module/require.h>
#include <clibs/module/module.h>
#include <clibs/module/def.h>

// module definition
module(your_module) {
  define(your_module, CLIB_MODULE);
};

// module initializer
int your_module_init(module(your_module) *exports) {
  return 0;
}

// module de-initializer
void your_module_deinit(module(your_module) *exports) {
}

// module exports
exports(your_module) {
  .init = your_module_init,
  .deinit = your_module_deinit,
};
```

### Defining A Module

```c
// the 'clib_module', 'clib_exports', etc macros
#include <clibs/module/module.h>

// defines 'module, exports, defaults, define, require' alias macros
#include <clibs/module/def.h>

enum channel_status {
  CHANNEL_STATUS_ERROR = -1,
  CHANNEL_STATUS_NONE = 0,
  CHANNEL_STATUS_OPEN = 1,
  CHANNEL_STATUS_CLOSED = 2,
};

// creates a module struct with the name 'channel'
// ie: `struct channel_clib_module { };`
module(channel) {
  // this initializes a few fields using the 'CLIB_MODULE'
  // prototype which is the expected prototype for the
  // 'clib_module_require()' function to work
  define(channel, CLIB_MODULE);

  // channel functions
  enum channel_status state;
  int (*open)();
  int (*close)();
  int (*send)(void *buf, size_t size);
  int (*recv)(void *buf, size_t size);
};
```

### Exporting Module Symbols

```c
// creates an exports interface with optional default values
exports(channel) {
  // the module initializer function called exactly once
  .init = channel_init,
  // the module deinitializer function called when free'd with
  // 'cilb_module_free' just before the 'free()' call
  .deinit = channel_deinit,
};
```

### Requiring Module Symbols

```c
module(channel) *channel = require(channel);
```

## Example

Consider a simple logging module that defines and exports a `mode` property,
and `info()`, `debug()`, and `error()` logging functions.

Below is a `logger.h` file that defines a `logger` module with a few
exports.

_**logger.h:**_

```c
#include <clibs/module/require.h>
#include <clibs/module/module.h>
#include <clibs/module/def.h>

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
```

The following `logger.c` file implements the `logger` module definition
with a few explicit symbols `init()` and `deinit()`. These symbols
can be set to `0` (`NULL`) for default behavior.

_**logger.c:**_

```c
#include <stdio.h>
#include "logger.h"

// Default Module Exports
exports(logger) {
  .mode = LOGGER_NONE,
  .init = logger_init,
  .deinit = logger_deinit,
};

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
```

Below is a program consumes the `logger` module.

```c

#include "logger.h"

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
```

## API

### `clibs/module/module.h`

#### `clib_module(type)`

Gets a reference to the module structure type.

```c
// as a definition
clib_module(module) {
  clib_module_define(module, CLIB_MODULE);
};

// as type
clib_module(module) *exports = require(module);
```

**Source:**

```c
#ifndef clib_module
#define clib_module(type) struct __##type##_clib_module
#endif
```

#### `clib_module_exports(type)`

Initializes the exports of a module and scopes its definition.

```c
clib_module_exports(module) {
  clib_module_define(module, CLIB_MODULE);
};
```

**Source:**

```c
#ifndef clib_module_exports
#define clib_module_exports(type)                                              \
  typedef clib_module(type) type##_t;                                          \
  static unsigned int __##type##_clib_module_init = 0;                         \
  static clib_module(type) *__##type##_clib_module;                            \
  static clib_module(type) __##type##_clib_module_exports =                    \

#endif
```

#### `CLIB_MODULE_DEFAULT`

The default module field initializer prototype.

```c
clib_module_exports(module) {
  clib_module_defaults(module, CLIB_MODULE_DEFAULT)
}
```

**Source:**

```c
#ifndef CLIB_MODULE_DEFAULT
#define CLIB_MODULE_DEFAULT(type)  \
  .name = ""#type,                 \
  .init = 0,                       \
  .deinit = 0                      \

#endif
```

#### `clib_module_defaults(type, prototype)`

Initializes defaults on a module type with a prototype.

```
clib_module_exports(module) {
  clib_module_defaults(module, CLIB_MODULE_DEFAULT)
};
```

**Source:**

```c
#ifndef clib_module_defaults
#define clib_module_defaults(type, prototype) prototype(type)
#endif
```

#### `clib_module_define(module, prototype)`

Defines a module with a prototype's fields intended to be called inside the
definition of a module structure.

```c
clib_module(module) {
  clib_module_define(module, CLIB_MODULE);
  char *value;
  void *(function)(void *);
};
```

Custom prototypes can be used by defining a new macro

```c
#define CUSTOM_PROTOTYPE      \
  CLIB_MODULE                 \
  void *(*function)(void *);  \

clib_module(module) {
  clib_module_define(module, CUSTOM_PROTOTYPE);
};

clib_exports(module) {
  .init = init, .deinit = 0,
};
```

**Source:**

```c
#ifndef clib_module_define
#define clib_module_define(module, prototype) prototype(module)
#endif
```

#### `clib_module_free(module)`

Frees a module pointer and calls `deinit()` right before.

```c
clib_module_free(module)
```

**Source:**

```c
#ifndef clib_module_free
#define clib_module_free(module)                                               \
  if (0 != (module)) {                                                         \
    if (0 != (module)->deinit) {                                               \
      (module)->deinit((module));                                              \
    }                                                                          \
    free(module);                                                              \
  }

#endif
```

### `clibs/module/require.h`

#### `clib_module_require(name)`

Returns an allocated pointer a module that should be
free'd with `clib_module_free()`.

```c
clib_module(module) *mod = clib_module_require(module);
```

**Source:**

```c
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
```

### `clibs/module/def.h`

The `def.h` header file defines a set of alias macros for convenience.

```c
module(type) {
  define(type, CLIB_MODULE);
  char *value;
};

exports(type) {
  defaults(type, CLIB_MODULE_DEFAULT),
  .value = "default value"
};

module(type) *exports = require(type);
```

**Source:**

```c
#define defaults(...) clib_module_defaults(__VA_ARGS__)
#define require(...) clib_module_require(__VA_ARGS__)
#define exports(...) clib_module_exports(__VA_ARGS__)
#define module(...) clib_module(__VA_ARGS__)
#define define(...) clib_module_define(__VA_ARGS__)
```

## Tests

Tests can be built and ran by running `make test`:

```sh
$ make test
```

Or with the `clib build` command:

```sh
$ clib build --force --test
```

## Examples

Examples can be built and ran by running `make example/{EXAMPLE}`

```sh
$ make example/logger
$ make example/main
```

Or with the `clib build` command:

```sh
$ clib build --force -- example/logger
$ clib build --force -- example/main
```

## License

MIT
