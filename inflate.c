#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <malloc.h>
#include "inflate.h"

u16_t fixed_lenghts [ 288 ] =
{
    [0]   = 8,
    [1]   = 8,
    [2]   = 8,
    [3]   = 8,
    [4]   = 8,
    [5]   = 8,
    [6]   = 8,
    [7]   = 8,
    [8]   = 8,
    [9]   = 8,
    [10]  = 8,
    [11]  = 8,
    [12]  = 8,
    [13]  = 8,
    [14]  = 8,
    [15]  = 8,
    [16]  = 8,
    [17]  = 8,
    [18]  = 8,
    [19]  = 8,
    [20]  = 8,
    [21]  = 8,
    [22]  = 8,
    [23]  = 8,
    [24]  = 8,
    [25]  = 8,
    [26]  = 8,
    [27]  = 8,
    [28]  = 8,
    [29]  = 8,
    [30]  = 8,
    [31]  = 8,
    [32]  = 8,
    [33]  = 8,
    [34]  = 8,
    [35]  = 8,
    [36]  = 8,
    [37]  = 8,
    [38]  = 8,
    [39]  = 8,
    [40]  = 8,
    [41]  = 8,
    [42]  = 8,
    [43]  = 8,
    [44]  = 8,
    [45]  = 8,
    [46]  = 8,
    [47]  = 8,
    [48]  = 8,
    [49]  = 8,
    [50]  = 8,
    [51]  = 8,
    [52]  = 8,
    [53]  = 8,
    [54]  = 8,
    [55]  = 8,
    [56]  = 8,
    [57]  = 8,
    [58]  = 8,
    [59]  = 8,
    [60]  = 8,
    [61]  = 8,
    [62]  = 8,
    [63]  = 8,
    [64]  = 8,
    [65]  = 8,
    [66]  = 8,
    [67]  = 8,
    [68]  = 8,
    [69]  = 8,
    [70]  = 8,
    [71]  = 8,
    [72]  = 8,
    [73]  = 8,
    [74]  = 8,
    [75]  = 8,
    [76]  = 8,
    [77]  = 8,
    [78]  = 8,
    [79]  = 8,
    [80]  = 8,
    [81]  = 8,
    [82]  = 8,
    [83]  = 8,
    [84]  = 8,
    [85]  = 8,
    [86]  = 8,
    [87]  = 8,
    [88]  = 8,
    [89]  = 8,
    [90]  = 8,
    [91]  = 8,
    [92]  = 8,
    [93]  = 8,
    [94]  = 8,
    [95]  = 8,
    [96]  = 8,
    [97]  = 8,
    [98]  = 8,
    [99]  = 8,
    [100] =	8,
    [101] =	8,
    [102] =	8,
    [103] =	8,
    [104] =	8,
    [105] =	8,
    [106] =	8,
    [107] =	8,
    [108] =	8,
    [109] =	8,
    [110] =	8,
    [111] =	8,
    [112] =	8,
    [113] =	8,
    [114] =	8,
    [115] =	8,
    [116] =	8,
    [117] =	8,
    [118] =	8,
    [119] =	8,
    [120] =	8,
    [121] =	8,
    [122] =	8,
    [123] =	8,
    [124] =	8,
    [125] =	8,
    [126] =	8,
    [127] =	8,
    [128] =	8,
    [129] =	8,
    [130] =	8,
    [131] =	8,
    [132] =	8,
    [133] =	8,
    [134] =	8,
    [135] =	8,
    [136] =	8,
    [137] =	8,
    [138] =	8,
    [139] =	8,
    [140] =	8,
    [141] =	8,
    [142] =	8,
    [143] =	8,
    [144] =	9,
    [145] =	9,
    [146] =	9,
    [147] =	9,
    [148] =	9,
    [149] =	9,
    [150] =	9,
    [151] =	9,
    [152] =	9,
    [153] =	9,
    [154] =	9,
    [155] =	9,
    [156] =	9,
    [157] =	9,
    [158] =	9,
    [159] =	9,
    [160] =	9,
    [161] =	9,
    [162] =	9,
    [163] =	9,
    [164] =	9,
    [165] =	9,
    [166] =	9,
    [167] =	9,
    [168] =	9,
    [169] =	9,
    [170] =	9,
    [171] =	9,
    [172] =	9,
    [173] =	9,
    [174] =	9,
    [175] =	9,
    [176] =	9,
    [177] =	9,
    [178] =	9,
    [179] =	9,
    [180] =	9,
    [181] =	9,
    [182] =	9,
    [183] =	9,
    [184] =	9,
    [185] =	9,
    [186] =	9,
    [187] =	9,
    [188] =	9,
    [189] =	9,
    [190] =	9,
    [191] =	9,
    [192] =	9,
    [193] =	9,
    [194] =	9,
    [195] =	9,
    [196] =	9,
    [197] =	9,
    [198] =	9,
    [199] =	9,
    [200] =	9,
    [201] =	9,
    [202] =	9,
    [203] =	9,
    [204] =	9,
    [205] =	9,
    [206] =	9,
    [207] =	9,
    [208] =	9,
    [209] =	9,
    [210] =	9,
    [211] =	9,
    [212] =	9,
    [213] =	9,
    [214] =	9,
    [215] =	9,
    [216] =	9,
    [217] =	9,
    [218] =	9,
    [219] =	9,
    [220] =	9,
    [221] =	9,
    [222] =	9,
    [223] =	9,
    [224] =	9,
    [225] =	9,
    [226] =	9,
    [227] =	9,
    [228] =	9,
    [229] =	9,
    [230] =	9,
    [231] =	9,
    [232] =	9,
    [233] =	9,
    [234] =	9,
    [235] =	9,
    [236] =	9,
    [237] =	9,
    [238] =	9,
    [239] =	9,
    [240] =	9,
    [241] =	9,
    [242] =	9,
    [243] =	9,
    [244] =	9,
    [245] =	9,
    [246] =	9,
    [247] =	9,
    [248] =	9,
    [249] =	9,
    [250] =	9,
    [251] =	9,
    [252] =	9,
    [253] =	9,
    [254] =	9,
    [255] =	9,
    [256] =	7,
    [257] =	7,
    [258] =	7,
    [259] =	7,
    [260] =	7,
    [261] =	7,
    [262] =	7,
    [263] =	7,
    [264] =	7,
    [265] =	7,
    [266] =	7,
    [267] =	7,
    [268] =	7,
    [269] =	7,
    [270] =	7,
    [271] =	7,
    [272] =	7,
    [273] =	7,
    [274] =	7,
    [275] =	7,
    [276] =	7,
    [277] =	7,
    [278] =	7,
    [279] =	7,
    [280] =	8,
    [281] =	8,
    [282] =	8,
    [283] =	8,
    [284] =	8,
    [285] =	8,
    [286] =	8,
    [287] =	8,
};

