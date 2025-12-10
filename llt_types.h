#ifndef LLT_TYPES_H
#define LLT_TYPES_H

typedef unsigned int LLT_COFF;
typedef unsigned int LLT_CLEN;

typedef struct{ LLT_CLEN clen; LLT_CLEN csz; const char * string; } LLT_CSTRING;

typedef enum
{
    LLT_TUID_TYPE_NULL,
    LLT_TUID_TYPE_OPEN,
    LLT_TUID_TYPE_CLOSE,
} LLT_TUID_TYPE;

typedef int LLT_TUID_VALUE;

typedef struct
{
    LLT_COFF        coff;
    LLT_TUID_TYPE   type;
    LLT_TUID_VALUE  value;
} LLT_TUID;

#define LLT_NULL_COFF (-1u)
#define LLT_NULL_TUID_VALUE (0)

#define LLT_TUID_PREFIX "LLT-ID-"
#define LLT_TUID_OPEN  "/* >> " LLT_TUID_PREFIX
#define LLT_TUID_CLOSE "/* << " LLT_TUID_PREFIX

#define LLT_TUID_OPEN_LEN  ( sizeof( LLT_TUID_OPEN  ) - 1Ull )
#define LLT_TUID_CLOSE_LEN ( sizeof( LLT_TUID_CLOSE ) - 1Ull )

extern LLT_TUID llt_find_tuid( const LLT_CSTRING * src, LLT_COFF from );

#endif
