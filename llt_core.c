#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "llt_types.h"

extern LLT_TUID llt_find_tuid( const LLT_CSTRING * src, LLT_COFF from )
{
    LLT_TUID ret;

    if( src->clen < ( sizeof("/*") - 1LLu ) )
    {
        ret.coff    = LLT_NULL_COFF;
        ret.type    = LLT_TUID_TYPE_NULL;
        ret.value   = LLT_NULL_TUID_VALUE;
    }

    else
    {
        ret.coff    = LLT_NULL_COFF;
        ret.type    = LLT_TUID_TYPE_NULL;
        ret.value   = LLT_NULL_TUID_VALUE;

        LLT_CLEN i  = from;

        while( ( i < src->csz ) && ( ret.coff == LLT_NULL_COFF ) )
        {
            LLT_CSTRING src_view = *src;

            src_view.string = &src->string[ i ];
            src_view.clen   = src_view.clen - i;
            src_view.csz    = 1u + src_view.csz;

            if( src->string[ i ] == '/' )
            {
                if( strncmp( &src->string[ i ], LLT_TUID_OPEN, LLT_TUID_OPEN_LEN ) == 0 )
                {
                    ret.coff    = i + LLT_TUID_OPEN_LEN;
                    ret.type    = LLT_TUID_TYPE_OPEN;
                    ret.value   = +strtol( &src->string[ ret.coff ], NULL, 10 );
                }

                else if( strncmp( &src->string[ i ], LLT_TUID_CLOSE, LLT_TUID_CLOSE_LEN ) == 0 )
                {
                    ret.coff    = i + LLT_TUID_CLOSE_LEN;
                    ret.type    = LLT_TUID_TYPE_CLOSE;
                    ret.value   = -strtol( &src->string[ ret.coff ], NULL, 10 );
                }

                else
                {
                    /* NOP: continue parsing */
                }
            }

            else
            {
                /* NOP: continue parsing */
            }

            ++i;
        }
    }

    return ( ret );
}

LLT_CSTRING llt_load_file_as_cstring ( const char * path )
{
    LLT_CSTRING result = { 0 };

    if ( path == NULL )
    {
        return result;
    }

    FILE * f = fopen( path, "rb" );

    if ( f == NULL )
    {
        return result;
    }

    if ( fseek( f, 0, SEEK_END ) != 0 )
    {
        fclose( f );
        return result;
    }

    long sz = ftell( f );

    if ( sz < 0 )
    {
        fclose( f );
        return result;
    }

    rewind( f );

    char * buf = (char *) malloc( (size_t)sz + 1u );
    if ( buf == NULL )
    {
        fclose( f );
        return result;
    }

    size_t rd = fread( buf, 1u, (size_t)sz, f );

    fclose( f );

    if ( rd != (size_t)sz )
    {
        free( buf );
        return result;
    }

    buf[ sz ] = '\0';

    result.string = buf;
    result.clen   = (size_t)sz;
    result.csz    = (size_t)sz + 1u;

    return result;
}

extern int main_check( const char * fname )
{
    static LLT_TUID tuid_list[ 10000 ] = { 0 };
    static int tuid_len = 0;

    LLT_CSTRING csrc        = llt_load_file_as_cstring( fname );

    LLT_TUID id_offset_prev = { .coff = 0, /* */ };
    LLT_TUID id_offset_curr = { .coff = 0, /* */ };

    LLT_COFF id_check = 0;

    do
    {
        id_offset_prev = id_offset_curr;

        id_offset_curr = llt_find_tuid( &csrc, id_offset_prev.coff );

        if( id_offset_curr.value != LLT_NULL_TUID_VALUE )
        {
            tuid_list[ tuid_len ] = id_offset_curr;
            tuid_len++;
        }

        else
        {
            /* NOP: do not insert null tuid values */
        }

        id_check += id_offset_curr.value;
    }

    while ( ( id_offset_prev.coff < id_offset_curr.coff ) && ( id_offset_curr.coff != LLT_NULL_COFF ) );

    int merges = 0;

    do
    {
        merges = 0;

        for( int i = 0; i < ( tuid_len - 1 ); ++i )
        {
            const int tuid_diff = ( tuid_list[ i ].value + tuid_list[ i + 1 ].value );

            if( tuid_diff == 0 )
            {
                merges++;

                for( int j = i; j < ( tuid_len - 2 ); ++j )
                {
                    tuid_list[ j ] = tuid_list[ j + 2 ];
                }

                tuid_len -= 2;

                continue;
            }

            else
            {
                /* NOP */
            }
        }
    }

    while( ( merges != 0 ) && ( tuid_len != 0 ) );

    for( int i = 0; i < tuid_len; ++i )
    {
        printf( "[ %3d ] = %+d\n", i, tuid_list[ i ].value );
    }

    return ( id_check );
}

int main( int argc, char ** argv )
{
    if( argc > 1 )
    {
        const int balance = main_check( argv[ 1 ] );

        if( balance == 0 )
        {
            printf( "Test PASSED: %s (LLTraced is balanced)\n", argv[ 1 ] );
        }

        else
        {
            printf( "Test FAILED: %d\n", balance );
        }

    }


    return ( 0 );
}
