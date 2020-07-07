#ifndef __ERROR_HH__
#define __ERROR_HH__

typedef int gcc_error_t;

static thread_local int gcc_errno;

enum GCC_ERROR {
    GCC_SUCCESS = 0,
    GCC_INVALID_VALUE = -1,
    GCC_OUT_OF_MEMORY = -2,
};

static const char *__str_error[] = {
    "Success",
    "Invalid value",
    "Out of memory",
};

static inline const char *gcc_error(gcc_error_t ret)
{
    if (ret > GCC_SUCCESS || ret < GCC_OUT_OF_MEMORY)
        return "Invalid error number";
    return __str_error[-ret];
}

#endif /* __ERROR_HH__ */
