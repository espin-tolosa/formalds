#ifndef DFL_TREE_H
#define DFL_TREE_H

#include "set_types.h"
#include "dfl_inflate.h"

typedef struct
{
    dfl_code_t * count;  /* number of codes for each codelens (0,...,codelen) */
    dfl_code_t * codes;  /* sorted values for each code */
    dfl_code_t   nbits;  /* max number of bits needed to encode a code, corresponds with the length of count */
    u32_t        len;    /* max number of codes, corresponds with the length of codes */
} dfl_tree_t;

#endif