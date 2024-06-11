#include <assert.h>
#include <malloc.h>
#include <string.h>
#include <stdio.h>

#include "array.h"

#define ERR_NULL_THIS 1
#define ERR_NULL_DATA 2
#define ERR_OUT_BOUND 4

static int array_errno = 0;

extern void * _safe_eval ( void * this, size_t size )
{
    assert( size <= MAX_SIZE_GENERIC_TYPE && "ERROR: generic type size is larger than max supported\n" );
    static unsigned char null_value [ MAX_SIZE_GENERIC_TYPE ] = { 0 };

    if ( this == NULL )
    {
        array_errno = 1; /* TODO: mechanism to emitt errors is still not defined */
        return null_value;
    }

    return this;
}

extern void * _array_ctor ( size_t length, size_t element_size )
{
    const size_t data_size = length * element_size;
    void * this = malloc ( data_size + _ARRAY_HEADER_SIZE );

    if ( this == NULL )
    {
        return NULL;
    }

    (( size_t * ) this )[ 0 ] = length;
    (( size_t * ) this )[ 1 ] = element_size;

    return this + _ARRAY_HEADER_SIZE;
}

extern void _array_dtor ( void ** ptr_this )
{
    if ( ptr_this == NULL )
    {
        return;
    }

    void * this = *( ptr_this - _ARRAY_HEADER_SIZE );

    if ( this == NULL )
    {
        return;
    }

    free ( this );

    this = NULL;
}

extern void * _array_at ( void * this, size_t index, int * error )
{
    if ( this == NULL )
    {
        *error = ERR_NULL_THIS;
        return NULL;
    }

    const size_t length         = *( ( size_t * ) (this - 16) );
    const size_t element_size   = *( ( size_t * ) (this - 8))   ;

    if ( index >= length )
    {
        *error = ERR_OUT_BOUND;
        return NULL;
    }

    return this + ( index * element_size );
}

extern void _array_push ( void * this, size_t index, const void * src, int * error )
{
    if ( this == NULL )
    {
        *error = ERR_NULL_THIS;
        return;
    }

    const size_t length         = *( ( size_t * ) (this - 16));
    const size_t element_size   = *( ( size_t * ) (this - 8 ));

    if ( index >= length )
    {
        *error = ERR_OUT_BOUND;
        return;
    }

    memcpy( this + ( index * element_size ) , src , element_size );
}
