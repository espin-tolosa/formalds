#ifndef DFL_STREAM_H
#define DFL_STREAM_H

#include "set_types.h"
#include "dfl_inflate.h"

typedef struct
{
    u8_t *    head  ; /* points to actual byte being read, the head pointer is moving forward while the condition: "head < end" is satisfied */
    u8_t *    end   ; /* points to the next byte past of the buffer pointed by head */
    u32_t     nbyte ; /* counts the bytes advanced by the head pointer */
    u8_t      nbit  ; /* records which is the next bit to read among n-bit (0...7) */
    bool_t    is_end; /* sets to TRUE whenever the last bit of the stream is read */
    bool_t    is_err; /* sets to TRUE when there is one attempt to read a bit when is_end is TRUE */
} dfl_stream_t;

extern dfl_stream_t *   dfl_stream_init         ( dfl_stream_t * stream, u8_t * begin, u32_t size );
extern bool_t           dfl_stream_is_end       ( dfl_stream_t * stream );
extern u8_t             dfl_stream_read_bit     ( dfl_stream_t * stream );
extern dfl_stream_t *   dfl_stream_reset        ( dfl_stream_t * stream );
extern dfl_code_t       dfl_stream_read_ncode   ( dfl_stream_t * stream, dfl_code_t n16 );
extern void             dfl_stream_write_bit    ( dfl_stream_t * stream, u8_t       bit );

extern u64_t            dfl_validate_nbits      ( u64_t n, u16_t max, u64_t opt );

#endif/*DFL_STREAM_H*/