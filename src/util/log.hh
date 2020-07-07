#ifndef __LOG_HH__
#define __LOG_HH__

#include <cstdio>
#include <cstdarg>
#include <string>

#define LOG_LEVEL_ERROR "\x1b[31merror\x1b[0m"
#define LOG_LEVEL_WARN  "\x1b[33mwarn\x1b[0m"
#define LOG_LEVEL_INFO  "\x1b[34minfo\x1b[0m"
#define LOG_LEVEL_DEBUG "debug"

#define __gcc_debug(level, fmt, ...) \
	fprintf(stderr, "%s:%s:%s: " fmt, level, \
            CHANNEL, __func__, ##__VA_ARGS__)

#define __gcc_debug_once(level, fmt, ...) \
    do { \
        static int __dbg_once_##level; \
        if (!__dbg_once_##level) { \
            __gcc_debug(level, fmt, ##__VA_ARGS__); \
            __dbg_once_##level = 1; \
        } \
    } while (0)

#define DEBUG(fmt,  ...) __gcc_debug(LOG_LEVEL_DEBUG,  fmt, ##__VA_ARGS__)
#define ERROR(fmt,  ...) __gcc_debug(LOG_LEVEL_ERROR, fmt, ##__VA_ARGS__)
#define WARN(fmt,   ...) __gcc_debug(LOG_LEVEL_WARN,  fmt, ##__VA_ARGS__)
#define INFO(fmt,   ...) __gcc_debug(LOG_LEVEL_INFO,  fmt, ##__VA_ARGS__)

#define DEBUG_ONCE(fmt,  ...) __gcc_debug_once(LOG_LEVEL_DEBUG,  fmt, ##__VA_ARGS__)
#define ERROR_ONCE(fmt,  ...) __gcc_debug_once(LOG_LEVEL_ERROR, fmt, ##__VA_ARGS__)
#define WARN_ONCE(fmt,   ...) __gcc_debug_once(LOG_LEVEL_WARN,  fmt, ##__VA_ARGS__)
#define INFO_ONCE(fmt,   ...) __gcc_debug_once(LOG_LEVEL_INFO,  fmt, ##__VA_ARGS__)

#endif /* __LOG_HH__ */
