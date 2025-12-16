#include "../llt_types.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_CAPACITY SIZE_MAX

typedef struct llt_tuid_vec
{
    size_t      len;
    size_t      cap;
    LLT_TUID    vec[ ];
}
    LLT_TUID_VEC;

static size_t llt_lib_vector_sizeof( size_t capacity );
static size_t llt_lib_vector_safe_grow_capacity( size_t current_capacity );

extern LLT_TUID_VEC * llt_lib_vector_create ( size_t capacity )
{
    LLT_TUID_VEC * ret;

    ret         = malloc( llt_lib_vector_sizeof( capacity ) );

    if( ret != NULL )
    {
        ret->cap    = capacity;
        ret->len    = 0;
    }

    else
    {
        /* NOP: ret is NULL, no need to set .cap, .len */
    }

    return ( ret );
}

extern void llt_lib_vector_destroy ( LLT_TUID_VEC * this )
{
    free( this );
}

extern LLT_TUID_VEC * llt_lib_vector_push ( LLT_TUID_VEC * this, LLT_TUID element )
{
    LLT_TUID_VEC * ret;

    void * tmp_realloc;

    size_t tmp_cap;

    if( this == NULL )
    {
        tmp_realloc = NULL;
    }

    else
    {
        if( ( this->len == this->cap ) )
        {
            tmp_cap = this->cap;

            if( this->cap == 0 )
            {
                tmp_cap = 1;
            }

            else
            {
                tmp_cap = llt_lib_vector_safe_grow_capacity( this->cap );
            }

            tmp_realloc = realloc( this, llt_lib_vector_sizeof( tmp_cap ) );
        }

        else
        {
            /* NOP: no need to realloc - copy `this` to `tmp_realloc` allows to check for NULL to detect the cases when realloc failed */
            tmp_realloc = this;

            tmp_cap     = this->cap;
        }

        if( tmp_realloc == NULL )
        {
            /* NOP: realloc failed or `this` came NULL */
            ret = this;
        }

        else
        {
            ret                     = tmp_realloc;
            this                    = tmp_realloc;
            this->cap               = tmp_cap;
            this->vec[ this->len ]  = element;
            this->len               = 1 + this->len;
        }
    }

    return ( ret );
}

extern const LLT_TUID * llt_lib_vector_get ( LLT_TUID_VEC * this, size_t at )
{
    LLT_TUID * ret = NULL;

    if( ( this != NULL ) && ( at < this->len ) )
    {
        ret = &this->vec[ at ];
    }

    else
    {
        /* NOP: ret is already set NULL */
    }

    return ( ret );
}

extern size_t llt_lib_vector_len ( LLT_TUID_VEC * this )
{
    size_t ret;

    if( this == NULL )
    {
        ret = 0;
    }

    else
    {
        ret = this->len;
    }

    return ( ret );
}

extern void llt_lib_vector_print( LLT_TUID_VEC * this, size_t at )
{
    const LLT_TUID * tuid = llt_lib_vector_get( this, at );

    if( tuid != NULL )
    {
        printf( "TUID: { .state = %d, .coff = %4u, .type = %1d, .value = %4d, .path = %s }\n", tuid->state, tuid->coff, tuid->type, tuid->value, tuid->path );
    }

    else
    {
        printf( "TUID: { NULL }\n" );
    }
}

static size_t llt_lib_vector_sizeof( size_t capacity )
{
    const size_t fix_size =             sizeof( LLT_TUID_VEC    );
    const size_t var_size = capacity *  sizeof( LLT_TUID        );

    return ( fix_size + var_size );
}

static size_t llt_lib_vector_safe_grow_capacity( size_t current_capacity )
{
    size_t ret;

    if( current_capacity > ( MAX_CAPACITY / 2 ) )
    {
        ret = 0;
    }

    else if( current_capacity > ( ( MAX_CAPACITY - sizeof(LLT_TUID_VEC) ) / sizeof(LLT_TUID) ) )
    {
        ret = 0;
    }

    else if( current_capacity == 0 )
    {
        ret = 1;
    }

    else
    {
        ret = 2 * current_capacity;
    }

    return ( ret );
}

extern LLT_TUID_VEC * llt_lib_vector_push_tuids( LLT_TUID_VEC * this, const char * fname )
{
    LLT_CSTRING csrc        = llt_load_file_as_cstring( fname );

    LLT_TUID id_offset_prev = { .coff = 0, /* */ };
    LLT_TUID id_offset_curr = { .coff = 0, /* */ };

    do
    {
        id_offset_prev = id_offset_curr;

        id_offset_curr      = llt_find_tuid( &csrc, id_offset_prev.coff );

        strcpy( id_offset_curr.path, fname );

        id_offset_curr.state = LLT_TUID_STATE_ACTIVE;

        if( id_offset_curr.value != LLT_NULL_TUID_VALUE )
        {
            this = llt_lib_vector_push( this, id_offset_curr );
        }

        else
        {
            /* NOP: do not insert null tuid values */
        }
    }

    while ( ( id_offset_prev.coff < id_offset_curr.coff ) && ( id_offset_curr.coff != LLT_NULL_COFF ) );

    return ( this );
}

extern void llt_lib_vector_delete_tuid( LLT_TUID_VEC * this, size_t at )
{
    if( ( this != NULL ) && ( at < this->len ) )
    {
        this->vec[ at ].state = LLT_TUID_STATE_DELETE;
    }

    else
    {
        /* NOP: ret is already set NULL */
    }
}
