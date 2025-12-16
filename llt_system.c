#include <stddef.h>

#include "../llt_types.h"

#include <stddef.h>

#if defined( _WIN32 ) || defined( _WIN64 )

    #include <windows.h>

    extern const char * llt_system_next_file ( const char * root )
    {
        static HANDLE            h_find = INVALID_HANDLE_VALUE;
        static WIN32_FIND_DATAA  find_data;
        static char              name_buffer[ MAX_PATH ];
        static char              search_pattern[ MAX_PATH ];
        static const char *      current_root = NULL;

        if( root != current_root )
        {
            if( h_find != INVALID_HANDLE_VALUE )
            {
                FindClose( h_find );
                h_find = INVALID_HANDLE_VALUE;
            }

            current_root = root;

            if( root == NULL )
            {
                return ( NULL );
            }

            {
                char * dst = search_pattern;
                const char * src = root;

                while( *src != '\0' )
                {
                    *dst++ = *src++;
                }

                if(
                    ( dst != search_pattern ) &&
                    ( dst[ -1 ] != '\\' ) &&
                    ( dst[ -1 ] != '/' )
                )
                {
                    *dst++ = '\\';
                }

                *dst++ = '*';
                *dst   = '\0';
            }

            h_find = FindFirstFileA( search_pattern, &find_data );

            if( h_find == INVALID_HANDLE_VALUE )
            {
                return ( NULL );
            }
        }

        else
        {
            if( h_find == INVALID_HANDLE_VALUE )
            {
                return ( NULL );
            }

            if( FindNextFileA( h_find, &find_data ) == 0 )
            {
                FindClose( h_find );
                h_find = INVALID_HANDLE_VALUE;

                return ( NULL );
            }
        }

        /* Skip "." and ".." */
        if(
            ( find_data.cFileName[ 0 ] == '.' ) &&
            (
                ( find_data.cFileName[ 1 ] == '\0' ) ||
                (
                    ( find_data.cFileName[ 1 ] == '.' ) &&
                    ( find_data.cFileName[ 2 ] == '\0' )
                )
            )
        )
        {
            return ( llt_system_next_file( root ) );
        }

        /* Skip directories */
        if( ( find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0 )
        {
            return ( llt_system_next_file( root ) );
        }

        /* Copy name */
        {
            char * dst = name_buffer;
            const char * src = find_data.cFileName;

            while( *src != '\0' )
            {
                *dst++ = *src++;
            }

            *dst = '\0';
        }

        return ( name_buffer );
    }

#else /* POSIX */

    #include <dirent.h>
    #include <string.h>

    extern const char * llt_system_next_file ( const char * root )
    {
        static DIR *             dir = NULL;
        static struct dirent *   ent;
        static char              name_buffer[ 256 ];
        static const char *      current_root = NULL;

        if( root != current_root )
        {
            if( dir != NULL )
            {
                closedir( dir );
                dir = NULL;
            }

            current_root = root;

            if( root == NULL )
            {
                return ( NULL );
            }

            dir = opendir( root );

            if( dir == NULL )
            {
                return ( NULL );
            }
        }

        for( ;; )
        {
            if( dir == NULL )
            {
                return ( NULL );
            }

            ent = readdir( dir );

            if( ent == NULL )
            {
                closedir( dir );
                dir = NULL;

                return ( NULL );
            }

            /* Skip "." and ".." */
            if(
                ( ent->d_name[ 0 ] == '.' ) &&
                (
                    ( ent->d_name[ 1 ] == '\0' ) ||
                    (
                        ( ent->d_name[ 1 ] == '.' ) &&
                        ( ent->d_name[ 2 ] == '\0' )
                    )
                )
            )
            {
                continue;
            }

            /* Skip non-regular files when possible */
            #ifdef DT_REG
            if( ent->d_type != DT_REG && ent->d_type != DT_UNKNOWN )
            {
                continue;
            }
            #endif

            /* Copy name */
            {
                char * dst = name_buffer;
                const char * src = ent->d_name;

                while( *src != '\0' )
                {
                    *dst++ = *src++;
                }

                *dst = '\0';
            }

            return ( name_buffer );
        }
    }

#endif
