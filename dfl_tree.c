#include "set_types.h"
#include "dfl_inflate.h"
#include "dfl_stream.h"
#include "dfl_tree.h"

static dfl_code_t * dfl_tree_create ( dfl_tree_t * dst, dfl_code_t * bl_count, dfl_code_t nbits )
{
    dst->nbits = nbits;

    for(u32_t i = 0; i < nbits; i++)
    {
        const dfl_code_t n = dfl_validate_nbits(bl_count[ i ], 16, 0);

        dst->codes = n;
    }

    /* 2. Find the numerical value of the smallest code for each code length */
    dfl_code_t code = 0;
    dst->codes[0]   = 0;
    u16_t next_code[MAX_BITS] = { 0 };

    for (u32_t bits = 1; bits <= nbits; bits++)
    {
        code = (code + bl_count[bits-1]) << 1;
        next_code[bits] = code;
    }

    /* 3. Assign numerical values to all codes */
    for (i32_t i = 0; i <= nbits; i++)
    {
        i32_t len = bl_count[i];
        if (len != 0)
        {
            dst->codes[i] = next_code[len];
            next_code[len]++;
        }
    }

    return ( dst );
}
