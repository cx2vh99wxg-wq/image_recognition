#ifndef STUB_PRELUDE_H
#define STUB_PRELUDE_H
#include <unistd.h>
extern unsigned getuid(void);   /* MinGW 的 unistd.h 不声明它，glibc 的会 —— 补齐以复现板端语义 */
#endif
