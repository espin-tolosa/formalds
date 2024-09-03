#ifndef INFLATE_H
#define INFLATE_H

typedef unsigned char       u8_t;
typedef signed   char       i8_t;
typedef unsigned short      u16_t;
typedef signed   short      i16_t;
typedef unsigned int        u32_t;
typedef signed   int        i32_t;
typedef unsigned long long  u64_t;
typedef signed   long long  i64_t;

#define TRUE 1
#define FALSE 0

typedef enum
{
    INFLATE_BTYPE0 = 0, /* 0b00 */
    INFLATE_BTYPE1 = 1, /* 0b01 */
    INFLATE_BTYPE2 = 2, /* 0b10 */
    INFLATE_BTYPE3 = 3, /* 0b11 */
} inflate_BTYPE;

typedef struct
{
    u8_t * data;
    u32_t  len;
    u32_t  size;
} fatu8_t;

#define EOB   0
#define LL0   0x30
#define LL143 0xBF
#define LL144 0x190
#define LL255 0x1FF
#define LL256 0x00
#define LL257 0x01
#define LL279 0x17
#define LL280 0xC0
#define LL287 0x11F

#define MAX_BITS 16
#define MAX_SYMBOLS 65535 + 1 /* u32 */

typedef struct
{
    u8_t * begin;
    u8_t * end;
    i8_t   is_end;
    i8_t   count;
} inflate_stream_t;

#endif