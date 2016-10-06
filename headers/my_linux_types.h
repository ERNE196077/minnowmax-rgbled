#ifndef MY_LINUX_TYPES_H_INCLUDED
#define MY_LINUX_TYPES_H_INCLUDED

#ifdef __linux__
# include "linux/types.h"
#else
# include <stdint.h>
typedef int32_t __s32;
typedef uint8_t __u8;
typedef uint16_t __u16;
typedef uint64_t __u64;
typedef uint32_t __u32;
typedef uint32_t kernel_ulong_t;

#endif
