#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "llt_types.h"

static LLT_TUID_VEC * llt_use_case_math_tuids_groups( LLT_TUID_VEC * this, size_t from, size_t to );


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

/*
 * llt_use_case_global_verify
 *
 * - This funciton check that:
 *
 *      1. all TUIDS are balanced
 *      2. any TUID is duplicated (in a sense defined in doc)
 */

extern int llt_use_case_global_verify( void )
{

    return ( 0 );
}

extern LLT_TUID_VEC * llt_use_case_push_tuids( const char * root )
{
    LLT_TUID_VEC * tuids = llt_lib_vector_create( 0 );

    const char * fname;

    size_t prev_len = 0;

    while( ( fname = llt_system_next_file( root ) ) != NULL )
    {
        char auto_path[ 250 ] = { 0 };
        strcat( auto_path, root  );
        strcat( auto_path, fname );

        tuids = llt_lib_vector_push_tuids( tuids, auto_path );

        const size_t current_len = llt_lib_vector_len( tuids );

        if( current_len == prev_len )
        {
            printf("[ERROR] empty file: %s\n", fname );
        }

        else
        {
            /* NOP: do not display nothing if file increments tuids vector */
        }

        prev_len = current_len;
    }

    return ( tuids );
}

extern LLT_TUID_VEC * llt_use_case_match_tuids( LLT_TUID_VEC * this )
{
    const size_t tuids_len = llt_lib_vector_len( this );

    const LLT_TUID * prev_element;
    const LLT_TUID * curr_element;

    if( tuids_len != 0 )
    {
        prev_element = llt_lib_vector_get( this, 0 );
    }

    else
    {
        /* NOP: tuids list is empty */
    }

    /* Match interleaved pairs */
    for( size_t i = 1; i < tuids_len; ++i )
    {
        curr_element = llt_lib_vector_get( this, i );

        if( ( prev_element          != NULL ) && ( curr_element         != NULL ) &&
            ( prev_element->value   != 0    ) && ( curr_element->value  != 0    ) )
        {
            const LLT_TUID_VALUE balance = prev_element->value + curr_element->value;

            if( balance == 0 )
            {
                llt_lib_vector_delete_tuid( this, i - 1u    );
                llt_lib_vector_delete_tuid( this, i         );

                /* Avoid matching interleave +- sequences */
                prev_element = llt_lib_vector_get( this, i + 1 );
                continue;
            }

            else
            {
                /* NOP: do not delete unbalanced TUIDS */
            }
        }

        else
        {
            /* NOP: do not delete null TUIDS */
        }

        prev_element = curr_element;
    }

    /* Math wrap traces */
    size_t i = 0;

    size_t from, to;

    while( i < tuids_len )
    {
        const LLT_TUID * element = llt_lib_vector_get( this, i );

        /* Skip all deletions */
        while( ( element != NULL ) && ( element->state == LLT_TUID_STATE_DELETE ) && ( i < tuids_len ) )
        {
            element = llt_lib_vector_get( this, i );

            from = i;

            ++i;
        }

        /* Skip all actives */
        while( ( element != NULL ) && ( element->state == LLT_TUID_STATE_ACTIVE ) && ( i < tuids_len ) )
        {
            element = llt_lib_vector_get( this, i );

            ++i;
        }

        /* Skip all deletions */
        while( ( element != NULL ) && ( element->state == LLT_TUID_STATE_DELETE ) && ( i < tuids_len ) )
        {
            element = llt_lib_vector_get( this, i );

            ++i;
        }

        /* Skip all actives */
        to = i;

        while( ( element != NULL ) && ( element->state == LLT_TUID_STATE_ACTIVE ) && ( i < tuids_len ) )
        {
            element = llt_lib_vector_get( this, i );

            to = i;

            ++i;
        }

        this = llt_use_case_math_tuids_groups( this, from, to );

        ++i;
    }

    for( size_t i = 0; i < tuids_len; ++i )
    {
//        llt_lib_vector_print( this, i );
    }

    return( this );
}

