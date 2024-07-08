#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

typedef unsigned char 			u8_t;
typedef unsigned short			u16_t;
typedef unsigned int  			u32_t;
typedef unsigned long long int	u64_t; 
typedef float					f32_t;
typedef double					f64_t;


typedef struct
{
	unsigned char * data;
	size_t len;
	size_t size;
} raw_ptr_t;

typedef struct
{
	float * data;
	size_t 	len;
	size_t 	size;
} f32_ptr_t;

typedef struct
{
	u64_t * data;
	size_t	len;
	size_t	size;
} huffman_table_t;
	

typedef struct
{
	u8_t FTEXT;
	u8_t FHCRC;
	u8_t FEXTRA;
	u8_t FNAME;
	u8_t FCOMMENT;
} FLG_t;	

typedef struct
{
	u8_t	*	FEXTRA;		// 8
	u8_t	*	FNAME;		// 8
	u8_t	*	FCOMMENT;	// 8
	u8_t	*	DEFLATE;	// 8 (start position of compressed blocks)
	FLG_t 		FLG;		// 5
	u32_t		MTIME;		// 4
	u32_t		CRC32;		// 4
	u32_t		ISIZE;		// 4
	u16_t		XLEN;		// 2
	u16_t		CRC16;		// 2
	u8_t 		ID1;		// 1
	u8_t 		ID2;		// 1
	u8_t 		CM;			// 1
	u8_t		XFL;		// 1
	u8_t		OS;			// 1
} GZIP_t;

#define SCOPE_FOPEN(filehandler,fname,mode,code) \
	FILE * fileptr = fopen( fname, mode ); \
	if( fileptr == NULL ) return 1; \
	{ \
		fseek(fileptr,0,SEEK_END); \
		size_t fsize = ftell(fileptr); /* TODO: check for errors, and fsize == 0 */ \
		fseek(fileptr,0,SEEK_SET); \
		raw_ptr_t filehandler = { .data = malloc(fsize), .len = 0, .size = fsize }; \
		if( filehandler.data == NULL ) return 1; \
		fread(filehandler.data,1,filehandler.size,fileptr); \
		code \
		free(filehandler.data); \
		filehandler = (raw_ptr_t) { 0 }; \
	} \
	fclose( fileptr )

int check_gz_type( const raw_ptr_t fp );
void dump_hex( const raw_ptr_t fp );
huffman_table_t create_huffman_table( const char * message, size_t len );
GZIP_t gz_parse_container ( const raw_ptr_t fp );

int main( int argc, char ** argv )
{
	printf("%llu\n", sizeof(unsigned long long int) );
	
	SCOPE_FOPEN( fp, argv[1], "rb",

	dump_hex( fp );
	
	if ( check_gz_type( fp ) )
	{
		printf("Is GZ\n");
	}
	
	GZIP_t metadata = gz_parse_container( fp );
	
	printf("ID1: 	%u\n", metadata.ID1);
	printf("ID2: 	%u\n", metadata.ID2);
	printf("CM: 	%u\n", metadata.CM);
	printf(".FTEXT: 	%u\n", metadata.FLG.FTEXT);
	printf(".FHCRC: 	%u\n", metadata.FLG.FHCRC);
	printf(".FEXTRA: 	%u\n", metadata.FLG.FEXTRA);
	printf(".FNAME: 	%u\n", metadata.FLG.FNAME);
	printf(".FCOMMENT: 	%u\n", metadata.FLG.FCOMMENT);
	printf("MTIME: 	%u\n", metadata.MTIME);
	printf("XFL: 	%u\n", metadata.XFL);
	printf("OS: 	%u\n", metadata.OS);
	printf("FNAME: 	%s\n", metadata.FNAME);
	printf("DEFLATE: %02X %02X %02X %02X\n", metadata.DEFLATE[0],metadata.DEFLATE[1],metadata.DEFLATE[2],metadata.DEFLATE[3]);

	const u8_t * deflate = metadata.DEFLATE;
	
	const u8_t BFINAL	= ( deflate[0] & 0x01 ) >> 0;
	const u8_t BTYPE	= ( deflate[0] & 0x06 ) >> 1;

	printf("BFINAL:	%u\n", BFINAL);
	printf("BTYPE:	%u, %u%u\n", BTYPE, (BTYPE & 0x02) >> 1 , (BTYPE & 0x1) );
	
	huffman_table_t result = create_huffman_table( "Hello this is a message", strlen("Hello this is a message") );
	
	);//SCOPE_FOPEN
	
	return 0;
}

void dump_hex( const raw_ptr_t fp )
{
	size_t j = 0;
	printf("%08X ", 0);
	for( size_t i = 0; i < fp.size; i++ )
	{

		printf("%02X ", fp.data[i]);
		j++;
		
		if( j == 16 )
		{
			printf(" ");
			while( j != 0 )
			{
				const char c = fp.data[i - j];
				if( c >= ' ' && c <= '~' )
					printf("%c", fp.data[i - j]);
				else
					printf(".");
				j--;
			}
			printf("\n");
			printf("%08X ", i);
		}
	}

	printf("\n");
}

int check_gz_type( const raw_ptr_t fp )
{
	if( fp.size < 16 )
	{
		return 0;
	}
	
	return ( fp.data[0] == 0x1F ) && ( fp.data[1] == 0x8B );
}

huffman_table_t create_huffman_table( const char * message, size_t len )
{
	static size_t temp_count_table [ 256 ] = { 0 };

	memset( temp_count_table, 256, 1 );

	for( size_t i = 0; i < len; i++ )
	{
		temp_count_table[ message[i] ]++;
	}
	
	for( int i = 0; i < 256; i++ )
	{
		if( temp_count_table[i] != 0 )
		{
			if( i >= ' ' && i <= '~' )
			{
				printf("%c, %zu\n", (char) i, temp_count_table[i] );
			}
			else
			{
				printf("%d, %zu\n", i, temp_count_table[i] );
			}
		}
	}
}

GZIP_t gz_parse_container ( const raw_ptr_t fp )
{	
	FLG_t flg =
	{
		.FTEXT 		= (fp.data[3] & (1<<0)) >> 0,
		.FHCRC 		= (fp.data[3] & (1<<1)) >> 1,
		.FEXTRA 	= (fp.data[3] & (1<<2)) >> 2,
		.FNAME 		= (fp.data[3] & (1<<3)) >> 3,
		.FCOMMENT 	= (fp.data[3] & (1<<4)) >> 4,
	};

	GZIP_t result =
	{
		.ID1 = fp.data[0],
		.ID2 = fp.data[1],
		.CM  = fp.data[2],
		.FLG = flg,
		.MTIME = fp.data[7] << 24 | fp.data[6] << 16 | fp.data[5] << 8 | fp.data[4],
		.XFL	= fp.data[8],
		.OS		= fp.data[9],
		// TODO: implement the different cases of this logic. Now, FNAME the position of FNAME and DEFLATE is almost hardcoded
		.XLEN	= 0, //flg.FEXTRA ? fp.data[10] 	: 0
		.FEXTRA		= NULL, //flg.FEXTRA ? fp.data + 11 : NULL,
		.FNAME		= flg.FNAME  ? fp.data + 10 : NULL,
		.FCOMMENT	= NULL,
		.DEFLATE	= 1 + fp.data + 10 + strlen( fp.data + 10 ),
	};

	return result;
}

//raw_ptr_t gz_view_deflated ( const raw_ptr_t fp )
