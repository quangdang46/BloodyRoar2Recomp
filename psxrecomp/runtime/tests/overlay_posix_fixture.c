#include <stdint.h>

#if defined(__GNUC__)
#define EXPORT __attribute__((visibility("default")))
#else
#define EXPORT
#endif

/* overlay abi: return 0x12345678; */
EXPORT int overlay_abi(void) { return 0x12345678; }
/* overlay init: (void)callbacks; */
EXPORT void overlay_init(const void *callbacks) { (void)callbacks; }
/* overlay flush cycles */
EXPORT void overlay_flush_cycles(void) {}
/* func 80012345: return 0xC0DEF00Du; */
EXPORT uint32_t func_80012345(void) { return 0xC0DEF00Du; }
