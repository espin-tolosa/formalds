#include <stdio.h>
#include <array.h>

typedef struct
{
    int x;
    int y;
    int z;
}
    vec3_t;

int main ( void )
{
    vec3_t * my_array = ARRAY_CTOR( 10, vec3_t );

    for(int i = 0; i < 100; i++)
    {
        int error           = ERR_SAFE;
        const vec3_t init_value   = { 0 };

        ARRAY_PUSH( my_array, i, &init_value, &error );

        if ( error != ERR_SAFE )
        {
            break;
        }
    }

    for(int i = 0; i < 20; i++)
    {
        int error       = ERR_SAFE;
        vec3_t value    = ARRAY_AT( my_array, i, &error );

        if ( error != ERR_SAFE )
        {
            break;
        }

        printf( "%d %d %d\n", value.x, value.y, value.z );
    }

    ARRAY_DTOR ( my_array );
}
