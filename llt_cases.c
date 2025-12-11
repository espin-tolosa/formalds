#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "llt_types.h"

extern int llt_use_case_find_trace( const char * path, int id )
{
    //TODO: parse path, if it is a FILE go straight, if is FOLDER, scan it

    int ret;

    LLT_TUID found_id = llt_find_tuid_by_value( path, id );

    if( found_id.value != LLT_NULL_TUID_VALUE )
    {
        printf( "%s%d: %s\n", LLT_TUID_PREFIX, found_id.value, path );

        ret = EXIT_SUCCESS;
    }

    else
    {
        /* NOP: do not print nothing */

        ret = EXIT_FAILURE;
    }

    return ( ret );
}

extern int llt_use_case_trace_check( const char * path )
{
    int ret;

    const int balance = llt_tuid_check( path );

    if( balance == 0 )
    {
        printf( "%s is balanced\n", path );

        ret = EXIT_SUCCESS;
    }

    else
    {
        printf( "[ERROR] Traces are not balanced by %d\n", balance );

        ret = EXIT_FAILURE;
    }

    return ( ret );
}

/*
 * USE CASE: erase TRACE
 */

extern int llt_use_case_erase_trace( const char * path, int id )
{
    int ret;

    if( path == NULL )
    {
        return ( EXIT_FAILURE );
    }

    LLT_CSTRING csrc = llt_load_file_as_cstring( path );

    if( csrc.string == NULL )
    {
        return ( EXIT_FAILURE );
    }

    /* ------------------------------------------------------------------
     * Prepare search strings for both open and close traces
     * ------------------------------------------------------------------ */

    char open_tag[ 128 ];
    char close_tag[ 128 ];

    int open_len;
    int close_len;

    open_len  = snprintf( open_tag  , sizeof( open_tag  ) , "/* >> " LLT_TUID_PREFIX "%d */" , id );
    close_len = snprintf( close_tag , sizeof( close_tag ) , "/* << " LLT_TUID_PREFIX "%d */" , id );

    if( ( open_len <= 0 ) || ( close_len <= 0 ) )
    {
        return ( EXIT_FAILURE );
    }

    /* ------------------------------------------------------------------
     * Allocate output buffer for first pass
     * ------------------------------------------------------------------ */

    char * out_buf;

    out_buf = (char *) malloc( csrc.csz );
    if( out_buf == NULL )
    {
        free( (void*) csrc.string );
        return ( EXIT_FAILURE );
    }

    LLT_CLEN r = 0;
    LLT_CLEN w = 0;

    /* ------------------------------------------------------------------
     * First pass: remove trace markers
     * ------------------------------------------------------------------ */

    while( r < csrc.clen )
    {
        int is_open;
        int is_close;

        is_open  = 0;
        is_close = 0;

        if( ( r + (LLT_CLEN) open_len ) <= csrc.clen )
        {
            if( strncmp( &csrc.string[ r ] , open_tag , (size_t) open_len ) == 0 )
            {
                is_open = 1;
            }
        }

        if( is_open == 0 )
        {
            if( ( r + (LLT_CLEN) close_len ) <= csrc.clen )
            {
                if( strncmp( &csrc.string[ r ] , close_tag , (size_t) close_len ) == 0 )
                {
                    is_close = 1;
                }
            }
        }

        if( is_open != 0 )
        {
            r += (LLT_CLEN) open_len;
            continue;
        }

        if( is_close != 0 )
        {
            r += (LLT_CLEN) close_len;
            continue;
        }

        out_buf[ w ] = csrc.string[ r ];
        w++;
        r++;
    }

    out_buf[ w ] = '\0';

    /* ------------------------------------------------------------------
     * Second pass:
     * Remove whitespace-only lines, but KEEP pure empty lines ("\n")
     * ------------------------------------------------------------------ */

    char * clean_buf;

    clean_buf = (char *) malloc( csrc.csz );
    if( clean_buf == NULL )
    {
        free( out_buf );
        free( (void*) csrc.string );
        return ( EXIT_FAILURE );
    }

    LLT_CLEN rr = 0;
    LLT_CLEN ww = 0;

    while( out_buf[ rr ] != '\0' )
    {
        LLT_CLEN line_start;
        LLT_CLEN i;
        int saw_non_ws;
        int saw_ws;

        line_start = rr;
        saw_non_ws = 0;
        saw_ws     = 0;

        i = rr;

        while( ( out_buf[ i ] != '\n' ) && ( out_buf[ i ] != '\0' ) )
        {
            if( ( out_buf[ i ] != ' ' ) &&
                ( out_buf[ i ] != '\t' ) &&
                ( out_buf[ i ] != '\r' ) )
            {
                saw_non_ws = 1;
            }
            else
            {
                saw_ws = 1;
            }

            i++;
        }

        rr = i;

        if( out_buf[ rr ] == '\n' )
        {
            rr++;
        }

        /* Determine if we should skip the line */
        if( saw_non_ws == 0 )
        {
            /* The line contains ONLY whitespace characters */

            if( saw_ws != 0 )
            {
                /* Line contained whitespace → DELETE */
                continue;
            }

            /* Line contains NOTHING → pure empty line ("\n") → KEEP */
        }

        /* Copy full line */
        {
            LLT_CLEN j;

            j = line_start;

            while( j < rr )
            {
                clean_buf[ ww ] = out_buf[ j ];
                ww++;
                j++;
            }
        }
    }

    clean_buf[ ww ] = '\0';

    /* ------------------------------------------------------------------
     * Write modified output back to the file
     * ------------------------------------------------------------------ */

    FILE * f = fopen( path , "wb" );
    if( f == NULL )
    {
        free( clean_buf );
        free( out_buf );
        free( (void*) csrc.string );
        return ( EXIT_FAILURE );
    }

    {
        size_t wr;

        wr = fwrite( clean_buf , 1u , ww , f );
        fclose( f );

        if( wr != (size_t) ww )
        {
            free( clean_buf );
            free( out_buf );
            free( (void*) csrc.string );
            return ( EXIT_FAILURE );
        }
    }

    free( clean_buf );
    free( out_buf );
    free( (void*) csrc.string );

    ret = EXIT_SUCCESS;

    return ( ret );
}
