#ifndef CLIB_MODULE_DEF_H
#define CLIB_MODULE_DEF_H

#define defaults(...) clib_module_defaults(__VA_ARGS__)
#define require(...) clib_module_require(__VA_ARGS__)
#define exports(...) clib_module_exports(__VA_ARGS__)
#define module(...) clib_module(__VA_ARGS__)
#define define(...) clib_module_define(__VA_ARGS__)

#endif
