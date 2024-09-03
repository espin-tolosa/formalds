#include "set_types.h"
#include "dfl_stream.h"
#include "dfl_inflate.h"

static i32_t m_inflate_error = 0;

static u8_t     dfl_stream_peek_bit         ( dfl_stream_t * stream );
static void     dfl_stream_move_head        ( dfl_stream_t * stream );
static u8_t     dfl_stream_peek_bit         ( dfl_stream_t * stream );
static void     dfl_stream_set_bit          ( dfl_stream_t * stream, i8_t bit );

extern dfl_stream_t * dfl_stream_init( dfl_stream_t * this, u8_t * begin, u32_t size )
{
    this->head   = begin;
    this->end    = begin + size;
    this->is_end = FALSE;
    this->nbyte  = 0;
    this->nbit   = 0;

    return ( this );
};

static void dfl_stream_move_head ( dfl_stream_t * stream )
{
    if( stream->nbit < 7 )
    {
        stream->nbit++; /* max possible value after the increase is 7 */
    }

    else
    {
        if( &stream->head[+1] >= stream->end ) /* the use of greater cond. is defensive programming */
        {
            stream->is_end = TRUE;
            stream->nbit   = 7;
            stream->head   = &stream->end[-1];
        }

        else
        {
            stream->nbit = 0;
            stream->head++;
        }
    }
}

extern bool_t dfl_stream_is_end( dfl_stream_t * stream )
{
    return ( stream->is_end );
}

static u8_t dfl_stream_peek_bit( dfl_stream_t * stream )
{
    static const u8_t bit_masks[ 8 ] = { 1, 2, 4, 8, 16, 32, 64, 128 };

    return ( ( stream->head[ 0 ] & bit_masks[ stream->nbit ] ) >> stream->nbit );
}

static void dfl_stream_set_bit( dfl_stream_t * stream, i8_t bit )
{
    stream->head[ 0 ] = stream->head[ 0 ] | ( (bit & 1u) << stream->nbit ) ;
}
/*
 * inflate_read_bit:
 * it is a crucial function and shall be deeply analysed
 */

extern u8_t dfl_stream_read_bit( dfl_stream_t * stream )
{
    u8_t ret = 0;

    if( stream->is_end ) /* sets the deflate local error to true to indicate attempting to read a finished stream */
    {
        stream->is_err = TRUE;
    }

    else
    {
        ret = dfl_stream_peek_bit( stream );

        dfl_stream_move_head( stream );
    }

    return ( (u8_t) ret );
}

extern void dfl_stream_write_bit( dfl_stream_t * stream, u8_t bit )
{
    if( stream->is_end ) /* sets the deflate local error to true to indicate attempting to read a finished stream */
    {
        stream->is_err = TRUE;
    }

    else
    {
        dfl_stream_set_bit( stream, bit );
        dfl_stream_move_head( stream );
    }
}

extern dfl_stream_t * dfl_stream_reset ( dfl_stream_t * stream )
{
    stream->head    = stream->head - stream->nbyte;
    stream->nbyte   = 0;
    stream->nbit    = 0;
    stream->is_end  = FALSE;
    stream->is_err  = FALSE;

    return ( stream );
}

extern dfl_code_t dfl_stream_read_ncode( dfl_stream_t * stream, dfl_code_t n16 )
{
    dfl_code_t ret = 0u;

    const dfl_code_t n = dfl_validate_nbits( n16, 16u, 0u );

    for( dfl_code_t i = 0u; ( i < n ) && !m_inflate_error; i++ )
    {
        ret = ( ret << 1u ) | dfl_stream_read_bit( stream );
    }

    return ( ret );
}

extern u64_t dfl_validate_nbits( u64_t n, u16_t max, u64_t opt )
{
    u64_t result;

    if( n > max )
    {
        m_inflate_error = __LINE__;
        result          = opt;
    }

    else
    {
        result          = n;
    }

    return ( result );
}
