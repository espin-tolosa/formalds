#ifndef FDS_ARRAY_H
#define FDS_ARRAY_H

#define ERR_SAFE              0
#define MAX_SIZE_GENERIC_TYPE 1080 /* redefinible property */

#define _ARRAY_HEADER_SIZE ( 2 * sizeof( size_t ) )
#define _MAX_SIZE_GENERIC_TYPE 1080
#define _PUNNING(type) * ( type * )
#define _SAFE_EVAL(type, ptr) _PUNNING( type ) ( _safe_eval( ptr, sizeof( type ) ) )

#define ARRAY_CTOR(elements, type) ( type * ) _array_ctor( elements, sizeof( type ) )
#define ARRAY_DTOR( ptr_this ) _array_dtor ( (void**) &ptr_this )
#define ARRAY_AT(this, index, ptr_error) _SAFE_EVAL( typeof(this[0]), _array_at( (void*) this, i, ptr_error ) )
#define ARRAY_PUSH(this, index, value, ptr_error) _array_push( this, index, (const void * ) value, ptr_error )

extern void *   _array_ctor ( size_t  length     , size_t element_size                              );
extern void     _array_dtor ( void ** ptr_this                                                      );
extern void *   _safe_eval  ( void *      this   , size_t size                                      );
extern void     _array_push ( void *      this   , size_t index  , const void * src, int * error    );
extern void *   _array_at   ( void *      this   , size_t index  , int * error                      );

#endif/*FDS_ARRAY_H*/