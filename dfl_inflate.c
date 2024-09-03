#include "set_types.h"
#include "dfl_inflate.h"
#include "dfl_stream.h"

/*
 * init stream data object
 *              do
 * dfl_bfinal       read block header from input stream
 * dfl_btype
 * dfl_btype0       if stored with no compression
 *                      skip any remaining bits in current partially processed byte
 *                      read LEN and NLEN (see next section)
 *                      copy LEN bytes of data to output
 *
 *                  otherwise
 *
 * dfl_dynamic          if compressed with dynamic Huffman codes
 *                          read representation of code trees
 *
 * dfl_inflate          loop until end of block code recognized
 *                          decode literal/length value from input stream
 *                          if value < 256
 *                              copy value (literal byte) to output stream
 *                          otherwise
 *                              if value = end of block (256)
 *                                  break from loop
 *                              otherwise (value = 257..285)
 *                                  decode distance from input stream
 *                                  move backwards distance bytes in the output
 *                                  stream, and copy length bytes from this
 *                                  position to the output stream.
 *                      end loop
 *              while not last block
 *
 * 1. read bfinal, btype
 * 2. branch (btype)
 * 3.   case 0:
 *          skip remaining first 5 bits
 *          read block header |LEN|~LEN|
 *          copy LEN bytes from src to dst
 *          update dst len in LEN bytes
 *      case 1:
 *          parse stream codes
 *      case 2:
 *          2.1 read HLIT, HDIST, HCLEN
 *          2.2 allocate following (4 + HCLEN) · 3 bits in the CL Code Lenghts in the order:
 *              16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
 *          2.3 build Huffman tree of CL Code Lengths
 *          2.4 decode (257 + HLIT + 1 + HDIST) bits using Huffman tree of CL Code Lengths
 *          2.5
 *
 *
 * 2.4 code meaning:
 *      1-15:
 */



static u32_t m_inflate_error = 0;

static i32_t dfl_bfinal ( dfl_stream_t * stream )
{
    return ( dfl_stream_read_bit( stream ) );
}

/*
 * read_btype
 *
 * takes advantage of btype 11 to send error in the case stream was finished
 */

static i32_t dfl_btype ( dfl_stream_t * stream )
{
    i32_t result;

    const i8_t b0 = dfl_stream_read_bit( stream );
    const i8_t b1 = dfl_stream_read_bit( stream ) << 1;

    if( stream->is_end )
    {
        result = INFLATE_BTYPE3;
    }

    else
    {
        result = b1 | b0;
    }

    return ( result );
}

static void dfl_dynamic( fatu8_t * dst, dfl_stream_t * stream )
{
    u16_t hlit  = 257   + dfl_stream_read_ncode( stream, 5 );
    u16_t hdist = 1     + dfl_stream_read_ncode( stream, 5 );
    u16_t hclen = 4     + dfl_stream_read_ncode( stream, 4 ); // range (0...19)

    if (hlit > 286 || hdist > 30 || hclen > 19) /* Mark Adler suggests early rejection https://github.com/madler/zlib/issues/82 */
    {
		m_inflate_error = __LINE__;
	}

	static const u16_t cl_index_map[19] =
    {
		16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15,
	};
}

extern u32_t dfl_inflate_blocks ( fatu8_t * dst, fatu8_t * src )
{
    dfl_stream_t palloc_stream;
    dfl_stream_t * stream = dfl_stream_init( &palloc_stream, src->data, src->len );

    while( (( dfl_bfinal( stream ) ) != 0 ) && !m_inflate_error )
    {
        switch ( dfl_btype( stream ) )
        {
            case ( INFLATE_BTYPE0 ):
            {
                //inflate_read_block_btype0( dst, &stream );
                int a = 0;
                break;
            }

            case ( INFLATE_BTYPE1 ):
            {
                int a = 0;
                //inflate_read_block_btype1( dst, &stream );
                break;
            }

            case ( INFLATE_BTYPE2 ):
            {
                dfl_dynamic( dst, stream );
                break;
            }

            default:
            {
                m_inflate_error = __LINE__;
            }
        }
    }

    return ( m_inflate_error );
}
