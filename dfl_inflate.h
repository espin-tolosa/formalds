#ifndef DFL_INFLATE_H
#define DFL_INFLATE_H

#include "set_types.h"

typedef u16_t dfl_code_t; /* Huffman codes lenght is limited to 15 bits */

#define DFL_MAX_CODE_LEN 0x7FFF /* biggest value holded by type deflate_code_t when all 15 bits are set to 1 */

typedef enum
{
    INFLATE_BTYPE0 = 0, /* 0b00 */
    INFLATE_BTYPE1 = 1, /* 0b01 */
    INFLATE_BTYPE2 = 2, /* 0b10 */
    INFLATE_BTYPE3 = 3, /* 0b11 */
} dfl_BTYPE;

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

#endif/*DFL_INFLATE_H*/