static i32_t m_inflate_error = 0;

static u32_t extra_lbits[ 286 ] =
{
    [265] = 1,
    [266] = 1,
    [267] = 1,
    [268] = 1,
    [269] = 2,
    [270] = 2,
    [271] = 2,
    [272] = 2,
    [273] = 3,
    [274] = 3,
    [275] = 3,
    [276] = 3,
    [277] = 4,
    [278] = 4,
    [279] = 4,
    [280] = 4,
    [281] = 5,
    [282] = 5,
    [283] = 5,
    [284] = 0,
};

static u32_t extra_dbits[ ] =
{
    [ 0  ] = 0,
    [ 1  ] = 0,
    [ 2  ] = 0,
    [ 3  ] = 0,
    [ 4  ] = 1,
    [ 5  ] = 1,
    [ 6  ] = 2,
    [ 7  ] = 2,
    [ 8  ] = 3,
    [ 9  ] = 3,
    [ 10 ] = 4,
    [ 11 ] = 4,
    [ 12 ] = 5,
    [ 13 ] = 5,
    [ 14 ] = 6,
    [ 15 ] = 6,
    [ 16 ] = 7,
    [ 17 ] = 7,
    [ 18 ] = 8,
    [ 19 ] = 8,
    [ 20 ] = 9,
    [ 21 ] = 9,
    [ 22 ] = 10,
    [ 23 ] = 10,
    [ 24 ] = 11,
    [ 25 ] = 11,
    [ 26 ] = 12,
    [ 27 ] = 12,
    [ 28 ] = 13,
    [ 29 ] = 13,
};