static LLT_TUID_VEC * llt_use_case_math_tuids_groups( LLT_TUID_VEC * this, size_t from, size_t to )
{
    /* 0. Range is positive */
    const _Bool check_range_is_positive = from < to;

    /* 1. Number of deletes shall be even */
    size_t count_deletes = 0;

    for( size_t i = from; i < to; ++i )
    {
        const LLT_TUID * element = llt_lib_vector_get( this, i );

        if( element->state == LLT_TUID_STATE_DELETE )
        {
            count_deletes++;
        }

        else
        {
            /* NOP: do not count ACTIVE elements */
        }
    }

    const _Bool check_deletes_are_even = ( count_deletes != 0 ) && ( count_deletes % 2 ) == 0;

    /* 2. There is only one deletion inner subgroup */
    const _Bool check_first_is_active = check_range_is_positive && ( llt_lib_vector_get( this, from    )->state == LLT_TUID_STATE_ACTIVE );
    const _Bool check_last_is_active  = check_range_is_positive && ( llt_lib_vector_get( this, to - 1  )->state == LLT_TUID_STATE_ACTIVE );

    const _Bool check_one_deletion_inner_subgroup = check_first_is_active && check_last_is_active;

    /* 3. Open group has the same number of elements thant the close group */
    size_t to_open_group;
    size_t from_close_group;

    _Bool delete_group_reached = false;

    if( check_one_deletion_inner_subgroup )
    {
        for( size_t i = from; i < to; ++i )
        {
            const LLT_TUID * element = llt_lib_vector_get( this, i );

            if( !delete_group_reached && ( element->state == LLT_TUID_STATE_DELETE ) )
            {
                to_open_group           = i;

                delete_group_reached    = true;
            }

            else if( delete_group_reached && ( element->state == LLT_TUID_STATE_ACTIVE ) )
            {
                from_close_group        = i;

                break;
            }

            else
            {
                /* NOP */
            }
        }
    }

    const size_t count_opens    = to_open_group - from;
    const size_t count_closes   = to            - from_close_group;

    const _Bool check_same_open_and_closes = ( count_opens == count_closes );

    /* The number of Open + Closes is multiple of number of Deletions */
    const _Bool check_count_open_close_eq_deletes = ( to_open_group >= from ) && ( to >= from_close_group ) && ( ( count_opens + count_closes ) % count_deletes ) == 0;

    /* The group shall sum zero */
    LLT_TUID_VALUE balance_deletes = 0;
    LLT_TUID_VALUE balance_actives = 0;

    for( size_t i = from; i < to; ++i )
    {
        const LLT_TUID * element = llt_lib_vector_get( this, i );

        if( element != NULL )
        {
            if( element->state == LLT_TUID_STATE_ACTIVE )
            {
                balance_actives += abs( element->value );
            }

            else if( element->state == LLT_TUID_STATE_DELETE )
            {
                balance_deletes += abs( element->value );
            }

            else
            {
                /* NOP: null elements */
            }
        }
    }

    const _Bool check_is_balanced = check_range_is_positive && ( balance_deletes != 0 ) && ( ( balance_actives % balance_deletes ) == 0 );

    /* Check all conditions */
    const _Bool is_success =    check_range_is_positive             &&
                                check_one_deletion_inner_subgroup   &&
                                check_deletes_are_even              &&
                                check_first_is_active               &&
                                check_last_is_active                &&
                                check_same_open_and_closes          &&
                                check_count_open_close_eq_deletes   &&
                                check_is_balanced                   ;

    if( !is_success )
    {
        printf("[ERROR] in group: ");
        int count_prints = 0;
        for( size_t i = from; i < (from + (count_deletes)); ++i )
        {
            const LLT_TUID * element = llt_lib_vector_get( this, i );

            if( element != NULL )
            {
                count_prints++;
                printf("%s%u (%s), ", LLT_TUID_PREFIX, element->value, element->path );
            }
        }
        printf("\n");

        if( count_prints == 0 )
        {
            printf("\tthere are several errors in matching TUIDs, dumping all:\n\t");
            for( size_t i = from; i < to; ++i )
            {
                const LLT_TUID * element = llt_lib_vector_get( this, i );

                if( element != NULL )
                {
                    count_prints++;
                    printf("%s%u (%s), ", LLT_TUID_PREFIX, element->value, element->path );
                }
            }
            printf("\n");
        }
    }

    else
    {

    }

//    printf( "Group: %s, [%zu, %zu)\n", is_success ? "[PASS]" : "[FAIL]", from, to );

    return ( this );
}

/*
 * llt_use_case_export_traces
 *
 * - This funciton export the a document which contains two columns:
 *
 *      1. TUID value
 *      2. Asocciated code to this TUID
 *
 * In a way equivalent to when DOORS export LLR to a CSV.
 *
 */

extern int llt_use_case_export_traces( void )
{

    return ( 0 );
}
