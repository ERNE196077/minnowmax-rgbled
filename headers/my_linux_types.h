#ifndef MY_LINUX_TYPES_H_INCLUDED
#define MY_LINUX_TYPES_H_INCLUDED

#ifdef __linux__
# include "linux/types.h"
#else
# include <stdint.h>
typedef int32_t __s32;
typedef u_int8_t __u8;
typedef u_int16_t __u16;
typedef u_int64_t __u64;
typedef u_int32_t __u32;
typedef u_int32_t kernel_ulong_t;

#endif

#endif