static u32_t extra_lengths[ 286 ] =
{
    [265] = 2,
    [266] = 2,
    [267] = 2,
    [268] = 2,
    [269] = 4,
    [270] = 4,
    [271] = 4,
    [272] = 4,
    [273] = 8,
    [274] = 8,
    [275] = 8,
    [276] = 8,
    [277] = 16,
    [278] = 16,
    [279] = 16,
    [280] = 16,
    [281] = 32,
    [282] = 32,
    [283] = 32,
    [284] = 31,
};

static u32_t lengths[ 286 ] =
{
    [257] = 3,
    [258] = 4,
    [259] = 5,
    [260] = 6,
    [261] = 7,
    [262] = 8,
    [263] = 9,
    [264] = 10,
    [265] = 11,
    [266] = 13,
    [267] = 15,
    [268] = 17,
    [269] = 19,
    [270] = 23,
    [271] = 27,
    [272] = 31,
    [273] = 35,
    [274] = 43,
    [275] = 51,
    [276] = 59,
    [277] = 67,
    [278] = 83,
    [279] = 99,
    [280] = 115,
    [281] = 131,
    [282] = 163,
    [283] = 195,
    [284] = 227,
    [285] = 258,
};

static u16_t *  inflate_create_tree ( u16_t * dst,  u16_t *               bl_count,   u32_t   n   );
static u16_t    inflate_max_bits    (               u16_t *               bl_count,   u32_t   n   );
static u16_t *  inflate_bitlength   (               u16_t *               bl_count,   u32_t   n   );
static i8_t     inflate_read_bit    (               inflate_stream_t *    head                  );

static i32_t read_bfinal ( inflate_stream_t * stream );
static i32_t read_btype ( inflate_stream_t * stream );

static i32_t read_bfinal ( inflate_stream_t * stream )
{
    return ( inflate_read_bit( stream ) );
}

/*
 * read_btype
 *
 * takes advantage of btype 11 to send error in the case stream was finished
 */

