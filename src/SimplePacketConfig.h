#ifndef __SimplePacketConfig_H__
#define __SimplePacketConfig_H__

#include <Arduino.h>

/* When the UNIVERSAL_CPP macro is enabled, the types used will be the
minimum size according to the C++ standard. It should be used if you
want to communicate CPUs with different architectures. */
//#define UNIVERSAL_CPP 

/* If the CUSTOM_TYPES macro is enabled, the types used will be the size of
what you define. */
//#define CUSTOM_TYPES

#if defined(UNIVERSAL_CPP)
#define SP_BOOL uint8_t
#define SP_CHAR int8_t
#define SP_UCHAR SP_BOOL
#define SP_INT int16_t
#define SP_UINT uint16_t
#define SP_LONG int32_t
#define SP_ULONG uint32_t
#define SP_DOUBLE float

#elif defined(CUSTOM_TYPES)
#define SP_BOOL uint8_t
#define SP_CHAR int8_t
#define SP_UCHAR SP_BOOL
#define SP_INT int16_t
#define SP_UINT uint16_t
#define SP_LONG int32_t
#define SP_ULONG uint32_t
#define SP_DOUBLE float

#else
#define SP_BOOL bool
#define SP_CHAR char
#define SP_UCHAR unsigned char
#define SP_INT int
#define SP_UINT unsigned int
#define SP_LONG long
#define SP_ULONG unsigned long
#define SP_DOUBLE double

#endif

#endif  /* __SimplePacketConfig_H__ */