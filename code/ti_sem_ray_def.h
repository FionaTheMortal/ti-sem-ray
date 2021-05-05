#ifndef TI_SEM_RAY_DEF_H
#define TI_SEM_RAY_DEF_H

#include <stddef.h>

#if defined(_MSC_VER)
#if _MSC_VER < 1300
typedef unsigned char     u8;
typedef   signed char     s8;
typedef unsigned short   u16;
typedef   signed short   s16;
typedef unsigned int     u32;
typedef   signed int     s32;
#else
typedef unsigned __int8   u8;
typedef   signed __int8   s8;
typedef unsigned __int16 u16;
typedef   signed __int16 s16;
typedef unsigned __int32 u32;
typedef   signed __int32 s32;
#endif
typedef unsigned __int64 u64;
typedef   signed __int64 s64;
#else
#include <stdint.h>

typedef uint8_t   u8;
typedef  int8_t   s8;
typedef uint16_t u16;
typedef  int16_t s16;
typedef uint32_t u32;
typedef  int32_t s32;
typedef uint64_t u64;
typedef  int64_t s64;
#endif

#define SOURCE_LINE __LINE__
#define SOURCE_FILE __FILE__

#define STATIC_ASSERT_3(expression, message) typedef char message[(!!(expression))*2-1]
#define STATIC_ASSERT_2(expression, line)    STATIC_ASSERT_3(expression, static_assertion_at_line_##Line)
#define STATIC_ASSERT_1(expression, line)    STATIC_ASSERT_2(expression, line)
#define STATIC_ASSERT(expression)            STATIC_ASSERT_1(expression, SOURCE_LINE)

STATIC_ASSERT(sizeof(u8)  == sizeof(s8));
STATIC_ASSERT(sizeof(u16) == sizeof(s16));
STATIC_ASSERT(sizeof(u32) == sizeof(s32));
STATIC_ASSERT(sizeof(u64) == sizeof(s64));

STATIC_ASSERT(sizeof(u8)  == 1);
STATIC_ASSERT(sizeof(u16) == 2);
STATIC_ASSERT(sizeof(u32) == 4);
STATIC_ASSERT(sizeof(u64) == 8);

typedef  s8  b8;
typedef s16 b16;
typedef s32 b32;

typedef char c8;

#define U8_MIN 0u
#define U8_MAX 0xffu
#define S8_MIN (-0x7f - 1)
#define S8_MAX 0x7f

#define U16_MIN 0u
#define U16_MAX 0xffffu
#define S16_MIN (-0x7fff - 1)
#define S16_MAX 0x7fff

#define U32_MIN 0u
#define U32_MAX 0xffffffffu
#define S32_MIN (-0x7fffffff - 1)
#define S32_MAX 0x7fffffff

#define U64_MIN 0ull
#define U64_MAX 0xffffffffffffffffull
#define S64_MIN (-0x7fffffffffffffffll - 1)
#define S64_MAX 0x7fffffffffffffffll

#define F32_INF (INFINITY)
#define F32_NAN (NAN)

typedef float  f32;
typedef double f64;

STATIC_ASSERT(sizeof(f32) == 4);
STATIC_ASSERT(sizeof(f64) == 8);

typedef size_t    umi;
typedef ptrdiff_t smi;

#define global   static
#define local    static
#define internal static

#define SizeOf(type)           ((smi)sizeof(type))
#define ArrayCount(Array)      ((SizeOf(Array) / SizeOf((Array)[0])) / ((smi)!(SizeOf(Array) % SizeOf((Array)[0]))))
#define OffsetOf(type, Member) ((smi)&(((type *)0)->Member))

#define Align(Value, Alignment) (((Value) + ((Alignment) - 1)) & (~((Alignment) - 1)))

#if defined(_MSC_VER)
#define CPU_IS_LITTLE_ENDIAN() (1)
#elif defined(__GNUC__)
#ifdef __BYTE_ORDER__
#define CPU_IS_LITTLE_ENDIAN() (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#endif
#else
#error ti_sem_ray_def.h - Compiler unknown or not supported
#endif

#endif