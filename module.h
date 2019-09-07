#ifndef CLIB_MODULE_H
#define CLIB_MODULE_H

/**
 * Gets a reference to the module structure type.
 *
 * ```
 *  // as a definition
 *  clib_module(module) {
 *    clib_module_define(module, CLIB_MODULE);
 *  };
 *
 *  // as type
 *  clib_module(module) *exports = require(module);
 * ```
 */
#ifndef clib_module
#define clib_module(type) struct __##type##_clib_module
#endif

/**
 * Initializes the exports of a module and scopes its definition.
 *
 * ```
 *   clib_module_exports(module) {
 *     clib_module_define(module, CLIB_MODULE);
 *   };
 * ```
 */
#ifndef clib_module_exports
#define clib_module_exports(type)                                              \
  typedef clib_module(type) type##_t;                                          \
  clib_module(type) *__##type##_clib_module;                                   \
  static unsigned int __##type##_clib_module_init = 0;                         \
  static clib_module(type) __##type##_clib_module_exports =                    \

#endif

/**
 * Creates clib module fields for a structure definition.
 *
 * ```
 *   clib_module(module) {
 *     clib_module_fields(module);
 *   };
 * ```
 */
#ifndef clib_module_fields
#define clib_module_fields(type)                                               \
  const char *name;                                                            \
  int (*init)(clib_module(type) *exports);                                     \
  void (*deinit)(clib_module(type) *exports);                                  \

#endif

/**
 * The clib module type definition prototype intended to be used with the
 * `clib_module()` macro.
 *
 * ```
 *   clib_module(module) {
 *     clib_module_define(module, CLIB_MODULE);
 *   };
 * ```
 */
#ifndef CLIB_MODULE
#define CLIB_MODULE clib_module_fields
#endif

/**
 * The default module field initialization.
 */
#ifndef CLIB_MODULE_DEFAULT
#define CLIB_MODULE_DEFAULT(type)  \
  .name = ""#type,                 \
  .init = 0,                       \
  .deinit = 0                      \

#endif

/**
 * Initializes defaults on a module type with a prototype.
 *
 * ```
 *  clib_module_exports(module) {
 *    clib_module_defaults(module, CLIB_MODULE_DEFAULT)
 *  };
 * ```
 */
#ifndef clib_module_defaults
#define clib_module_defaults(type, prototype) prototype(type)
#endif

/**
 * Defines a module with a prototype's fields intended to be called inside the
 * definition of a module structure.
 *
 * ```
 *   clib_module(module) {
 *    clib_module_define(module, CLIB_MODULE);
 *    char *value;
 *    void *(function)(void *);
 *   };
 * ```
 *
 * Custom prototypes can be used by defining a new macro
 *
 * ```
 *   #define CUSTOM_PROTOTYPE      \
 *     CLIB_MODULE                 \
 *     void *(*function)(void *);  \
 *
 *   clib_module(module) {
 *    clib_module_define(module, CUSTOM_PROTOTYPE);
 *   };
 *
 *   clib_exports(module) {
 *    .init = init, .deinit = 0,
 *   };
 * ```
 */
#ifndef clib_module_define
#define clib_module_define(module, prototype) prototype(module)
#endif

/**
 * Marks module and variables as initialized or used. Should be called in
 * a module `init()` function.
 */
#ifndef clib_module_init
#define clib_module_init(type, exports)                                        \
  __##type##_clib_module_init = 1;                                             \
  if (0 != (__##type##_clib_module)) {                                         \
    __##type##_clib_module->name = ""#type;                                   \
  }                                                                            \
  /** marks the following variables as "used" */                               \
  (void) (__##type##_clib_module_exports);                                     \
  (void) (__##type##_clib_module_init);                                        \
  (void) (__##type##_clib_module);                                             \
  (void) (exports);                                                            \

#endif

/**
 * Marks module as de-initialized and points module pointer
 * to `0x0` address, the `NULL` pointer. Should be called in a
 * `deinit()` module function.
 */
#ifndef clib_module_deinit
#define clib_module_deinit(type)                                               \
  __##type##_clib_module = 0;                                                  \
  __##type##_clib_module_init = 0;                                             \

#endif

/**
 * Frees a module pointer and calls `deinit()` right before.
 */
#ifndef clib_module_free
#define clib_module_free(module)                                               \
  if (0 != (module)) {                                                         \
    if (0 != (module)->deinit) {                                               \
      (module)->deinit((module));                                              \
    }                                                                          \
    free(module);                                                              \
  }

#endif

#endif