static i32_t read_btype ( inflate_stream_t * stream )
{
    i32_t result;

    const i8_t b0 = inflate_read_bit( stream );
    const i8_t b1 = inflate_read_bit( stream ) << 1;

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

/*
 * inflate_read_bit:
 * it is a crucial function and shall be deeply analysed
 */

static i8_t inflate_read_bit( inflate_stream_t * head )
{
    i8_t bit;

    if( head->is_end )
    {
        m_inflate_error = __LINE__;
    }

    if( head->count < 8 ) /* defensive: in fact this condition is always ensured because read_bit is the only one allowed to manipulate head */
    {
        const i8_t bit_mask = 1 << head->count;
        bit = head->begin[0] & bit_mask;
    }

    else
    {
        head->is_end    = TRUE;
        bit             = 0;
    }

    if( head->count < 7 )
    {
        head->count++;
    }

    else
    {
        if( (head->begin + 1) < head->end )
        {
            head->count = 0;
            head->begin++;
        }

        else
        {
            head->is_end = TRUE;
        }
    }

    return ( bit ? 1 : 0 );
}

static u16_t * inflate_bitlength( u16_t * bl_count, u32_t n )
{
    for(u32_t i = 0; i < n; i++)
    {
        bl_count[ i ] = 0;
    }

    for(u32_t i = 0; i < n; i++)
    {
        const u16_t len = bl_count[ i ];
        bl_count [ len ]++;
    }

    return ( bl_count );
}

static u16_t * inflate_create_tree ( u16_t * dst, u16_t * bl_count, u32_t n )
{
    for(u32_t i = 0; i < n; i++)
    {
        bl_count[ i ] = 0;
    }

    for(u32_t i = 0; i < n; i++)
    {
        const u16_t len = bl_count[ i ];
        bl_count [ len ]++;
    }

    /* 2. Find the numerical value of the smallest code for each code length */
    static u16_t next_code[MAX_BITS] = { 0 };
    const u16_t max_bits = inflate_max_bits(bl_count, n);
    u16_t code = 0;
    bl_count[0] = 0;
    for (u32_t bits = 1; bits <= max_bits; bits++)
    {
        code = (code + bl_count[bits-1]) << 1;
        next_code[bits] = code;
    }

    /* 3. Assign numerical values to all codes */
    for (i32_t i = 0; i <= n; i++)
    {
        i32_t len = bl_count[i];
        if (len != 0)
        {
            dst[i] = next_code[len];
            next_code[len]++;
        }
    }

    return ( dst );
}

static u16_t inflate_max_bits( u16_t * bl_count, u32_t n )
{
    u16_t max = 0;

    for( u32_t i = 0; i < n; i++ )
    {
        if( bl_count[i] > max )
        {
            max = bl_count[i];
        }
    }

    return ( max );
}

static u64_t inflate_validate_range( u64_t n, u16_t max, u64_t opt )
{
    u64_t result;

    if( n > max )
    {
        m_inflate_error = __LINE__;
        result = opt;
    }

    else
    {
        result = n;
    }

    return ( result );
}

static u16_t inflate_read_n16code( inflate_stream_t * stream, i8_t n16 )
{
    u16_t result = 0;

    const i8_t n = inflate_validate_range( n16, 16, 0 );

    for( i8_t i = 0; ( i < n ) && !m_inflate_error; i++ )
    {
        result = result << 1 | inflate_read_bit( stream );
    }

    return ( result );
}

static void inflate_read_block_btype0 ( fatu8_t * dst, inflate_stream_t * stream )
{
    (void) inflate_read_n16code( stream, 5 );

    const u16_t block_len   = inflate_read_n16code( stream, 16 );
    const u16_t nblock_len  = inflate_read_n16code( stream, 16 );

    if( block_len != ~nblock_len )
    {
        m_inflate_error = __LINE__;
    }

    else
    {
        /* NOP */
    }

    memcpy( dst->data, stream->begin, block_len );

    stream->begin += block_len;
    dst->len++;
}

static i32_t inflate_symbol_in_range( u16_t symbol, u16_t min, u16_t max )
{
    return ( min <= symbol && symbol <= max );
}

void dump_stream( inflate_stream_t * stream )
{
    int cbyte = 0;
    while(!stream->is_end)
    {
        printf("%u", inflate_read_n16code(stream,1));
        if( cbyte < 8 )
        {
            cbyte++;
        }
        else
        {
            cbyte = 0;
            printf("\n");
        }
    }
}

static void inflate_read_block_btype2( fatu8_t * dst, inflate_stream_t * stream )
{
    u16_t hlit  = 257 + inflate_read_n16code( stream, 5 );
    u16_t hdist = 1 + inflate_read_n16code( stream, 5 );
    u16_t hclen = 4 + inflate_read_n16code( stream, 4 ); //limited to 19

    printf("HLIT = %u, HDIST = %u, HCLEN = %u\n", hlit, hdist, hclen);

    if (hlit > 286 || hdist > 30 || hclen > 19) /* Mark Adler suggests early rejection https://github.com/madler/zlib/issues/82 */
    {
		m_inflate_error = __LINE__;
	}

	static const u16_t cl_index_map[19] =
    {
		16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15,
	};

    /* read 3·(4 + HCLEN) bits of 3-bit symbols */
    u16_t bl_count[ 19 ] = { 0 };
    for (u16_t i = 0; ( i < hclen ) && !m_inflate_error; ++i)
    {
        const u16_t index = cl_index_map[i];

        bl_count[ index ] = inflate_read_n16code( stream, 3 );
	}

    for(int i = 0 ; i < 19; i++)
    {
        printf("SYMBOL: %3hu => CODE: %hu\n", i, bl_count[i]);
    }

    u16_t bitcodes[ MAX_SYMBOLS ] = { 0 };

    inflate_create_tree( bitcodes, bl_count, hclen + 4 );

//    inflate_bitlength( lengths, 19)

	/* Build code length tree (in literal/length tree to save space) */
//	res = tinf_build_tree(lt, lengths, 19);


}

static void inflate_read_block_btype1( fatu8_t * dst, inflate_stream_t * stream )
{
    u32_t block_len = 0;

    u32_t length = 0;

    u32_t dextra = 0;
    u32_t distance = 0;

    u16_t symbol = inflate_read_n16code( stream, 7 );

    while( symbol != EOB && !m_inflate_error )
    {
        if( inflate_symbol_in_range( symbol, LL257, LL279 ) ) // NOT TESTED (after change LL256 -> LL257)
        { //TODO: good to extract a method to obtain backward_length
            length = lengths[ 256 + symbol ];

            const u16_t ebits = extra_lbits[ 256 + symbol ];
            length = length + inflate_read_n16code( stream, ebits );

            // read backward distance
            symbol = inflate_read_n16code( stream, 5 );
            distance = 1 + symbol;

            const u16_t dbits = extra_dbits[ symbol ];
            symbol = inflate_read_n16code( stream, dbits );
            distance = distance + symbol;

            for( u32_t i = 0; i < length; i++ )
            {
                dst->data[ dst->len ] = dst->data[ dst->len - distance ];
                dst->len++;
            }

            // copy length + lextra bits from -distance - dextra (not computed yet)
        }

        else
        {
            symbol = symbol << 1 | inflate_read_n16code( stream, 1 );

            if( inflate_symbol_in_range( symbol, LL0, LL143 ) ) // TESTED
            {
                dst->data[ dst->len ] = (u8_t) ( symbol - 48 );
                dst->len++;
                length = 0;
            }

            else if( inflate_symbol_in_range( symbol, LL280, LL287 ) ) // PENDING TO TEST
            {
                // parse LEN 8 bits 280 - 287, 11000000 = 192
                length = lengths[ 280 + symbol ];

                const u16_t ebits = extra_lbits[ 280 + symbol ];
                length = length + inflate_read_n16code( stream, ebits );

                // read backward distance
                symbol = inflate_read_n16code( stream, 5 );
                distance = 1 + symbol;

                const u16_t dbits = extra_dbits[ symbol ];
                distance = distance + inflate_read_n16code( stream, dbits );

                for( u32_t i = 0; i < length; i++ )
                {
                    dst->data[ dst->len ] = dst->data[ dst->len - distance ];
                    dst->len++;
                }
                // copy length + lextra bits from -distance - dextra (not computed yet)
            }

            else
            {
                symbol = symbol << 1 | inflate_read_n16code( stream, 1 );

                if( inflate_symbol_in_range( symbol, LL144, LL255 ) ) // PENDING TO TEST
                {
                    dst->data[ dst->len ] = (u8_t) ( symbol - 400 );
                    dst->len++;
                    length = 0;
                }

                else
                {
                    m_inflate_error = __LINE__;
                }
            }
        }

        symbol = inflate_read_n16code( stream, 7 );
    }
}

extern u32_t inflate_blocks ( fatu8_t * dst, fatu8_t * src )
{
    inflate_stream_t stream = { .begin = src->data, .end = src->data + src->len, .is_end = 0, .count = 0 };

    while( (( read_bfinal( &stream ) ) != 0 ) && !m_inflate_error )
    {
        switch ( read_btype( &stream ) )
        {
            case ( INFLATE_BTYPE0 ):
            {
                inflate_read_block_btype0( dst, &stream );
                break;
            }

            case ( INFLATE_BTYPE1 ):
            {
                inflate_read_block_btype1( dst, &stream );
                break;
            }

            case ( INFLATE_BTYPE2 ):
            {
                inflate_read_block_btype2( dst, &stream );
                break;
            }

            default:
            {
                m_inflate_error = __LINE__;
            }
        }
    }
}
